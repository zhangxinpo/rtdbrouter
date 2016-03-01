#ifndef UPDATEFIELDTHREAD_H_INCLUDED
#define UPDATEFIELDTHREAD_H_INCLUDED
#include <QThread>

class CUpdateFieldThread : public QThread
{
public:
	CUpdateFieldThread();
	void run();
	void stop();
private:
	bool m_bStop;
};

#endif