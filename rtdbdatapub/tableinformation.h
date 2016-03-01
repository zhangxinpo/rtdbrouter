#ifndef TABLEINFORMATION_H_INCLUDED
#define TABLEINFORMATION_H_INCLUDED

#include <QMap>
#include <QMutex>
#include <QString>

class CFieldInformation;
class IRTDBQuery;
class IRTDBTableSchema;

class CTableInformation
{
public:
	CTableInformation(IRTDBQuery* pQuery, IRTDBTableSchema* pTableShema);
	CFieldInformation* FindField(QString strField);
	CFieldInformation* OpenField(QString strField);
	void ReleaseFieldBuffers();

	////读取整个字段全部记录的值到内存中（批量读）
	//int RefreshField(const char* fiedname);
	//读字段值（转换为字符串）
	int ReadFieldValue(QString fieldname, int row, int* fieldtype, int* fieldsize, char* buffer, int buffersize);
private:
	void ResetRecordCount();

public:
	unsigned int m_nRecordCount;

private:
	IRTDBQuery* m_pTable;
	IRTDBTableSchema* m_pTableSchema;
	QMap<QString, CFieldInformation*> mapFields;
	QMutex mtxMapFields;
};

#endif