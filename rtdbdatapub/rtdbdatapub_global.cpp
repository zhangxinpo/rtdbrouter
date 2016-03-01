#include "rtdbdatapub_global.h"
#include <QUuid>
#include <QLibrary>
#include <QHostInfo>
#include <zmq.h>
#include "tableinformation.h"
#include "rtdbdatasubclient.h"
#include "../rtdbdatasub/rtdbdata.h"
#include "fieldinformation.h"
#include "../rtdbdatasub/onesubscriber.h"
#include "../rtdbdatasub/rtdbrouter_protocol.h"
#include <iostream>
#include "mytrace.h"
#include "rtdbintf.h"
#include "icssettings.h"
#include "inodestatus.h"
#include "iprocclient.h"

#ifdef Q_OS_LINUX
#include <arpa/inet.h>
#endif

typedef bool (*FunRegAppProc)(const char*, const char*, const char*, CONFIG_CALLBACK_STATCHANGE, PARAM_CHANGE_CB);

CRTDBDATAPUB_GLOBAL* CRTDBDATAPUB_GLOBAL::g_sCRTDBDATAPUB_GLOBAL = NULL;
int CRTDBDATAPUB_GLOBAL::gRoleCurrent = CFGSRV_ROLE_UNKNOWN;
QMutex CRTDBDATAPUB_GLOBAL::gMtxCurRole;

CRTDBDATAPUB_GLOBAL* CRTDBDATAPUB_GLOBAL::Instance()
{
	if ( g_sCRTDBDATAPUB_GLOBAL == NULL )
	{
		g_sCRTDBDATAPUB_GLOBAL = new CRTDBDATAPUB_GLOBAL();
	}

	return g_sCRTDBDATAPUB_GLOBAL;
}

CRTDBDATAPUB_GLOBAL::~CRTDBDATAPUB_GLOBAL()
{
	zmq_close (socket_snd);
	zmq_close (socket_rcv);
    zmq_ctx_destroy (zmq_context);
}

void CRTDBDATAPUB_GLOBAL::RoleChangeCB(int role)
{
	QMutexLocker locker(&gMtxCurRole);
	gRoleCurrent = role;
	switch(gRoleCurrent)
	{
		case CFGSRV_ROLE_UNKNOWN:
			std::cout << "switch role to CFGSRV_ROLE_UNKNOWN" << std::endl;
			break;
		case CFGSRV_ROLE_ENABLED:
			std::cout << "switch role to CFGSRV_ROLE_ENABLED" << std::endl;
			break;
		case CFGSRV_ROLE_STANDBY:
			std::cout << "switch role to CFGSRV_ROLE_STANDBY" << std::endl;
			break;
		case CFGSRV_ROLE_DISABLED:
			std::cout << "switch role to CFGSRV_ROLE_DISABLED" << std::endl;
			break;
	}
}

void CRTDBDATAPUB_GLOBAL::ParamChangeCB(const char* /*key*/, int /*value*/)
{
	;
}

CRTDBDATAPUB_GLOBAL::CRTDBDATAPUB_GLOBAL()
{
	zmq_context = NULL;
	socket_rcv = NULL;
	socket_snd = NULL;
	pGetPrimaryComputerNameByApp = NULL;
	m_pReportAvailable = NULL;
	hasInitilized = false;
	m_nRefCount = 0;
	m_bPrimary = true;
	myLocalHostName = QHostInfo::localHostName().toUpper(); 
}

void CRTDBDATAPUB_GLOBAL::AddRefCount()
{
	++m_nRefCount;
}

void CRTDBDATAPUB_GLOBAL::ReleaseRef()
{
	--m_nRefCount;
	if ( m_nRefCount == 0)
		delete this;
}

bool CRTDBDATAPUB_GLOBAL::LoadNodestatus()
{
 	QLibrary nodestatusLib("nodestatus");
	if ( nodestatusLib.load() == false )
	{
		printf("load nodestatus failed\n");
		return false;
	}

	pGetPrimaryComputerNameByApp = (FunGetPrimaryComputerNameByApp) nodestatusLib.resolve("GetPrimaryComputerNameByApp");

	if(!pGetPrimaryComputerNameByApp)
	{
		printf("Resolve function \"GetPrimaryComputerNameByApp\" failed!\n");
		return false;
	}

	return true;
}

