#include "dbinformation.h"
#include "tableinformation.h"
#include "rtdbintf.h"
#include <QLibrary>
#include <stdio.h>
#include "mytrace.h"

typedef int (*FunGetRtdbConnection)(IRTDBConnection**);

CDBInformation::CDBInformation()
{
	MyTrace t1("CDBInformation::CDBInformation");

	m_rtdbLib = NULL;
	m_pRTDBConnection = NULL;
}

int CDBInformation::LoadRtdbModule()
{
		MyTrace t1("CDBInformation::LoadRtdbModule");

        if ( m_rtdbLib != NULL)
         {
                 printf("Reload rtdbintf library!");
                 return -1;
         }
         m_rtdbLib = new QLibrary("rtdbintf");
         if ( NULL == m_rtdbLib)
         {
                 printf("Allocate memory for rtdbintf libray failed!");
                 return -1;
         }
		 if ( !m_rtdbLib->load() )
		 {
			 printf("load rtdbintf library failed!");
			 return -1;
		 }
		 
         FunGetRtdbConnection funGetRtdbConnection;
         funGetRtdbConnection = (FunGetRtdbConnection)m_rtdbLib->resolve("CreateRtdbConnection");
         if ( NULL == funGetRtdbConnection)
         {
                 printf("Resolve function GetRtdbConnection failed!");
                 return -1;
         }
         int status = funGetRtdbConnection(&m_pRTDBConnection);
         if ( RTDB_S_OK != status)
         {
                 printf("Invoke GetRtdbConnection failed!");
                 return -1;
         }
         return 0;
}

int CDBInformation::Connect()
 {
	MyTrace t1("CDBInformation::Connect");

     int status = m_pRTDBConnection->Connect();
     if (RTDB_S_OK != status )
     {
             printf("Connect failed! Status code is %d", status);
             return -1;
     }
     status = m_pRTDBConnection->GetDatabase(&m_pRTDBDatabase);
     if (RTDB_S_OK != status )
     {
             printf("GetDatabase failed! Status code is %d", status);
             return -1;
     }
     return 0;
 }
 
int CDBInformation::OpenDB(QString strFam, QString strApp, QString strDB)
{
	MyTrace t1("CDBInformation::OpenDB");

	LoadRtdbModule();
	Connect();
    int status;
	QString strMiscParams = QString("APP=%1;FAM=%2").arg(strApp).arg(strFam);
	status = m_pRTDBDatabase->Open(strDB.toLatin1().constData(), strMiscParams.toLatin1().constData());
    if (RTDB_S_OK != status )
    {
        printf("Open Database failed! Status code is %d", status);
        return -1;
    }
	return 0;
}

CTableInformation* CDBInformation::FindTable(QString tablename)
{
	//QMutexLocker locker(&mtxMapTables);
	MyTrace t1("CDBInformation::FindTable");

	QMap<QString, CTableInformation*>::const_iterator it = mapTables.find(tablename);
	if ( it == mapTables.constEnd() )
		return NULL;
	
	return it.value();
}

CTableInformation* CDBInformation::OpenTable(QString tablename)
{
	MyTrace t1("CDBInformation::OpenTable");

	int status;
	IRTDBQuery* pQuery = NULL;
	status = m_pRTDBDatabase->GetQuery(tablename.toLatin1().constData(), &pQuery);
    if (RTDB_S_OK != status )
    {
         printf("GetQuery failed! Status code is %d", status);
         return NULL;
    }

	IRTDBTableSchema* pTableSchema = NULL;
    status = m_pRTDBDatabase->GetTableSchema(tablename.toLatin1().constData(), &pTableSchema);
    if (RTDB_S_OK != status )
    {
        printf("GetTableSchema failed! Status code is %d", status);
        return NULL;
    }

	CTableInformation* pTable = new CTableInformation(pQuery, pTableSchema);
	if ( pTable != NULL )
	{
		//QMutexLocker locker(&mtxMapTables);
		mapTables.insert(tablename, pTable);
	}
	return pTable;
}

void CDBInformation::ReleaseFieldBuffers()
{
	//QMutexLocker locker(&mtxMapTables);
	MyTrace t1("CDBInformation::ReleaseFieldBuffers");

	QMap<QString, CTableInformation*>::const_iterator it = mapTables.constBegin();
	while(it != mapTables.constEnd() )
	{
		it.value()->ReleaseFieldBuffers();
		++it;
	}
}