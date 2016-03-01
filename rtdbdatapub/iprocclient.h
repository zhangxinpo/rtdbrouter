#ifndef IPROCCLIENT_H_INCLUDED
#define IPROCCLIENT_H_INCLUDED 1

#ifdef _MSC_VER
	#ifdef PROCCLIENT_LIB
		#define IPROCCLIENT_API __declspec(dllexport)
	#else
		#define IPROCCLIENT_API __declspec(dllimport)
	#endif
#else
	#define IPROCCLIENT_API
#endif
typedef enum
{
    CFGSRV_ROLE_UNKNOWN = 1, //未知的角色
    CFGSRV_ROLE_ENABLED,     //主用角色
    CFGSRV_ROLE_STANDBY,     //备用角色
    CFGSRV_ROLE_DISABLED     //不可用角色
} CFGSRV_ROLE;
/*
 *	1 UNKNOWN,2 ENABLED,3 STANDBY,4 DISABLED
 */

typedef void (*CONFIG_CALLBACK_STATCHANGE) (int role);
typedef void (*PARAM_CHANGE_CB)(const char* key, int value);


extern "C"
{
	IPROCCLIENT_API bool RegAppProc(const char* famName, const char* appName, const char* procName, CONFIG_CALLBACK_STATCHANGE pState, PARAM_CHANGE_CB pParamChangeCB);
	IPROCCLIENT_API bool RegProc(const char* procName, PARAM_CHANGE_CB pParamChangeCB);
	IPROCCLIENT_API bool ReportAvailable(bool bAvailable);
	IPROCCLIENT_API bool ReportRunInfo(char* keys, int* pValues);

	IPROCCLIENT_API bool GetParam(char* keys, int* pValues);


}

#endif