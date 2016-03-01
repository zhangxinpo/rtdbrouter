#ifndef RTDBDATAPUB_GLOBAL_H_INCLUDED
#define RTDBDATAPUB_GLOBAL_H_INCLUDED

#include <QString>
#include <QMap>
#include <QMutex>
#include "dbinformation.h"
#include "iprocclient.h"

class CRtdbDataSubClient;

typedef const char* (*FunGetPrimaryComputerNameByApp)(const char*, const char*);
typedef bool (*FunReportAvailable)(bool);

class CRTDBDATAPUB_GLOBAL
{
public:
	static CRTDBDATAPUB_GLOBAL* Instance();
private:
	static CRTDBDATAPUB_GLOBAL* g_sCRTDBDATAPUB_GLOBAL;
	static void RoleChangeCB(int role);
	static void ParamChangeCB(const char* key, int value);
	static int gRoleCurrent;
	static QMutex gMtxCurRole;

public:
	~CRTDBDATAPUB_GLOBAL();
	bool Initilize();
	void AddRefCount();
	void ReleaseRef();

	int OpenRtdb();
	int ReleaseFieldBuffers();
	int Publish();
	int ProcessRequest();
	bool RegisterToAppManager();

private:
	CRTDBDATAPUB_GLOBAL();
	bool LoadConfig();
	bool LoadNodestatus();
	int ProcessRegisterRequest();
	int ProcessCancelRegisterRequest();
	int ProcessFreshRequest();
	int ReceiveProtocol(char* buffer, int buffersize);
	int ReceiveMsg(char* buffer, int buffersize);
	int SendQString(QString str, int flags = 0);
	int SendInt4(int value, int flags);

private:
	void *zmq_context;
	void *socket_rcv;
	void *socket_snd;
	FunGetPrimaryComputerNameByApp pGetPrimaryComputerNameByApp;
	FunReportAvailable m_pReportAvailable;

	bool hasInitilized;
	int m_nRefCount;
	QString sendAddress;
	QString subcriberAddress;
	QMap<QString, CRtdbDataSubClient*> mapClients;
	CDBInformation myDBInformation;
	QMutex mtxPushOrPublish;

public:
	QString myFam;
	QString myApp;
	QString myDB;
	QString myLocalHostName;
	bool m_bPrimary;
};


#endif