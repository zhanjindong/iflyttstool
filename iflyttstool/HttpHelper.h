#ifndef __HTTPHELPER_H__
#define __HTTPHELPER_H__

#include "stdafx.h"

class HttpHelper{
public:
	string HttpRequest(char * lpHostName,short sPort,char * lpUrl,char * lpMethod,char * lpPostData,int nPostDataLen);
	int Post(char* hostname, char* api, char* parameters);
};
#endif