bool CRTDBDATAPUB_GLOBAL::LoadConfig()
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
	int nSendPort = pSettings->ItemReadInt("sendportforserver");
	int nSubscriberPort = pSettings->ItemReadInt("subscriberportforserver");

	pSettings->Release();

	sendAddress = QString("tcp://").append(szBuffer).append(":") + QString::number(nSendPort);
	subcriberAddress = QString("tcp://").append(szBuffer).append(":") + QString::number(nSubscriberPort);
	return true;
}

bool CRTDBDATAPUB_GLOBAL::Initilize()
{
	MyTrace t1("CRTDBDATAPUB_GLOBAL::Initilize");
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
    zmq_setsockopt (socket_rcv, ZMQ_SUBSCRIBE, "", 0);

	if ( socket_snd == NULL )
		socket_snd = zmq_socket(zmq_context, ZMQ_PUSH);
	if ( socket_snd == NULL )
		return false;
	zmq_connect (socket_snd, sendAddress.toLatin1().constData());

	hasInitilized = true;
	return true;
}

int CRTDBDATAPUB_GLOBAL::ReceiveProtocol(char* buffer, int buffersize)
{
	MyTrace t1("CRTDBDATAPUB_GLOBAL::ReceiveProtocol");
    int size = zmq_recv (socket_rcv, buffer, buffersize, 0);
    buffer[size] = 0;

	int hasMore;
	size_t hasMore_size = sizeof (hasMore);
	zmq_getsockopt (socket_rcv, ZMQ_RCVMORE, &hasMore, &hasMore_size);
	return hasMore == 1;

	//zmq_msg_t message_part;
	//zmq_msg_init(&message_part);
	//zmq_msg_recv(&message_part, socket_rcv, 0);
	//size_t nMsgSize = zmq_msg_size(&message_part);
	//memcpy(buffer, zmq_msg_data(&message_part), nMsgSize > buffersize ? buffersize - 1: nMsgSize);
	//zmq_msg_close(&message_part);
	//return zmq_msg_more (&message_part);
}

int CRTDBDATAPUB_GLOBAL::ReceiveMsg(char* buffer, int buffersize)
{
	MyTrace t1("CRTDBDATAPUB_GLOBAL::ReceiveMsg");
    int size = zmq_recv (socket_rcv, buffer, buffersize, 0);
    buffer[size] = 0;

	int hasMore;
	size_t hasMore_size = sizeof (hasMore);
	zmq_getsockopt (socket_rcv, ZMQ_RCVMORE, &hasMore, &hasMore_size);
	return hasMore == 1;

	//zmq_msg_t message_part;
	//zmq_msg_init(&message_part);
	//zmq_msg_recv(&message_part, socket_rcv, 0);
	//size_t nMsgSize = zmq_msg_size(&message_part);
	//memset(buffer, 0, buffersize);
	//memcpy(buffer, zmq_msg_data(&message_part), nMsgSize > buffersize ? buffersize - 1: nMsgSize);
	//zmq_msg_close(&message_part);
	//return zmq_msg_more (&message_part);
}

int CRTDBDATAPUB_GLOBAL::SendQString(QString str, int flags)
{
	MyTrace t1("CRTDBDATAPUB_GLOBAL::SendQString");
	int rtn = zmq_send(socket_snd,str.toLatin1().constData(), str.length(), flags);
	return rtn;

	//zmq_msg_t message;
	//zmq_msg_init_size(&message, str.length());
	//memcpy(zmq_msg_data(&message), str.toLatin1().constData(), str.length());
	//int rtn = zmq_msg_send(&message, socket_snd, flags);
	//return rtn;
}

