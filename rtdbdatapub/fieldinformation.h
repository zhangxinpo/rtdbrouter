#ifndef FIELDINFORMATION_H_INCLUDED
#define FIELDINFORMATION_H_INCLUDED
#include <QMutex>
#include <QString>

class IRTDBQuery;
class CFieldInformation
{
public:
	CFieldInformation(IRTDBQuery* pQuery, QString fieldName, int nFieldType, int nFieldSize);
	void ReleaseFieldBuffers();
	bool HasFieldBuffer();
	bool AllocateFieldBuffer(int nRecordCount);
	bool RefreshFieldBuffer(int nRecordCount);
	int ReadFieldValue(int row, int* fieldtype, int* fieldsize, char* buffer, int buffersize);
private:
	IRTDBQuery* m_pQuery;
	QString m_strFieldName;
	int m_nFieldType;
	int m_nFieldSize;
	char* m_szBuffer;
	QMutex mtxBuffer;
};
#endif