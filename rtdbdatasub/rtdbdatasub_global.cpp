#include "rtdbdatasub_global.h"
#include <QUuid>
#include <zmq.h>
#include "onesubscriber.h"
#include "rtdbdata.h"
#include "rtdbdatasub_errorcode.h"
#include "rtdbrouter_protocol.h"
#include <iostream>
#include <stdlib.h>
#include <QLibrary>
#include "icssettings.h"

#ifdef Q_OS_LINUX
#include <arpa/inet.h>
#endif

//Gaia rtdb Field's Data Type
#define  GDB_DT_UNKNOWN  0 // unknown or undetermined
#define  GDB_DT_INT  1     // integer
#define  GDB_DT_FLOAT 2   // floating point
#define  GDB_DT_DATE  3    // date
#define  GDB_DT_TIME  4    // time
#define  GDB_DT_BOOL  5    // boolean (true or false)
#define  GDB_DT_BIT  6     // bit holder
#define  GDB_DT_MASK 7    // bit mask
#define  GDB_DT_CHAR 8  // character string

CRTDBDATASUB_GLOBAL* CRTDBDATASUB_GLOBAL::g_sCRTDBDATASUB_GLOBAL = NULL;


CRTDBDATASUB_GLOBAL* CRTDBDATASUB_GLOBAL::Instance()
{
	if ( g_sCRTDBDATASUB_GLOBAL == NULL )
	{
		g_sCRTDBDATASUB_GLOBAL = new CRTDBDATASUB_GLOBAL();
	}

	return g_sCRTDBDATASUB_GLOBAL;
}

CRTDBDATASUB_GLOBAL::~CRTDBDATASUB_GLOBAL()
{
	trdUpdateField.stop();
	trdUpdateField.wait();
	zmq_close (socket_snd);
	zmq_close (socket_rcv);
    zmq_ctx_destroy (zmq_context);
}


CRTDBDATASUB_GLOBAL::CRTDBDATASUB_GLOBAL()
{
	zmq_context = NULL;
	socket_rcv = NULL;
	socket_snd = NULL;
	clientUUID = QUuid::createUuid().toString();

	hasInitilized = false;
	m_nRefCount = 0;
	memset(subscribersArray, 0, sizeof(subscribersArray));
}

void CRTDBDATASUB_GLOBAL::AddRefCount()
{
	++m_nRefCount;
}

void CRTDBDATASUB_GLOBAL::ReleaseRef()
{
	--m_nRefCount;
	if ( m_nRefCount == 0)
		delete this;
}

bool CRTDBDATASUB_GLOBAL::LoadConfig()
{
 //<section description="系统平台" key="plat_sys">
 //  <section description="实时库路由器" key="rtdbrouter">
 //    <item description="IP地址" key="ipaddress" modify="false" value="192.168.0.254" type="String"/>
 //    <item description="服务端发送端口" key="sendportforserver" modify="false" value="5555" type="Int"/>
 //    <item description="服务端订阅端口" key="subscriberportforserver" modify="false" value="5556" type="Int"/>
 //    <item description="客户端发送端口" key="sendportforclient" modify="false" value="6666" type="Int"/>
 //    <item description="客户端订阅端口" key="subscriberportforclient" modify="false" value="6667" type="Int"/>
 //  </section>

	QLibrary icssettingLib("icssettings");
	if ( icssettingLib.load() == false )
	{
		printf("load icssetings failed\n");
		return false;
	}

	typedef bool (*PMETHOD)(char*, IICSUnknown**);

	PMETHOD pGetClassObject = (PMETHOD) icssettingLib.resolve("GetClassObject");

	if(!pGetClassObject)
	{
		printf("Resolve function \"GetClassobject\" failed!\n");
		return false;
	}

	IICSUnknown * pUnknown = NULL;
	(*pGetClassObject)(CLSID_ICSSettings,(IICSUnknown**)&pUnknown);
	if(pUnknown == NULL)
	{
		printf("get ICSSetting interface failed\n");
		return false;
	}
	
	IICSSettings* pSettings = NULL;
	pUnknown->QueryInterface(IID_ICSSettings, (void**)&pSettings);
	pUnknown->Release();
	if(pSettings == NULL)
	{
		printf("get ICSSetting interface failed\n");
		return false;
	}

	char szBuffer[1024];
	bool bOK = false;
	bOK = pSettings->SetCurrentSectionPath("plat_sys/rtdbrouter");
	if ( !bOK )
	{
		printf("please add plat_sys/rtdbrouterport section in icssetting.xml file\n");
		return false;
	}
	pSettings->ItemReadString("ipaddress", szBuffer, sizeof(szBuffer));
	int nSendPort = pSettings->ItemReadInt("sendportforclient");
	int nSubscriberPort = pSettings->ItemReadInt("subscriberportforclient");

	pSettings->Release();

	sendAddress = QString("tcp://").append(szBuffer).append(":") + QString::number(nSendPort);
	subcriberAddress = QString("tcp://").append(szBuffer).append(":") + QString::number(nSubscriberPort);
	return true;
}

