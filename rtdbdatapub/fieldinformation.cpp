#include "fieldinformation.h"
#include "rtdbintf.h"
#include "mytrace.h"
#include <iostream>

CFieldInformation::CFieldInformation(IRTDBQuery* pQuery, QString fieldName, int nFieldType, int nFieldSize)
{
	MyTrace t1("CFieldInformation::CFieldInformation");
	m_strFieldName = fieldName;
	m_nFieldType = nFieldType;
	m_nFieldSize = nFieldSize;
	m_pQuery = pQuery;
	m_szBuffer = NULL;
}

void CFieldInformation::ReleaseFieldBuffers()
{
	MyTrace t1("CFieldInformation::ReleaseFieldBuffers");
	//QMutexLocker locker(&mtxBuffer);
	if ( m_szBuffer != NULL )
		delete m_szBuffer;

	m_szBuffer = NULL;
}

bool CFieldInformation::HasFieldBuffer()
{
	MyTrace t1("CFieldInformation::HasFieldBuffer");
	//QMutexLocker locker(&mtxBuffer);
	return m_szBuffer != NULL;
}

bool CFieldInformation::AllocateFieldBuffer(int nRecordCount)
{
	MyTrace t1("CFieldInformation::AllocateFieldBuffer");
	//QMutexLocker locker(&mtxBuffer);
	m_szBuffer = new char[nRecordCount * m_nFieldSize];
	return m_szBuffer != NULL;
}

bool CFieldInformation::RefreshFieldBuffer(int nRecordCount)
{
	MyTrace t1("CFieldInformation::RefreshFieldBuffer");
	//QMutexLocker locker(&mtxBuffer);
	int	status = m_pQuery->ReadField(m_strFieldName.toLatin1().constData(),m_szBuffer, nRecordCount * m_nFieldSize, nRecordCount);
	return status == RTDB_S_OK;
}

int CFieldInformation::ReadFieldValue(int row, int* fieldtype, int* fieldsize, char* buffer, int /*buffersize*/)
{
	MyTrace t1("CFieldInformation::ReadFieldValue");
	//QMutexLocker locker(&mtxBuffer);
	memcpy(buffer, m_szBuffer + (row - 1) * m_nFieldSize, m_nFieldSize);
	*fieldtype = m_nFieldType;
	*fieldsize = m_nFieldSize;
	return 0;
}
