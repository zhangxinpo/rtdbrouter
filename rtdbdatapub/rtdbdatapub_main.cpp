#include "rtdbdatapub_global.h"
#include <iostream>
#include "publishthread.h"
#include <QString>
//#include "rtdbintf.h"

/*
1���������в�����fam,app,db��ȷ�������ʵʱ��
2��ע����������������֪ͨ
3������rtdbrouter
4���ӵ��ͻ�����Ϣ
5��switch������Ϣ��������Լ������ʵʱ�⣬���붩������
6��switchȡ��������Ϣ������Ǹ���Ķ��ģ��Ƴ�����
6��switchˢ����Ϣ�����¶���������
7��������ʱ�߳�
8��1���ӷ���һ�ζ�������
9��100��û�нӵ�ˢ����Ϣ�Ķ��ģ��Ƴ���������
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
	//ע����������ǰĬ��Ϊ��

	if ( CRTDBDATAPUB_GLOBAL::Instance()->RegisterToAppManager() == false )
	{
		std::cerr << "Register to AppManager failed and exit!" << std::endl;
		return -1;
	}


	//��ʼ�����������Ľڵ�
	if ( CRTDBDATAPUB_GLOBAL::Instance()->Initilize()  == false)
	{
		return -1;
	}
	CRTDBDATAPUB_GLOBAL::Instance()->AddRefCount();

	//openrtdb
	CRTDBDATAPUB_GLOBAL::Instance()->OpenRtdb();

	//������ʱ�����߳�
	CPublishThread publishThread;
	publishThread.start();

	//������Ϣ
	while(true)
	{
		CRTDBDATAPUB_GLOBAL::Instance()->ProcessRequest();
	}
	
	CRTDBDATAPUB_GLOBAL::Instance()->ReleaseRef();

    return 0;
}