bool CRTDBDATASUB_GLOBAL::Initilize()
{
	if ( hasInitilized == true)
		return true;

	if ( LoadConfig() == false )
		return false;
 
	if ( zmq_context == NULL )
		zmq_context = zmq_ctx_new();

	if ( zmq_context == NULL )
		return false;

	if ( socket_rcv == NULL )
	    socket_rcv = zmq_socket (zmq_context, ZMQ_SUB);
	if ( socket_rcv == NULL )
		return false;
	zmq_connect (socket_rcv, subcriberAddress.toLatin1().constData());
	zmq_setsockopt (socket_rcv, ZMQ_SUBSCRIBE, clientUUID.toLatin1().constData(), clientUUID.length());

	if ( socket_snd == NULL )
		socket_snd = zmq_socket(zmq_context, ZMQ_PUSH);
	if ( socket_snd == NULL )
		return false;
	zmq_connect (socket_snd, sendAddress.toLatin1().constData());

	hasInitilized = true;
	trdUpdateField.start();
	trdRefreshBufferData.start();
	return true;
}

int CRTDBDATASUB_GLOBAL::CreateSubscriptionHandle()
{
	COneSubscriber* pSubscriber = new COneSubscriber();
	for ( int i = 0; i < MAX_SUBSCRIPTION_COUNT; ++i)
	{
		if ( subscribersArray[i] == 0 )
		{
			subscribersArray[i] = pSubscriber;
			return i + 1;
		}
	}

	return 0;
}

QString CRTDBDATASUB_GLOBAL::BuildKey( const char* fam, const char* app, const char* db, const char* table, const char* field, int row)
{
	QString strKey = fam;
	strKey.append("_").append(app);
	strKey.append("_").append(db);
	strKey.append("_").append(table);
	strKey.append("_").append(field);
	strKey.append("_").append(QString::number(row));
	return strKey.toUpper();
}

bool CRTDBDATASUB_GLOBAL::AddSubRTDBData(int handle, const char* fam, const char* app, const char* db, const char* table, const char* field, int row)
{
	QMutexLocker locker(&mtDataMap);
	CRTDBDATA* pRtdbData = NULL;
	QString strKey = BuildKey(fam, app, db, table, field, row);
	QMap<QString, CRTDBDATA*>::const_iterator iterator = rtdbDataMap.find(strKey);
	if ( iterator != rtdbDataMap.end() )
	{
		pRtdbData = iterator.value();
	}
	else
	{
		pRtdbData = new CRTDBDATA(fam, app, db, table, field, row);
		rtdbDataMap[strKey] = pRtdbData;
		pRtdbData->strKey = strKey;
	}

	subscribersArray[handle - 1]->AddRtdbData(pRtdbData);
	return true;
}

