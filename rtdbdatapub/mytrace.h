#ifndef MYTRACE_H_INCLUDED
#define MYTRACE_H_INCLUDED
#include <QString>

class MyTrace
{
public:
	MyTrace(const char* function);
	~MyTrace();
private:
	QString m_strFuncName;
};
#endif