int CRTDBDATAPUB_GLOBAL::ProcessRegisterRequest()
{
	MyTrace t1("CRTDBDATAPUB_GLOBAL::ProcessRegisterRequest");

	//bool bHasRegistered = false;
	char szBuffer[1024];
	int nRow;
	int size;
	QString strClientUUID, strSubscriberUUID, strFam, strApp, strDB, strTable, strField, strRow;
	CRtdbDataSubClient* pClient = NULL;
	CTableInformation* pTable = NULL;
	CFieldInformation* pField = NULL;

	//clientuuid
    size = zmq_recv (socket_rcv, szBuffer, sizeof(szBuffer), 0);
    szBuffer[size] = 0;
	//ReceiveMsg(szBuffer, sizeof(szBuffer));
	strClientUUID = szBuffer;
	
	QMap<QString, CRtdbDataSubClient*>::const_iterator itClient = mapClients.find(strClientUUID);
	if (itClient == mapClients.end() )
	{
		pClient = new CRtdbDataSubClient(strClientUUID);
		mapClients.insert(strClientUUID, pClient);
	}
	else
	{
		pClient = itClient.value();
	}

	//subscriberid
	//ReceiveMsg(szBuffer, sizeof(szBuffer));
 //   size = zmq_recv (socket_rcv, szBuffer, sizeof(szBuffer), 0);
 //   szBuffer[size] = 0;
	//strSubscriberUUID = szBuffer;
	//COneSubscriber* pSubscriber = pClient->FindSubscriber(strSubscriberUUID);
	//if ( pSubscriber != NULL)
	//{
	//	bHasRegistered = true;
	//	//std::cerr << "should not be hear! CRTDBDATAPUB_GLOBAL::ProcessRegisterRequest, pSubscriber != NULL" << std::endl;
	//	//return -1;
	//}
	//else
	//{
	//	pSubscriber = pClient->CreateSubscriber(strSubscriberUUID);
	//}
	

	int nMore = 1;
	size_t nMore_size = sizeof(nMore);
	while(nMore == 1)
	{
		//ReceiveMsg(szBuffer, sizeof(szBuffer));
		size = zmq_recv (socket_rcv, szBuffer, sizeof(szBuffer), 0);
		szBuffer[size] = 0;
		strFam = szBuffer;
		if ( strFam == "END")
			break;
		//ReceiveMsg(szBuffer, sizeof(szBuffer));
		size = zmq_recv (socket_rcv, szBuffer, sizeof(szBuffer), 0);
		szBuffer[size] = 0;
		strApp = szBuffer;
		//ReceiveMsg(szBuffer, sizeof(szBuffer));
		size = zmq_recv (socket_rcv, szBuffer, sizeof(szBuffer), 0);
		szBuffer[size] = 0;
		strDB = szBuffer;
		//ReceiveMsg(szBuffer, sizeof(szBuffer));
		size = zmq_recv (socket_rcv, szBuffer, sizeof(szBuffer), 0);
		szBuffer[size] = 0;
		strTable = szBuffer;
		//ReceiveMsg(szBuffer, sizeof(szBuffer));
		size = zmq_recv (socket_rcv, szBuffer, sizeof(szBuffer), 0);
		szBuffer[size] = 0;
		strField = szBuffer;
		//nMore = ReceiveMsg(szBuffer, sizeof(szBuffer));
		//strRow = szBuffer;
		zmq_recv (socket_rcv, &nRow, sizeof(nRow), 0);
		nRow = ntohl(nRow);
		zmq_getsockopt (socket_rcv, ZMQ_RCVMORE, &nMore, &nMore_size);

		if ( strFam != myFam || strApp != myApp || strDB != myDB )
			continue;
		//如果没有就创建一个
		pTable = myDBInformation.FindTable(strTable);
		if ( pTable == NULL )
			pTable = myDBInformation.OpenTable(strTable);
		pField = pTable->FindField(strField);
		if ( pField == NULL )
			pField = pTable->OpenField(strField);
		QString strKey = strTable + "_" + strField + "_" + QString::number(nRow);
		CRTDBDATA* pRtdbData = pClient->FindRTDBDATA(strKey);
		if ( pRtdbData == NULL )
		{
			pRtdbData = pClient->CreateRTDBDATA(strFam, strApp, strDB, strTable, strField, nRow, strKey);
		}
		pRtdbData->ResetCounter();

		//if ( !bHasRegistered )
		//{
		//	pRtdbData->AddRef();
		//	pSubscriber->AddRtdbData(pRtdbData);
		//}
		//else
		//{
		//	pSubscriber->ResetCountDown(100);
		//}

	}
	
	return 0;
}

