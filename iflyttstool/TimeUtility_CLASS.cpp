#include "stdafx.h"
#include "TimeUtility_CLASS.h"
#include <time.h>

string TimeUtility::GetLocalTimeString()
{
	string result;
	struct tm *local;
    time_t t;
	//rawtime
    t=time(NULL);
	//localtime
    local=localtime(&t);

	//UTC time
	//local=gmtime(&t);

	//201305191604
	char cstr[13];
	strftime(cstr,13,"%Y%m%d%H%M",local);

	return string(cstr);
}

string TimeUtility::GetLocalTimeString2()
{
	string result;
	struct tm *local;
    time_t t;
	//rawtime
    t=time(NULL);
	//localtime
    local=localtime(&t);

	//UTC time
	//local=gmtime(&t);

	//2013-05-19 16:04:12
	char cstr[20];
	strftime(cstr,20,"%Y-%m-%d %H:%M:%S",local);

	return string(cstr);
}