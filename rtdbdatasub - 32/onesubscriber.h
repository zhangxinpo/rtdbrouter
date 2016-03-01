#ifndef ONESUBSCRIBER_H_INCLUDED
#define ONESUBSCRIBER_H_INCLUDED

#include <QString>
#include <QList>
#include <QMutex>

class CRTDBDATA;

class COneSubscriber
{
public:
	COneSubscriber();
	COneSubscriber(QString strID);
	~COneSubscriber();
	int AddRtdbData(CRTDBDATA* pRtdbData);
	bool RegisterSubscript();
	QString m_strUUID;

	QList<CRTDBDATA*> mRtdbDataList;
	void ResetCountDown(int count );
	int CountDown();
private:
	int m_nCountDown;
	QMutex mtCountDown;
};
#endif