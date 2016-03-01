#include "rtdbrouterclient.h"
#include "rtdbdatasub_global.h"
#include "zmq.h"

//!初始化工作
/*!
\param key 预留参数，将来可能一台机器上不同进程需要连接不同消息总线等策略
\return 如果执行成功，返回true  \n
		如果执行失败，返回false
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
//!反初始化，退出前清理工作
/*!
\return 如果执行成功，返回true  \n
		如果执行失败，返回false
*/
bool Deinitialize()
{
	CRTDBDATASUB_GLOBAL::Instance()->ReleaseRef();
	return true;
}

//!创建一个订阅
/*!
\param refreshpriod 该订阅要求的刷新周期
\return 如果执行成功，返回该订阅的句柄  \n
		如果执行失败，返回0
*/
//int CreateSubscriptionHandle(int /*refreshpriod*/)
//{
//	return CRTDBDATASUB_GLOBAL::Instance()->CreateSubscriptionHandle();
//}

//!增加订阅数据
/*!
\param handle 订阅句柄
\param fam 订阅数据所属应用态
\param app 订阅数据所属应用
\param db 订阅数据所属实时库
\param table 订阅数据所属数据表
\param field 订阅数据所属字段
\parma row 订阅数据所在行，从1开始
\return 如果执行成功，返回true  \n
		如果执行失败，返回false
*/
//bool AddSubRTDBData(int handle, const char* fam, const char* app, const char* db, const char* table, const char* field, int row)
//{
//	return CRTDBDATASUB_GLOBAL::Instance()->AddSubRTDBData( handle, fam, app, db, table, field, row);
//}

//!启动一个订阅
/*!
\param handle 该订阅的句柄
\return 如果执行成功，返回true  \n
		如果执行失败，返回false
*/
//bool RegisterSubscript(int handle)
//{
//	return CRTDBDATASUB_GLOBAL::Instance()->RegisterSubscript(handle);
//}
//!取消一个订阅
/*!
\param handle 该订阅的句柄
\return 如果执行成功，返回true  \n
		如果执行失败，返回false
*/
//bool CancelSubscript(int handle)
//{
//	return CRTDBDATASUB_GLOBAL::Instance()->CancelSubscript(handle);
//}
//!刷新一个订阅（心跳）
/*!
\param handle 该订阅的句柄
\return 如果执行成功，返回true  \n
		如果执行失败，返回false
*/
//bool RefreshSubscript(int handle)
//{
//	return CRTDBDATASUB_GLOBAL::Instance()->RefreshSubscript(handle);
//}

//!读取实时数据
/*!
\param fam 数据所属应用态
\param app 数据所属应用
\param db 数据所属实时库
\param table 数据所属数据表
\param field 数据所属字段
\param row 数据所在行，从1开始
\param buffer 输出缓存区，存储读到的数据
\param buffersize 输出缓存区大小
\param value 输出缓存区，存储读到的数据
\return 如果执行成功，返回正整数（>=0 具体代码待定）  \n
		如果执行失败，返回负整数（< 0 具体代码待定）
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
//!更新实时数据