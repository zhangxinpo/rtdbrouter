#include "updatefieldthread.h"
#include "rtdbdatasub_global.h"


CUpdateFieldThread::CUpdateFieldThread()
{
	m_bStop = false;
}

void CUpdateFieldThread::stop()
{
	m_bStop = true;
}

void CUpdateFieldThread::run()
{
	int nMore = 0;
	while (!m_bStop) 
	{
		nMore = CRTDBDATASUB_GLOBAL::Instance()->ReceiveProtocol();
		while ( nMore == 1)
		{
			QString strKey;
			QVariant value;
			nMore = CRTDBDATASUB_GLOBAL::Instance()->ReceiveRtdbData(strKey, value);
			if ( nMore == 0)//结束标志，非数据
				break;
			CRTDBDATASUB_GLOBAL::Instance()->UpdateRtdbData(strKey, value);
		}
	}
}