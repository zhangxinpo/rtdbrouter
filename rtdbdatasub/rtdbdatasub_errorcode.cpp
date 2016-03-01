#include "rtdbdatasub_errorcode.h"

static const char* rtdbdatasub_strerror_array[] =
{ 
	"RETURNCODE_RTDBROUTER_SUCCESS",
	"RETURNCODE_RTDBROUTER_NOTSUBSCRIPT",
	"RETURNCODE_RTDBROUTER_NOTREFRESH"
};

const char* rtdbdatasub_strerror(int errorcode)
{
	return rtdbdatasub_strerror_array[0 - errorcode];
}
