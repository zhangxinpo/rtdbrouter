#ifndef REFRESHBUFFERDATATHREAD_H_INCLUDED
#define REFRESHBUFFERDATATHREAD_H_INCLUDED
#include <QThread>

class CRefreshBufferDataThread : public QThread
{
public:
	CRefreshBufferDataThread();
	void run();
	void stop();
private:
	bool m_bStop;
};

#endif