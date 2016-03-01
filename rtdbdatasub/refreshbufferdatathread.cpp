#include "refreshbufferdatathread.h"
#include "rtdbdatasub_global.h"


CRefreshBufferDataThread::CRefreshBufferDataThread()
{
	m_bStop = false;
}

void CRefreshBufferDataThread::stop()
{
	m_bStop = true;
}

void CRefreshBufferDataThread::run()
{
	while (!m_bStop) 
	{
		CRTDBDATASUB_GLOBAL::Instance()->RefreshBufferedData();
		msleep(500);
	}
}