bool CRTDBDATASUB_GLOBAL::RegisterSubscript(int handle)
{
	QMutexLocker locker(&mtPulisherSocket);
	int rtn = 0;
	COneSubscriber* pSubscriber = subscribersArray[handle - 1];
	rtn = SendProtocol(RTDBROUTER_PROTOCOL_REGISTERSUBSCRIPT);
	rtn = SendQString(pSubscriber->m_strUUID, ZMQ_SNDMORE);

	int nListCount = pSubscriber->mRtdbDataList.size();
	for (int i = 0; i < nListCount; ++i) 
	{
		CRTDBDATA* pRtdbData = pSubscriber->mRtdbDataList[i];
		SendQString(pRtdbData->strFam, ZMQ_SNDMORE);
		SendQString(pRtdbData->strApp, ZMQ_SNDMORE);
		SendQString(pRtdbData->strDB, ZMQ_SNDMORE);
		SendQString(pRtdbData->strTable, ZMQ_SNDMORE);
		SendQString(pRtdbData->strField, ZMQ_SNDMORE);
		SendInt4(pRtdbData->nRow, ZMQ_SNDMORE);
		//SendQString(pRtdbData->strRow, ZMQ_SNDMORE);
	}
	//CRTDBDATA* pLastData = pSubscriber->mRtdbDataList[nListCount];
	//SendQString(pLastData->strFam, ZMQ_SNDMORE);
	//SendQString(pLastData->strApp, ZMQ_SNDMORE);
	//SendQString(pLastData->strDB, ZMQ_SNDMORE);
	//SendQString(pLastData->strTable, ZMQ_SNDMORE);
	//SendQString(pLastData->strField, ZMQ_SNDMORE);
	rtn = SendQString("END", 0);//结束标志
	return rtn != -1;
}

bool CRTDBDATASUB_GLOBAL::RefreshBufferedData()
{
	QMutexLocker locker(&mtPulisherSocket);

	QMutexLocker lockerDataMap(&mtDataMap);

	if ( rtdbDataMap.size() == 0 )
		return true;

	int rtn = 0;
	rtn = SendProtocol(RTDBROUTER_PROTOCOL_REGISTERSUBSCRIPT);

	QMap<QString,CRTDBDATA*>::iterator it = rtdbDataMap.begin();

	while(it != rtdbDataMap.end() )
	{
		CRTDBDATA* pRtdbData = it.value();
		if ( pRtdbData->CountDown() == 0)
		{
			it = rtdbDataMap.erase(it);
			delete pRtdbData;
			continue;
		}
		SendQString(pRtdbData->strFam, ZMQ_SNDMORE);
		SendQString(pRtdbData->strApp, ZMQ_SNDMORE);
		SendQString(pRtdbData->strDB, ZMQ_SNDMORE);
		SendQString(pRtdbData->strTable, ZMQ_SNDMORE);
		SendQString(pRtdbData->strField, ZMQ_SNDMORE);
		SendInt4(pRtdbData->nRow, ZMQ_SNDMORE);
		++it;
	}

	rtn = SendQString("END", 0);//结束标志
	return rtn != -1;
}

bool CRTDBDATASUB_GLOBAL::CancelSubscript(int handle)
{
	mtPulisherSocket.lock();
	int rtn = 0;
	COneSubscriber* pSubscriber = subscribersArray[handle - 1];
	rtn = SendProtocol(RTDBROUTER_PROTOCOL_CANCELSUBSCRIPT);
	rtn = SendQString(pSubscriber->m_strUUID, 0);
	mtPulisherSocket.unlock();

	QMutexLocker locker(&mtDataMap);
	delete pSubscriber;
	subscribersArray[handle - 1] = NULL;

	int nRefCount = 0;
	QMap<QString,CRTDBDATA*>::iterator itDataMap = rtdbDataMap.begin();
	while(itDataMap != rtdbDataMap.end() )
	{
		CRTDBDATA* pRtdbData = itDataMap.value();
		nRefCount = pRtdbData->GetRefCount();
		if ( nRefCount == 0)
		{
			itDataMap = rtdbDataMap.erase(itDataMap);
			delete pRtdbData;
		}
	}

	return rtn != -1;
}



bool CRTDBDATASUB_GLOBAL::RefreshSubscript(int handle)
{
	return RegisterSubscript(handle);
	//QMutexLocker locker(&mtPulisherSocket);
	//int rtn = 0;
	//COneSubscriber* pSubscriber = subscribersArray[handle - 1];
	//rtn = SendProtocol(RTDBROUTER_PROTOCOL_REFRESHSUBSCRIPT);
	//rtn = SendQString(pSubscriber->m_strUUID, 0);
	//return rtn != -1;
}

