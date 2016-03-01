#include "rtdbrouterclient.h"
#include "rtdbdatasub_global.h"
#include "zmq.h"

//!��ʼ������
/*!
\param key Ԥ����������������һ̨�����ϲ�ͬ������Ҫ���Ӳ�ͬ��Ϣ���ߵȲ���
\return ���ִ�гɹ�������true  \n
		���ִ��ʧ�ܣ�����false
*/
bool Initialize(const char* /*key*/ )
{
	if ( CRTDBDATASUB_GLOBAL::Instance()->Initilize()  == true)
	{
		CRTDBDATASUB_GLOBAL::Instance()->AddRefCount();
		return true;
	}
	return false;

}
//!����ʼ�����˳�ǰ������
/*!
\return ���ִ�гɹ�������true  \n
		���ִ��ʧ�ܣ�����false
*/
bool Deinitialize()
{
	CRTDBDATASUB_GLOBAL::Instance()->ReleaseRef();
	return true;
}

//!����һ������
/*!
\param refreshpriod �ö���Ҫ���ˢ������
\return ���ִ�гɹ������ظö��ĵľ��  \n
		���ִ��ʧ�ܣ�����0
*/
//int CreateSubscriptionHandle(int /*refreshpriod*/)
//{
//	return CRTDBDATASUB_GLOBAL::Instance()->CreateSubscriptionHandle();
//}

//!���Ӷ�������
/*!
\param handle ���ľ��
\param fam ������������Ӧ��̬
\param app ������������Ӧ��
\param db ������������ʵʱ��
\param table ���������������ݱ�
\param field �������������ֶ�
\parma row �������������У���1��ʼ
\return ���ִ�гɹ�������true  \n
		���ִ��ʧ�ܣ�����false
*/
//bool AddSubRTDBData(int handle, const char* fam, const char* app, const char* db, const char* table, const char* field, int row)
//{
//	return CRTDBDATASUB_GLOBAL::Instance()->AddSubRTDBData( handle, fam, app, db, table, field, row);
//}

//!����һ������
/*!
\param handle �ö��ĵľ��
\return ���ִ�гɹ�������true  \n
		���ִ��ʧ�ܣ�����false
*/
//bool RegisterSubscript(int handle)
//{
//	return CRTDBDATASUB_GLOBAL::Instance()->RegisterSubscript(handle);
//}
//!ȡ��һ������
/*!
\param handle �ö��ĵľ��
\return ���ִ�гɹ�������true  \n
		���ִ��ʧ�ܣ�����false
*/
//bool CancelSubscript(int handle)
//{
//	return CRTDBDATASUB_GLOBAL::Instance()->CancelSubscript(handle);
//}
//!ˢ��һ�����ģ�������
/*!
\param handle �ö��ĵľ��
\return ���ִ�гɹ�������true  \n
		���ִ��ʧ�ܣ�����false
*/
//bool RefreshSubscript(int handle)
//{
//	return CRTDBDATASUB_GLOBAL::Instance()->RefreshSubscript(handle);
//}

//!��ȡʵʱ����
/*!
\param fam ��������Ӧ��̬
\param app ��������Ӧ��
\param db ��������ʵʱ��
\param table �����������ݱ�
\param field ���������ֶ�
\param row ���������У���1��ʼ
\param buffer ������������洢����������
\param buffersize �����������С
\param value ������������洢����������
\return ���ִ�гɹ���������������>=0 ������������  \n
		���ִ��ʧ�ܣ����ظ�������< 0 ������������
*/
int ReadField(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, void* buffer, int buffersize)
{
	return CRTDBDATASUB_GLOBAL::Instance()->ReadField(fam, app, db, table, field, row, buffer, buffersize);
}

int ReadFieldInt2(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, short* value)
{
	return CRTDBDATASUB_GLOBAL::Instance()->ReadFieldInt2(fam, app, db, table, field, row, value);
}

int ReadFieldInt4(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, int* value)
{
	return CRTDBDATASUB_GLOBAL::Instance()->ReadFieldInt4(fam, app, db, table, field, row, value);
}

int ReadFieldInt8(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, long long* value)
{
	return CRTDBDATASUB_GLOBAL::Instance()->ReadFieldInt8(fam, app, db, table, field, row, value);
}

int ReadFieldFloat(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, float* value)
{
	return CRTDBDATASUB_GLOBAL::Instance()->ReadFieldFloat(fam, app, db, table, field, row, value);
}

int ReadFieldDouble(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, double* value)
{
	return CRTDBDATASUB_GLOBAL::Instance()->ReadFieldDouble(fam, app, db, table, field, row, value);
}

int ReadFieldString(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, char* buffer, int buffersize)
{
	return CRTDBDATASUB_GLOBAL::Instance()->ReadFieldString(fam, app, db, table, field, row, buffer, buffersize);
}

int ReadFieldDateTime(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, double* value)
{
	return CRTDBDATASUB_GLOBAL::Instance()->ReadFieldDateTime(fam, app, db, table, field, row, value);
}
//!����ʵʱ����