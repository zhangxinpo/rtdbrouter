#include "tableinformation.h"
#include "rtdbintf.h"
#include "fieldinformation.h"
#include "mytrace.h"

CTableInformation::CTableInformation(IRTDBQuery* pQuery, IRTDBTableSchema* pTableShema)
{
	MyTrace t1("CTableInformation::CTableInformation");
	m_pTable = pQuery;
	m_pTableSchema = pTableShema;
}

CFieldInformation* CTableInformation::FindField(QString strField)
{
	MyTrace t1("CTableInformation::FindField");

	//QMutexLocker locker(&mtxMapFields);
	QMap<QString, CFieldInformation*>::const_iterator it = mapFields.find(strField);
	if ( it == 	mapFields.constEnd() )
		return NULL;
	return it.value();
}

CFieldInformation* CTableInformation::OpenField(QString strField)
{
	MyTrace t1("CTableInformation::OpenField");
	IRTDBFieldSchema* pFieldSchema = NULL;
	int nStatus = m_pTableSchema->GetFieldSchema(strField.toLatin1().constData(), &pFieldSchema);
	if ( nStatus != RTDB_S_OK)
		return NULL;

	unsigned int nType, nSize;
	pFieldSchema->GetType(&nType);
	pFieldSchema->GetSize(&nSize);
	
	CFieldInformation* pField = new CFieldInformation(m_pTable, strField, nType, nSize);

	//QMutexLocker locker(&mtxMapFields);
	mapFields.insert(strField, pField);
	return pField;
}

void CTableInformation::ReleaseFieldBuffers()
{
	//QMutexLocker locker(&mtxMapFields);
	MyTrace t1("CTableInformation::ReleaseFieldBuffers");
	QMap<QString, CFieldInformation*>::const_iterator it = mapFields.constBegin();
	
	while ( it != 	mapFields.constEnd() )
	{
		it.value()->ReleaseFieldBuffers();
		++it;
	}
	ResetRecordCount();
}

void CTableInformation::ResetRecordCount()
{
	MyTrace t1("CTableInformation::ResetRecordCount");
	m_pTable->Clear();
	m_pTable->Search2("");
	m_pTable->GetRecordCount(&m_nRecordCount);
}


//	//读取整个字段全部记录的值到内存中（批量读）
//int CTableInformation::RefreshField(const char* fiedname)
//{
//}
//
//	//读字段值（转换为字符串）
int CTableInformation::ReadFieldValue(QString fieldname, int row, int* fieldtype, int* fieldsize, char* buffer, int buffersize)
{
	MyTrace t1("CTableInformation::ReadFieldValue");

	//QMutexLocker locker(&mtxMapFields);
	QMap<QString, CFieldInformation*>::const_iterator it = mapFields.find(fieldname);
	if ( it == 	mapFields.constEnd() )
		return -1;
	//todo
	return it.value()->ReadFieldValue(row, fieldtype, fieldsize, buffer, buffersize);
}