int CRTDBDATASUB_GLOBAL::ReadFieldQVariant(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, QVariant* value)
{
	QMutexLocker locker(&mtDataMap);

	CRTDBDATA* pRtdbData = NULL;
	QString strKey = BuildKey(fam, app, db, table, field, row);

	QMap<QString, CRTDBDATA*>::const_iterator iterator = rtdbDataMap.find(strKey);
	if ( iterator == rtdbDataMap.end() )
	{
		pRtdbData = new CRTDBDATA(fam, app, db, table, field, row);
		rtdbDataMap[strKey] = pRtdbData;
		pRtdbData->strKey = strKey;
	}
	else
	{
		pRtdbData = iterator.value();
	}
	pRtdbData->ResetCounter();//读一次记一次数

	QVariant curValue = pRtdbData->GetValue();
	if ( curValue.isValid() == false )
	{
		std::cout << rtdbdatasub_strerror(RETURNCODE_RTDBROUTER_NOTREFRESH) << std::endl;

		return RETURNCODE_RTDBROUTER_NOTREFRESH;
	}

	*value = curValue;
	return RETURNCODE_RTDBROUTER_SUCCESS;
}

int CRTDBDATASUB_GLOBAL::ReadField(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, void* buffer, int buffersize)
{
	QVariant curValue;
	int nReturnCode = ReadFieldQVariant(fam, app, db, table, field, row, &curValue);
	if ( nReturnCode < 0 )
	{
		return nReturnCode;
	}

	int nValue;
	double dValue;
	QString strValue;
	size_t nLength;
	switch ( curValue.type())
	{
	case QVariant::Int:
		nValue = curValue.toInt();
		nLength = sizeof(nValue) > buffersize ? buffersize : sizeof(nValue);
		memcpy(buffer, &nValue, nLength);
		break;
	case QVariant::Double:
		dValue = curValue.toDouble();
		nLength = sizeof(dValue) > buffersize ? buffersize : sizeof(dValue);
		memcpy(buffer, &dValue, nLength);
		break;
	case QVariant::String:
		strValue = curValue.toString();
		nLength = strValue.length() >= buffersize ? buffersize - 1 : strValue.length() + 1;
		memcpy(buffer, strValue.toLatin1().constData(), nLength);
	}

	return RETURNCODE_RTDBROUTER_SUCCESS;
}

int CRTDBDATASUB_GLOBAL::ReadFieldInt2(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, short* value)
{
	QVariant curValue;
	int nReturnCode = ReadFieldQVariant(fam, app, db, table, field, row, &curValue);
	if ( nReturnCode < 0 )
	{
		return nReturnCode;
	}

	*value = curValue.toInt();
	return RETURNCODE_RTDBROUTER_SUCCESS;
}

int CRTDBDATASUB_GLOBAL::ReadFieldInt4(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, int* value)
{
	QVariant curValue;
	int nReturnCode = ReadFieldQVariant(fam, app, db, table, field, row, &curValue);
	if ( nReturnCode < 0 )
	{
		return nReturnCode;
	}

	*value = curValue.toInt();
	return RETURNCODE_RTDBROUTER_SUCCESS;
}

int CRTDBDATASUB_GLOBAL::ReadFieldInt8(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, long long* value)
{
	QVariant curValue;
	int nReturnCode = ReadFieldQVariant(fam, app, db, table, field, row, &curValue);
	if ( nReturnCode < 0 )
	{
		return nReturnCode;
	}

	*value = curValue.toLongLong();
	return RETURNCODE_RTDBROUTER_SUCCESS;
}

int CRTDBDATASUB_GLOBAL::ReadFieldFloat(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, float* value)
{
	QVariant curValue;
	int nReturnCode = ReadFieldQVariant(fam, app, db, table, field, row, &curValue);
	if ( nReturnCode < 0 )
	{
		return nReturnCode;
	}

	*value = curValue.toFloat();
	return RETURNCODE_RTDBROUTER_SUCCESS;
}

int CRTDBDATASUB_GLOBAL::ReadFieldDouble(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, double* value)
{
	QVariant curValue;
	int nReturnCode = ReadFieldQVariant(fam, app, db, table, field, row, &curValue);
	if ( nReturnCode < 0 )
	{
		return nReturnCode;
	}

	*value = curValue.toDouble();
	return RETURNCODE_RTDBROUTER_SUCCESS;
}

