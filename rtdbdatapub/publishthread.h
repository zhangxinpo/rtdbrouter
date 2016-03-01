#ifndef PUBLISHTHREAD_H_INCLUDED
#define PUBLISHTHREAD_H_INCLUDED
#include <QThread>

class CPublishThread : public QThread
{
public:
	CPublishThread();
	void run();
	void stop();
private:
	bool m_bStop;
};

#endif