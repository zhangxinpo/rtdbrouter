#ifndef RTDBROUTERCLIENT_H_INCLUDED
#define RTDBROUTERCLIENT_H_INCLUDED 1

#ifdef _MSC_VER
	#ifdef RTDBROUTERCLIENT_BUILD_DLL
		#define RTDBROUTERCLIENT_EXPORT __declspec(dllexport)
	#else
		#define RTDBROUTERCLIENT_EXPORT __declspec(dllimport)
	#endif
#else
	#define RTDBROUTERCLIENT_EXPORT
#endif

/*
Ϊ���Ż��˻�����Զ�̷���ʵʱ��Զ��Ч�ʣ���Ƹýӿ�
����˼·��һ�ζ���һ��ҳ����������ݣ��ɷ�������ڷ������ĵ�����
1������������ʼ��Initialize
2����һ��ҳ�棬����һ������CreateSubscriptionHandle
3������ҳ�棬����ʵʱ���ݾ����Ӷ�������AddSubRTDBData
4���������ҳ�棬��������RegisterSubscript
5����ʱˢ��ҳ��RefreshSubscript
6����ȡʵʱ����ReadField
7���ر�ҳ�棬ȡ������CancelSubscript
8���������̣�����ʼ��Deinitialize
*/

extern "C" {
//!��ʼ������
/*!
\param key Ԥ����������������һ̨�����ϲ�ͬ������Ҫ���Ӳ�ͬ��Ϣ���ߵȲ���
\return ���ִ�гɹ�������true  \n
		���ִ��ʧ�ܣ�����false
*/
bool RTDBROUTERCLIENT_EXPORT Initialize(const char* key = 0);
//!����ʼ�����˳�ǰ������
/*!
\return ���ִ�гɹ�������true  \n
		���ִ��ʧ�ܣ�����false
*/
bool RTDBROUTERCLIENT_EXPORT Deinitialize();

//!����һ������
/*!
\param refreshpriod �ö���Ҫ���ˢ������
\return ���ִ�гɹ������ظö��ĵľ��  \n
		���ִ��ʧ�ܣ�����0
*/
//int RTDBROUTERCLIENT_EXPORT CreateSubscriptionHandle(int refreshpriod=1);

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
//bool RTDBROUTERCLIENT_EXPORT AddSubRTDBData(int handle, const char* fam, const char* app, const char* db, const char* table, const char* field, int row);

//!����һ������
/*!
\param handle �ö��ĵľ��
\return ���ִ�гɹ�������true  \n
		���ִ��ʧ�ܣ�����false
*/
//bool RTDBROUTERCLIENT_EXPORT RegisterSubscript(int handle);
//!ȡ��һ������
/*!
\param handle �ö��ĵľ��
\return ���ִ�гɹ�������true  \n
		���ִ��ʧ�ܣ�����false
*/
//bool RTDBROUTERCLIENT_EXPORT CancelSubscript(int handle);
//!ˢ��һ�����ģ�������
/*!
\param handle �ö��ĵľ��
\return ���ִ�гɹ�������true  \n
		���ִ��ʧ�ܣ�����false
*/
//bool RTDBROUTERCLIENT_EXPORT RefreshSubscript(int handle);

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
int RTDBROUTERCLIENT_EXPORT ReadField(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, void* buffer, int buffersize);
int RTDBROUTERCLIENT_EXPORT ReadFieldInt2(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, short* value);
int RTDBROUTERCLIENT_EXPORT ReadFieldInt4(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, int* value);
int RTDBROUTERCLIENT_EXPORT ReadFieldInt8(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, long long* value);
int RTDBROUTERCLIENT_EXPORT ReadFieldFloat(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, float* value);
int RTDBROUTERCLIENT_EXPORT ReadFieldDouble(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, double* value);
int RTDBROUTERCLIENT_EXPORT ReadFieldString(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, char* buffer, int buffersize);
int RTDBROUTERCLIENT_EXPORT ReadFieldDateTime(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, double* value);
//!����ʵʱ����
/*!
\param fam ��������Ӧ��̬
\param app ��������Ӧ��
\param db ��������ʵʱ��
\param table �����������ݱ�
\param field ���������ֶ�
\param row ���������У���1��ʼ
\param buffer ���뻺�������洢��д������
\param buffersize ���뻺������С
\param value ���뻺�������洢��д������
\return ���ִ�гɹ�������true  \n
		���ִ��ʧ�ܣ�����false
*/
//int WriteField(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, void* buffer, int buffersize);
//int WriteFieldInt2(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, short value);
//int WriteFieldInt4(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, int value);
//int WriteFieldInt8(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, long long value);
//int WriteFieldFloat(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, float value);
//int WriteFieldDouble(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, double value);
//int WriteFieldString(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, char* buffer, int buffersize);
//int WriteFieldDateTime(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, double value);

}
#endif