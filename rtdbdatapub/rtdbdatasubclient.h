#ifndef RTDBDATASUBCLIENT_H_INCLUDED
#define RTDBDATASUBCLIENT_H_INCLUDED
#include <QString>
#include <QMap>
#include <QMutex>

class COneSubscriber;
class CRTDBDATA;
class CRtdbDataSubClient
{
public:
	CRtdbDataSubClient(QString strID);
	COneSubscriber* FindSubscriber(QString subscriberID);
	COneSubscriber* CreateSubscriber(QString subscriberID);
	void RemoveSubscriber(QString subscriberID);
	CRTDBDATA* FindRTDBDATA(QString key);
	CRTDBDATA* CreateRTDBDATA(QString strFam, QString strApp, QString strDB, QString strTable, QString strField, int row, QString strKey);
	int CountDown();

	//QMap<QString, CRTDBDATA*> CloneAllClientRtdbData();
	QMap<QString, CRTDBDATA*> mapAllClientRtdbData;
	QString strClientUUID;

private:
	QMutex mtxMap;
	QMap<QString,COneSubscriber*> mapSubscribers; 
};
#endif