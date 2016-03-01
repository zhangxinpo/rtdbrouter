#include "rtdbdatapub_global.h"
#include <iostream>
#include "publishthread.h"
#include <QString>
//#include "rtdbintf.h"

/*
1、读命令行参数（fam,app,db）确定负责的实时库
2、注册主备，订阅主备通知
3、连接rtdbrouter
4、接到客户端消息
5、switch订阅消息：如果是自己负责的实时库，加入订阅链表
6、switch取消订阅消息，如果是负责的订阅，移出链表
6、switch刷新消息，更新订阅生命期
7、启动定时线程
8、1秒钟发布一次订阅数据
9、100秒没有接到刷新消息的订阅，移出订阅链表
*/
int main (int argc, char* argv[])
{
	for ( int i = 0; i < argc - 1 ; ++i)
	{
		QString strTemp;
		if ( strcmp(argv[i], "-f") == 0 )
		{
			strTemp = argv[++i];
			CRTDBDATAPUB_GLOBAL::Instance()->myFam = strTemp.toUpper();
		}

		if ( strcmp(argv[i], "-a") == 0 )
		{
			strTemp = argv[++i];
			CRTDBDATAPUB_GLOBAL::Instance()->myApp = strTemp.toUpper();
		}

		if ( strcmp(argv[i], "-d") == 0 )
		{
			strTemp = argv[++i];
			CRTDBDATAPUB_GLOBAL::Instance()->myDB = strTemp.toUpper();
		}	
	}
	//注册主备，当前默认为主

	if ( CRTDBDATAPUB_GLOBAL::Instance()->RegisterToAppManager() == false )
	{
		std::cerr << "Register to AppManager failed and exit!" << std::endl;
		return -1;
	}


	//初始化，连接中心节点
	if ( CRTDBDATAPUB_GLOBAL::Instance()->Initilize()  == false)
	{
		return -1;
	}
	CRTDBDATAPUB_GLOBAL::Instance()->AddRefCount();

	//openrtdb
	CRTDBDATAPUB_GLOBAL::Instance()->OpenRtdb();

	//启动定时发布线程
	CPublishThread publishThread;
	publishThread.start();

	//接收消息
	while(true)
	{
		CRTDBDATAPUB_GLOBAL::Instance()->ProcessRequest();
	}
	
	CRTDBDATAPUB_GLOBAL::Instance()->ReleaseRef();

    return 0;
}