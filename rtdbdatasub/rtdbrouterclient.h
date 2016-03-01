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
为了优化人机界面远程访问实时库远程效率，设计该接口
基本思路是一次订阅一个页面的所有数据，由服务端周期发布订阅的数据
1、程序启动初始化Initialize
2、打开一个页面，创建一个订阅CreateSubscriptionHandle
3、遍历页面，发现实时数据就增加订阅数据AddSubRTDBData
4、遍历完毕页面，启动订阅RegisterSubscript
5、定时刷新页面RefreshSubscript
6、读取实时数据ReadField
7、关闭页面，取消订阅CancelSubscript
8、结束进程，反初始化Deinitialize
*/

extern "C" {
//!初始化工作
/*!
\param key 预留参数，将来可能一台机器上不同进程需要连接不同消息总线等策略
\return 如果执行成功，返回true  \n
		如果执行失败，返回false
*/
bool RTDBROUTERCLIENT_EXPORT Initialize(const char* key = 0);
//!反初始化，退出前清理工作
/*!
\return 如果执行成功，返回true  \n
		如果执行失败，返回false
*/
bool RTDBROUTERCLIENT_EXPORT Deinitialize();

//!创建一个订阅
/*!
\param refreshpriod 该订阅要求的刷新周期
\return 如果执行成功，返回该订阅的句柄  \n
		如果执行失败，返回0
*/
//int RTDBROUTERCLIENT_EXPORT CreateSubscriptionHandle(int refreshpriod=1);

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
//bool RTDBROUTERCLIENT_EXPORT AddSubRTDBData(int handle, const char* fam, const char* app, const char* db, const char* table, const char* field, int row);

//!启动一个订阅
/*!
\param handle 该订阅的句柄
\return 如果执行成功，返回true  \n
		如果执行失败，返回false
*/
//bool RTDBROUTERCLIENT_EXPORT RegisterSubscript(int handle);
//!取消一个订阅
/*!
\param handle 该订阅的句柄
\return 如果执行成功，返回true  \n
		如果执行失败，返回false
*/
//bool RTDBROUTERCLIENT_EXPORT CancelSubscript(int handle);
//!刷新一个订阅（心跳）
/*!
\param handle 该订阅的句柄
\return 如果执行成功，返回true  \n
		如果执行失败，返回false
*/
//bool RTDBROUTERCLIENT_EXPORT RefreshSubscript(int handle);

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
int RTDBROUTERCLIENT_EXPORT ReadField(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, void* buffer, int buffersize);
int RTDBROUTERCLIENT_EXPORT ReadFieldInt2(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, short* value);
int RTDBROUTERCLIENT_EXPORT ReadFieldInt4(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, int* value);
int RTDBROUTERCLIENT_EXPORT ReadFieldInt8(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, long long* value);
int RTDBROUTERCLIENT_EXPORT ReadFieldFloat(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, float* value);
int RTDBROUTERCLIENT_EXPORT ReadFieldDouble(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, double* value);
int RTDBROUTERCLIENT_EXPORT ReadFieldString(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, char* buffer, int buffersize);
int RTDBROUTERCLIENT_EXPORT ReadFieldDateTime(const char* fam, const char* app, const char* db, const char* table, const char* field, int row, double* value);
//!更新实时数据
/*!
\param fam 数据所属应用态
\param app 数据所属应用
\param db 数据所属实时库
\param table 数据所属数据表
\param field 数据所属字段
\param row 数据所在行，从1开始
\param buffer 输入缓存区，存储欲写的数据
\param buffersize 输入缓存区大小
\param value 输入缓存区，存储欲写的数据
\return 如果执行成功，返回true  \n
		如果执行失败，返回false
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