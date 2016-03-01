#include "mytrace.h"
#include <QtDebug>

MyTrace::MyTrace(const char* function)
:m_strFuncName(function)
{
	//qDebug() << "Enter" << m_strFuncName;
}

MyTrace::~MyTrace()
{
	//qDebug() << "Leave" << m_strFuncName;
}