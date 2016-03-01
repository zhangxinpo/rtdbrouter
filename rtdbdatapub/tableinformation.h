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

	////��ȡ�����ֶ�ȫ����¼��ֵ���ڴ��У���������
	//int RefreshField(const char* fiedname);
	//���ֶ�ֵ��ת��Ϊ�ַ�����
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