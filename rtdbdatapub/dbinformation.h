#ifndef DBINFORMATION_H_INCLUDED
#define DBINFORMATION_H_INCLUDED

#include <QMap>
#include <QMutex>

class QLibrary;
class IRTDBConnection;
class IRTDBDatabase;
class IRTDBQuery;
class IRTDBTableSchema;
class IRTDBFieldSchema;
class CTableInformation;
class CDBInformation
{
public:
	CDBInformation();
	int OpenDB(QString strFam, QString strApp, QString strDB);
	CTableInformation* FindTable(QString tablename);
	CTableInformation* OpenTable(QString tablename);
	void ReleaseFieldBuffers();

private:
	int LoadRtdbModule();
	int Connect();

private:
	QLibrary* m_rtdbLib;
	IRTDBConnection* m_pRTDBConnection;
	IRTDBDatabase* m_pRTDBDatabase;

private:
	QMap<QString, CTableInformation*> mapTables;
	QMutex mtxMapTables;
};

#endif