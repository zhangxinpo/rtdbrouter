#ifndef RTDBDATASUB_GLOBAL_INCLUDED
#define RTDBDATASUB_GLOBAL_INCLUDED

#include <QString>
#include <QMap>
#include <QVariant>
#include <zmq.h>
#include <QMutex>
#include "updatefieldthread.h"

#define MAX_SUBSCRIPTION_COUNT	100

class COneSubscriber;
class CRTDBDATA;

class CRTDBDATASUB_GLOBAL
{
public:
	static CRTDBDATASUB_GLOBAL* Instance();
private:
	static CRTDBDATASUB_GLOBAL* g_sCRTDBDATASUB_GLOBAL;

public:
	~CRTDBDATASUB_GLOBAL();
	bool Initilize();
	void AddRefCount();
	void ReleaseRef();
	int CreateSubscriptionHandle();
	bool AddSubRTDBData(int handle, const char* fam, const char* app, const char* db, const char* table, const char* field, int row);
	bool RegisterSubscript(int handle);
	bool CancelSubscript(int handle);
	bool RefreshSubscript(int handle);
	QString BuildKey( const char* fam, const char* app, const char* db, const char* table, const char* field, int row);
	int ReadField(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, void* buffer, int buffersize);
	int ReadFieldInt2(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, short* value);
	int ReadFieldInt4(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, int* value);
	int ReadFieldInt8(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, long long* value);
	int ReadFieldFloat(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, float* value);
	int ReadFieldDouble(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, double* value);
	int ReadFieldString(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, char* buffer, int buffersize);
	int ReadFieldDateTime(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, double* value);

	int SendProtocol(const char* protocol);

	int ReceiveProtocol();
	int ReceiveRtdbData(QString& strKey, QVariant& value);
	int UpdateRtdbData(QString& strKey, QVariant& value);

private:
	CRTDBDATASUB_GLOBAL();
	bool LoadConfig();
	int ReadFieldQVariant(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, QVariant* value);
	int SendQString(QString str, int flags = 0);
	int SendInt2(short value, int flags);
	int SendInt4(int value, int flags);

private:
	void *zmq_context;
	void *socket_rcv;
	void *socket_snd;
	CUpdateFieldThread trdUpdateField;
	COneSubscriber* subscribersArray[MAX_SUBSCRIPTION_COUNT];
	QMap<QString,CRTDBDATA*> rtdbDataMap;
	bool hasInitilized;
	int m_nRefCount;
	QString sendAddress;
	QString subcriberAddress;
	QString clientUUID;

	bool isMyUpdate;

	QMutex mtPulisherSocket;
	QMutex mtDataMap;
};

#endif