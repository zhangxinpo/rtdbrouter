#include "rtdbdatasubclient.h"
#include "../rtdbdatasub/rtdbdata.h"
#include "../rtdbdatasub/onesubscriber.h"
CRtdbDataSubClient::CRtdbDataSubClient(QString strID)
{
	strClientUUID = strID;
}

COneSubscriber* CRtdbDataSubClient::FindSubscriber(QString subscriberID)
{
	QMap<QString,COneSubscriber*>::const_iterator it = mapSubscribers.find(subscriberID);
	if ( it == mapSubscribers.end() )
		return NULL;

	return it.value();
}

COneSubscriber* CRtdbDataSubClient::CreateSubscriber(QString subscriberID)
{
	COneSubscriber* pSubscriber = new COneSubscriber(subscriberID);
	mapSubscribers.insert(subscriberID, pSubscriber);
	return pSubscriber;
}

void CRtdbDataSubClient::RemoveSubscriber(QString subscriberID)
{
	QMap<QString,COneSubscriber*>::iterator it = mapSubscribers.find(subscriberID);
	if ( it == mapSubscribers.end() )
		return;

	COneSubscriber* pSubscriber = it.value();
	delete pSubscriber;
	mapSubscribers.erase(it);
}

CRTDBDATA* CRtdbDataSubClient::FindRTDBDATA(QString key)
{
	QMap<QString,CRTDBDATA*>::const_iterator it = mapAllClientRtdbData.find(key);
	if ( it == mapAllClientRtdbData.end() )
		return NULL;

	return it.value();
}

CRTDBDATA* CRtdbDataSubClient::CreateRTDBDATA(QString strFam, QString strApp, QString strDB, QString strTable, QString strField, int row, QString strKey)
{
	CRTDBDATA* pRtdbData = new  CRTDBDATA(0, 0, 0, strTable.toLatin1().constData(), strField.toLatin1().constData(), row);
	pRtdbData->strKey = strKey;
	mapAllClientRtdbData.insert(strKey, pRtdbData);
	return pRtdbData;
}

//QMap<QString, CRTDBDATA*> CRtdbDataSubClient::CloneAllClientRtdbData()
//{
//	if (mapAllClientRtdbData.size() == 0)
//		return 0;
//}

int CRtdbDataSubClient::CountDown()
{
	QMap<QString,COneSubscriber*>::iterator it = mapSubscribers.begin();
	while(it != mapSubscribers.end())
	{
		COneSubscriber* pSubscriber = it.value();
		int nCountDown = pSubscriber->CountDown();
		if ( nCountDown == 0)
		{
			it = mapSubscribers.erase(it);
			delete pSubscriber;
		}
		++it;
	}

	return mapSubscribers.size();
}
