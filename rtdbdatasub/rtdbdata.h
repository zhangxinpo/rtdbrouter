#ifndef RTDBDATA_H_INCLUDED
#define RTDBDATA_H_INCLUDED
#include <QVariant>
#include <QString>
#include <QMutex>
#include <QVariant>

class CRTDBDATA
{
public:
	CRTDBDATA(const char* fam, const char* app, const char* db, const char* table, const char* field, int row);
	int AddRef();
	int ReleaseRef();
	int GetRefCount();
	QVariant GetValue();
	void SetValue(QVariant& newValue);
	QString strFam;
	QString strApp;
	QString strDB;
	QString strTable;
	QString strField;
	QString strRow;
	int nRow;

	QString strKey;

	~CRTDBDATA();
	void ResetCounter(int count = 100);
	int CountDown();

private:
	QMutex mutexRef;
	QMutex mutexCounter;
	QVariant value;
	int nRefCount;
	int nCounter;
};

#endif