int CRTDBDATASUB_GLOBAL::ReadFieldString(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, char* buffer, int buffersize)
{
	QVariant curValue;
	int nReturnCode = ReadFieldQVariant(fam, app, db, table, field, row, &curValue);
	if ( nReturnCode < 0 )
	{
		return nReturnCode;
	}

	memcpy(buffer, curValue.toByteArray().constData(), buffersize);

	return RETURNCODE_RTDBROUTER_SUCCESS;
}

int CRTDBDATASUB_GLOBAL::ReadFieldDateTime(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, double* value)
{
	QVariant curValue;
	int nReturnCode = ReadFieldQVariant(fam, app, db, table, field, row, &curValue);
	if ( nReturnCode < 0 )
	{
		return nReturnCode;
	}

	*value = curValue.toDouble();
	return RETURNCODE_RTDBROUTER_SUCCESS;
}

int CRTDBDATASUB_GLOBAL::SendProtocol(const char* protocol)
{
	zmq_send(socket_snd, protocol, strlen(protocol), ZMQ_SNDMORE);
	//zmq_msg_t message_protocol;
	//zmq_msg_init_size(&message_protocol, strlen(protocol));
	//memcpy(zmq_msg_data(&message_protocol), protocol, strlen(protocol));
	//zmq_msg_send(&message_protocol, socket_snd, ZMQ_SNDMORE);

	int rtn = zmq_send(socket_snd, clientUUID.toLatin1().constData(), clientUUID.length(), ZMQ_SNDMORE);
	//zmq_msg_t message_clientuuid;
	//zmq_msg_init_size(&message_clientuuid, clientUUID.length());
	//memcpy(zmq_msg_data(&message_clientuuid), clientUUID.toLatin1().constData(), clientUUID.length());
	//int rtn = zmq_msg_send(&message_clientuuid, socket_snd, ZMQ_SNDMORE);
	return rtn;
}

int CRTDBDATASUB_GLOBAL::SendQString(QString str, int flags)
{
	int rtn = zmq_send(socket_snd,str.toLatin1().constData(), str.length(), flags);
	//zmq_msg_t message;
	//zmq_msg_init_size(&message, str.length());
	//memcpy(zmq_msg_data(&message), str.toLatin1().constData(), str.length());
	//int rtn = zmq_msg_send(&message, socket_snd, flags);
	return rtn;
}

int CRTDBDATASUB_GLOBAL::SendInt2(short value, int flags)
{
	short nValue = htons(value);
	int rtn = zmq_send(socket_snd, &nValue, sizeof(nValue), flags);
	//zmq_msg_t message;
	//zmq_msg_init_size(&message, str.length());
	//memcpy(zmq_msg_data(&message), str.toLatin1().constData(), str.length());
	//int rtn = zmq_msg_send(&message, socket_snd, flags);
	return rtn;
}

int CRTDBDATASUB_GLOBAL::SendInt4(int value, int flags)
{
	int nValue = htonl(value);
	int rtn = zmq_send(socket_snd, &nValue, sizeof(nValue), flags);
	//zmq_msg_t message;
	//zmq_msg_init_size(&message, str.length());
	//memcpy(zmq_msg_data(&message), str.toLatin1().constData(), str.length());
	//int rtn = zmq_msg_send(&message, socket_snd, flags);
	return rtn;
}

int CRTDBDATASUB_GLOBAL::ReceiveProtocol()
{
	char szUUID[256];
    int size = zmq_recv (socket_rcv, szUUID, 255, 0);
    szUUID [size] = 0;

	//memset(szUUID, 0, sizeof(szUUID));
	//zmq_msg_t message_part;
	//zmq_msg_init(&message_part);
	//zmq_msg_recv(&message_part, socket_rcv, 0);
	//memcpy(szUUID, zmq_msg_data(&message_part), zmq_msg_size(&message_part));
	//zmq_msg_close(&message_part);
	isMyUpdate = true;
	if ( clientUUID != szUUID )
	{
		std::cout << "clientUUID not equal!" << std::endl;
		isMyUpdate = false;
	}
	int hasMore;
	size_t hasMore_size = sizeof (hasMore);
	zmq_getsockopt (socket_rcv, ZMQ_RCVMORE, &hasMore, &hasMore_size);
	return hasMore == 1;
	//return zmq_msg_more (&message_part);
}

