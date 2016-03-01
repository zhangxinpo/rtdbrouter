#ifndef INODESTATUS_H_INCLUDED
#define INODESTATUS_H_INCLUDED 1

#ifdef _MSC_VER
	#ifdef NODESTATUS_LIB
		#define INODESTATUS_API __declspec(dllexport)
	#else
		#define INODESTATUS_API __declspec(dllimport)
	#endif
#else
	#define INODESTATUS_API
#endif

extern "C"
{
	INODESTATUS_API const char* GetAppByID(int id);
	INODESTATUS_API const char* GetPrimaryComputerNameByID(const char* fam, int id);
	INODESTATUS_API const char* GetPrimaryComputerIPByID(const char* fam, int id);
	INODESTATUS_API const char* GetPrimaryComputerIPByApp(const char* fam, const char* app);
	INODESTATUS_API const char* GetPrimaryComputerNameByApp(const char* fam, const char* app);

	INODESTATUS_API bool ChangeToPrimary(int appstateID);
	INODESTATUS_API bool ChangeToStandby(int appstateID);
	INODESTATUS_API bool ChangeProcParam(int procstateid, char* key, int value);


}

#endif