int CRTDBDATAPUB_GLOBAL::ProcessCancelRegisterRequest()
{
	MyTrace t1("CRTDBDATAPUB_GLOBAL::ProcessCancelRegisterRequest");
	char szBuffer[1024];
	QString strClientUUID, strSubscriberUUID;
	CRtdbDataSubClient* pClient = NULL;

	//clientuuid
	ReceiveMsg(szBuffer, sizeof(szBuffer));
	strClientUUID = szBuffer;
	
	QMap<QString, CRtdbDataSubClient*>::const_iterator itClient = mapClients.find(strClientUUID);
	if (itClient == mapClients.end() )
	{
		std::cerr << "should not be hear!CRTDBDATAPUB_GLOBAL::ProcessFreshRequest,itClient == mapClients.end()" << std::endl;
		return -1;
	}
	else
	{
		pClient = itClient.value();
	}

	//subscriberid
	ReceiveMsg(szBuffer, sizeof(szBuffer));
	strSubscriberUUID = szBuffer;
	pClient->RemoveSubscriber(strSubscriberUUID);
	return 0;
}

int CRTDBDATAPUB_GLOBAL::ProcessFreshRequest()
{
	MyTrace t1("CRTDBDATAPUB_GLOBAL::ProcessFreshRequest");
	char szBuffer[512];
	QString strClientUUID, strSubscriberUUID;
	CRtdbDataSubClient* pClient = NULL;

	//clientuuid
	ReceiveMsg(szBuffer, sizeof(szBuffer));
	strClientUUID = szBuffer;
	
	QMap<QString, CRtdbDataSubClient*>::const_iterator itClient = mapClients.find(strClientUUID);
	if (itClient == mapClients.end() )
	{
		std::cerr << "should not be hear!CRTDBDATAPUB_GLOBAL::ProcessFreshRequest,itClient == mapClients.end()" << std::endl;
		return -1;
	}
	else
	{
		pClient = itClient.value();
	}

	//subscriberid
	ReceiveMsg(szBuffer, sizeof(szBuffer));
	strSubscriberUUID = szBuffer;
	COneSubscriber* pSubscriber = pClient->FindSubscriber(strSubscriberUUID);
	if ( pSubscriber == NULL)
	{
		std::cerr << "should not be hear!CRTDBDATAPUB_GLOBAL::ProcessFreshRequest,pSubscriber == NULL" << std::endl;
		return -2;
	}
	pSubscriber->ResetCountDown(100);
	return 0;
}

int CRTDBDATAPUB_GLOBAL::ProcessRequest()
{
	MyTrace t1("CRTDBDATAPUB_GLOBAL::ProcessRequest");
	char szBuffer[256];
	memset(szBuffer, 0, sizeof(szBuffer));
	ReceiveProtocol(szBuffer, sizeof(szBuffer));

	QMutexLocker locker(&mtxPushOrPublish);
	if ( strcmp(szBuffer, RTDBROUTER_PROTOCOL_REGISTERSUBSCRIPT) == 0 )
	{
		ProcessRegisterRequest();
	}
	if ( strcmp(szBuffer, RTDBROUTER_PROTOCOL_CANCELSUBSCRIPT) == 0 )
	{
		ProcessCancelRegisterRequest();
	}
	if ( strcmp(szBuffer, RTDBROUTER_PROTOCOL_REFRESHSUBSCRIPT) == 0 )
	{
		ProcessFreshRequest();
	}
	return 0;
}

int CRTDBDATAPUB_GLOBAL::ReleaseFieldBuffers()
{
	MyTrace t1("CRTDBDATAPUB_GLOBAL::ReleaseFieldBuffers");

	myDBInformation.ReleaseFieldBuffers();

	return 0;
}

int CRTDBDATAPUB_GLOBAL::OpenRtdb()
{
	MyTrace t1("CRTDBDATAPUB_GLOBAL::OpenRtdb");
	return myDBInformation.OpenDB(myFam, myApp, myDB);
}