int CRTDBDATASUB_GLOBAL::ReceiveRtdbData(QString& strKey, QVariant& value)
{
	char szFam[16];
	char szApp[16];
	char szDB[16];
	char szTable[16];
	char szField[16];
	int nRow;
	int nFieldType;
	int nFieldSize;
	char szValue[1024];
	int size;

	//fam
    size = zmq_recv (socket_rcv, szFam, 15, 0);
    szFam[size] = 0;

	//定义了一个消息结束标志
	if ( strcmp(szFam, "END") == 0 )
		return 0;

	//app
    size = zmq_recv (socket_rcv, szApp, 15, 0);
    szApp[size] = 0;
	//db
    size = zmq_recv (socket_rcv, szDB, 15, 0);
    szDB[size] = 0;
	//table
    size = zmq_recv (socket_rcv, szTable, 15, 0);
    szTable[size] = 0;
	//field
    size = zmq_recv (socket_rcv, szField, 15, 0);
    szField[size] = 0;
	//row
    size = zmq_recv (socket_rcv, &nRow, sizeof(nRow), 0);
	nRow = ntohl(nRow);
	//type
    size = zmq_recv (socket_rcv, &nFieldType, sizeof(nRow), 0);
	nFieldType = ntohl(nFieldType);
	//size
    size = zmq_recv (socket_rcv, &nFieldSize, sizeof(nRow), 0);
	nFieldSize = ntohl(nFieldSize);
	//value
    size = zmq_recv (socket_rcv, szValue, sizeof(szValue) - 1, 0);
    szValue[size] = 0;

	//QVariant rawValue = szValue;
	int nValue = 0;
	switch(nFieldType)
	{
		case GDB_DT_INT:
			if ( nFieldSize == 4 )
			{
				nValue = *reinterpret_cast<int*>(szValue);
				nValue = ntohl(nValue);
				value = nValue;
			}
			if ( nFieldSize == 8 )
			{
				//这个地方其实也需要字节序转换的，暂时没有64位整数需求
				value = *reinterpret_cast<long long*>(szValue);
			}
			break;
		case GDB_DT_FLOAT:
			if ( nFieldSize == 4 )
			{
				value = *reinterpret_cast<float*>(szValue);
			}
			if ( nFieldSize == 8 )
			{
				value = *reinterpret_cast<double*>(szValue);
			}
			break;
		case GDB_DT_DATE:
		case GDB_DT_TIME:
			{
				value = *reinterpret_cast<double*>(szValue);
			}
			break;
		case GDB_DT_CHAR:
			value = szValue;
			break;
	}
	strKey = BuildKey(szFam, szApp, szDB, szTable, szField, nRow);

	//return zmq_msg_more (&message_part);
	int hasMore;
	size_t hasMore_size = sizeof (hasMore);
	zmq_getsockopt (socket_rcv, ZMQ_RCVMORE, &hasMore, &hasMore_size);
	return hasMore == 1;
}

int CRTDBDATASUB_GLOBAL::UpdateRtdbData(QString& strKey, QVariant& value)
{
	if ( !isMyUpdate )
		return RETURNCODE_RTDBROUTER_SUCCESS;

	QMutexLocker locker(&mtDataMap);

	CRTDBDATA* pRtdbData = NULL;
	QMap<QString, CRTDBDATA*>::const_iterator iterator = rtdbDataMap.find(strKey);
	if ( iterator == rtdbDataMap.end() )
	{
		std::cout << rtdbdatasub_strerror(RETURNCODE_RTDBROUTER_NOTSUBSCRIPT) << std::endl;
		return RETURNCODE_RTDBROUTER_NOTSUBSCRIPT;
	}
	
	pRtdbData = iterator.value();

	pRtdbData->SetValue(value);
	
	return RETURNCODE_RTDBROUTER_SUCCESS;
}
