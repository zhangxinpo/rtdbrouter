#include "onesubscriber.h"
#include <QUuid>
#include "rtdbdatasub_global.h"
#include "../rtdbdatasub/rtdbdata.h"
#include <zmq.h>

COneSubscriber::COneSubscriber()
{
	m_strUUID = QUuid::createUuid().toString();
	m_nCountDown = 100;
}

COneSubscriber::COneSubscriber(QString strID)
{
	m_strUUID = strID;
	m_nCountDown = 100;
}

COneSubscriber::~COneSubscriber()
{
	QList<CRTDBDATA*> mRtdbDataList;
	for ( int i = 0; i < mRtdbDataList.size(); ++i )
		mRtdbDataList[i]->ReleaseRef();

	mRtdbDataList.clear();
}

int COneSubscriber::AddRtdbData(CRTDBDATA* pRtdbData)
{
	pRtdbData->AddRef();
	mRtdbDataList.append(pRtdbData);
	return 0;
}

void COneSubscriber::ResetCountDown(int count )
{
	QMutexLocker locker(&mtCountDown);
	m_nCountDown = count;
}

int COneSubscriber::CountDown()
{
	QMutexLocker locker(&mtCountDown);
	return --m_nCountDown;
}
