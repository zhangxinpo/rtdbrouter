#include "publishthread.h"
#include "rtdbdatapub_global.h"

CPublishThread::CPublishThread()
{
	m_bStop = false;
}

void CPublishThread::stop()
{
	m_bStop = true;
}

void CPublishThread::run()
{
	while (!m_bStop) 
	{
		CRTDBDATAPUB_GLOBAL::Instance()->Publish();
		msleep(1000);
	}
}