bool CRTDBDATAPUB_GLOBAL::RegisterToAppManager()
{
	MyTrace t1("CRTDBDATAPUB_GLOBAL::RegisterToAppManager");
	QLibrary procclientLib("procclient");
	if ( procclientLib.load() == false )
	{
		printf("load procclient failed\n");
		return false;
	}

	m_pReportAvailable = (FunReportAvailable)procclientLib.resolve("ReportAvailable");
	if(!m_pReportAvailable)
	{
		printf("Resolve function \"ReportAvailable\" failed!\n");
		return false;
	}

	FunRegAppProc pRegAppProc = (FunRegAppProc) procclientLib.resolve("RegAppProc");

	if(!pRegAppProc)
	{
		printf("Resolve function \"RegAppProc\" failed!\n");
		return false;
	}

	return pRegAppProc(myFam.toAscii().constData(), myApp.toAscii().constData(), "rtdbdatapub", RoleChangeCB, ParamChangeCB);
}

int CRTDBDATAPUB_GLOBAL::SendInt4(int value, int flags)
{
	int nValue = htonl(value);
	int rtn = zmq_send(socket_snd, &nValue, sizeof(nValue), flags);
	//zmq_msg_t message;
	//zmq_msg_init_size(&message, str.length());
	//memcpy(zmq_msg_data(&message), str.toLatin1().constData(), str.length());
	//int rtn = zmq_msg_send(&message, socket_snd, flags);
	return rtn;
}

int CRTDBDATAPUB_GLOBAL::Publish()
{
	MyTrace t1("CRTDBDATAPUB_GLOBAL::Publish");
	QMutexLocker locker(&mtxPushOrPublish);

	if ( m_pReportAvailable != NULL )
	{
		m_pReportAvailable(true);
	}

	{
		QMutexLocker locker(&gMtxCurRole);
		if ( gRoleCurrent != CFGSRV_ROLE_ENABLED )
			return 0;
	}

	ReleaseFieldBuffers();
	QString strValue;

	QMap<QString, CRtdbDataSubClient*>::iterator itClients = mapClients.begin();
	while (itClients != mapClients.end()) 
	{
		CRtdbDataSubClient* pClient = itClients.value();

		int nDataCount =  pClient->mapAllClientRtdbData.size();
		if ( nDataCount == 0)
		{
			++itClients;
			continue;
		}
		SendQString(pClient->strClientUUID, ZMQ_SNDMORE);
		QMap<QString, CRTDBDATA*>::iterator itRtdbData = pClient->mapAllClientRtdbData.begin();
		while(itRtdbData != pClient->mapAllClientRtdbData.end() )
		{
			CRTDBDATA* pRtdbData = itRtdbData.value();
			if ( pRtdbData->CountDown() == 0)
			{
				itRtdbData = pClient->mapAllClientRtdbData.erase(itRtdbData);
				delete pRtdbData;
				continue;
			}

			CTableInformation* pTable = myDBInformation.FindTable(pRtdbData->strTable);
			int nFieldType, nFieldSize;
			char szBuffer[1024];
			int nValue = 0;
			CFieldInformation* pField = pTable->FindField(pRtdbData->strField);
			if ( pField == NULL )
			{
				//should not be hear
				++itRtdbData;
				continue;
			}
			if ( pField->HasFieldBuffer() == false )
			{
				pField->AllocateFieldBuffer(pTable->m_nRecordCount);
				pField->RefreshFieldBuffer(pTable->m_nRecordCount);
			}
			pTable->ReadFieldValue(pRtdbData->strField, pRtdbData->nRow,&nFieldType, &nFieldSize, szBuffer, sizeof(szBuffer));
			SendQString(myFam, ZMQ_SNDMORE);
			SendQString(myApp, ZMQ_SNDMORE);
			SendQString(myDB, ZMQ_SNDMORE);
			SendQString(pRtdbData->strTable, ZMQ_SNDMORE);
			SendQString(pRtdbData->strField, ZMQ_SNDMORE);
			SendInt4(pRtdbData->nRow, ZMQ_SNDMORE);
			SendInt4(nFieldType, ZMQ_SNDMORE);
			SendInt4(nFieldSize, ZMQ_SNDMORE);
			if (nFieldType == GDB_DT_INT && nFieldSize == 4)
			{
				nValue = *reinterpret_cast<int*>(szBuffer);
				SendInt4(nValue, ZMQ_SNDMORE);
			}
			else
			{
				zmq_send(socket_snd, szBuffer, nFieldSize, ZMQ_SNDMORE);
			}
			itRtdbData++;

		}

		SendQString("END", 0);

		++itClients;
	}

	return 0;
}
