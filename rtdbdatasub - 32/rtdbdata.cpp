#include "rtdbdata.h"
#include <QMutexLocker>
//#include "mytrace.h"

CRTDBDATA::CRTDBDATA(const char* fam, const char* app, const char* db, const char* table, const char* field, int row)
{
	//MyTrace("CRTDBDATA::CRTDBDATA");
	strFam = QString(fam).toUpper();
	strApp = QString(app).toUpper();
	strDB =  QString(db).toUpper();
	strTable =  QString(table).toUpper();
	strField =  QString(field).toUpper();
	nRow = row;
	strRow = QString::number(nRow);
	nRefCount = 0;
}

int CRTDBDATA::AddRef()
{
	QMutexLocker locker(&mutexRef);
	++nRefCount;
	return nRefCount;
}

int CRTDBDATA::ReleaseRef()
{
	QMutexLocker locker(&mutexRef);
	--nRefCount;
	return nRefCount;
}

int CRTDBDATA::GetRefCount()
{
	QMutexLocker locker(&mutexRef);
	return nRefCount;
}

QVariant CRTDBDATA::GetValue()
{
	return value;
}

void CRTDBDATA::SetValue(QVariant& newValue)
{
	value = newValue;
}

CRTDBDATA::~CRTDBDATA()
{
	//MyTrace trace("CRTDBDATA::~CRTDBDATA");
}