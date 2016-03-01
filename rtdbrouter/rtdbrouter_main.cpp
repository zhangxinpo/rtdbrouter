#include "zmq.h"
#include <iostream>
#include <stdio.h>
#include <QLibrary>
#include "icssettings.h"

int nSendPortForServer = 0;
int nSubscriberPortForServer = 0;
int nSendPortForClient = 0;
int nSubscriberPortForClient = 0;

bool LoadConfig()
{
 //<section description="系统平台" key="plat_sys">
 //  <section description="实时库路由器" key="rtdbrouter">
 //    <item description="IP地址" key="ipaddress" modify="false" value="192.168.0.254" type="String"/>
 //    <item description="服务端发送端口" key="sendportforserver" modify="false" value="5555" type="Int"/>
 //    <item description="服务端订阅端口" key="subscriberportforserver" modify="false" value="5556" type="Int"/>
 //    <item description="客户端发送端口" key="sendportforclient" modify="false" value="6666" type="Int"/>
 //    <item description="客户端订阅端口" key="subscriberportforclient" modify="false" value="6667" type="Int"/>
 //  </section>

	QLibrary icssettingLib("icssettings");
	if ( icssettingLib.load() == false )
	{
		printf("load icssetings failed\n");
		return false;
	}

	typedef bool (*PMETHOD)(char*, IICSUnknown**);

	PMETHOD pGetClassObject = (PMETHOD) icssettingLib.resolve("GetClassObject");

	if(!pGetClassObject)
	{
		printf("Resolve function \"GetClassobject\" failed!\n");
		return false;
	}

	IICSUnknown * pUnknown = NULL;
	(*pGetClassObject)(CLSID_ICSSettings,(IICSUnknown**)&pUnknown);
	if(pUnknown == NULL)
	{
		printf("get ICSSetting interface failed\n");
		return false;
	}
	
	IICSSettings* pSettings = NULL;
	pUnknown->QueryInterface(IID_ICSSettings, (void**)&pSettings);
	pUnknown->Release();
	if(pSettings == NULL)
	{
		printf("get ICSSetting interface failed\n");
		return false;
	}

	char szBuffer[1024];
	bool bOK = false;
	bOK = pSettings->SetCurrentSectionPath("plat_sys/rtdbrouter");
	if ( !bOK )
	{
		printf("please add plat_sys/rtdbrouterport section in icssetting.xml file\n");
		return false;
	}
	pSettings->ItemReadString("ipaddress", szBuffer, sizeof(szBuffer));

	nSendPortForServer = pSettings->ItemReadInt("sendportforserver");
	nSubscriberPortForServer = pSettings->ItemReadInt("subscriberportforserver");
	nSendPortForClient = pSettings->ItemReadInt("sendportforclient");
	nSubscriberPortForClient = pSettings->ItemReadInt("subscriberportforclient");

	pSettings->Release();

	return true;
}

int main (void)
{
	if ( !LoadConfig() )
	{
		printf("load config failed!");
		return -1;
	}

	char szBuffer[256];
    //  Prepare our context and sockets
    void *context = zmq_ctx_new ();
    void *xpull_forclient = zmq_socket (context, ZMQ_PULL);
  	sprintf(szBuffer, "tcp://*:%d", nSendPortForClient);
    zmq_bind (xpull_forclient, szBuffer);
	 void *xpub_forclient  = zmq_socket (context, ZMQ_PUB);
	sprintf(szBuffer, "tcp://*:%d", nSubscriberPortForClient);
    zmq_bind (xpub_forclient,  szBuffer);

    void *xpull_forserver = zmq_socket (context, ZMQ_PULL);
	sprintf(szBuffer, "tcp://*:%d", nSendPortForServer);
    zmq_bind (xpull_forserver, szBuffer);
	void *xpub_forserver  = zmq_socket (context, ZMQ_PUB);
	sprintf(szBuffer, "tcp://*:%d", nSubscriberPortForServer);
    zmq_bind (xpub_forserver, szBuffer);

    //  Initialize poll set
    zmq_pollitem_t items [] = {
        { xpull_forclient, 0, ZMQ_POLLIN, 0 },
        { xpull_forserver,  0, ZMQ_POLLIN, 0 }
    };

    //  Switch messages between sockets
    while (1) {
        zmq_msg_t message;
        zmq_poll (items, 2, -1);
        if (items [0].revents & ZMQ_POLLIN) {
            while (1) {
				//std::cout << "receive client msg" << std::endl;
                //  Process all parts of the message
                zmq_msg_init (&message);
                zmq_msg_recv (&message, xpull_forclient, 0);
                int more = zmq_msg_more (&message);
                zmq_msg_send (&message, xpub_forserver, more? ZMQ_SNDMORE: 0);
                zmq_msg_close (&message);
                if (!more)
                    break;      //  Last message part
            }
        }
        if (items [1].revents & ZMQ_POLLIN) {
            while (1) {
				//std::cout << "receive server msg" << std::endl;
                //  Process all parts of the message
                zmq_msg_init (&message);
                zmq_msg_recv (&message, xpull_forserver, 0);
                int more = zmq_msg_more (&message);
                zmq_msg_send (&message, xpub_forclient, more? ZMQ_SNDMORE: 0);
                zmq_msg_close (&message);
                if (!more)
                    break;      //  Last message part
            }
        }
    }
    //  We never get here, but clean up anyhow
    zmq_close (xpull_forclient);
    zmq_close (xpub_forclient);
    zmq_close (xpull_forserver);
    zmq_close (xpub_forserver);

    zmq_ctx_destroy (context);
    return 0;
}