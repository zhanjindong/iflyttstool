#include "stdafx.h"
#include <string>  
#include "Wininet.h"  
#include "HttpHelper.h"

#pragma comment(lib,"Wininet.lib") 

string HttpHelper::HttpRequest(char * lpHostName,short sPort,char * lpUrl,char * lpMethod,char * lpPostData,int nPostDataLen)
{
	HINTERNET hInternet,hConnect,hRequest;  

	BOOL bRet;  

	std::string strResponse;  
	string ctheader="Content-Type: text/xml;charset=UTF-8 \r\n";

	hInternet = (HINSTANCE)InternetOpen("User-Agent",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);  
	if(!hInternet)  
		goto Ret0;  

	hConnect = (HINSTANCE)InternetConnect(hInternet,lpHostName,sPort,NULL,"HTTP/1.1",INTERNET_SERVICE_HTTP,0,0);  
	if(!hConnect)  
		goto Ret0;  

	hRequest = (HINSTANCE)HttpOpenRequest(hConnect,lpMethod,lpUrl,"HTTP/1.1",NULL,NULL,INTERNET_FLAG_RELOAD,0);  
	if(!hRequest)  
		goto Ret0;  

	
	bRet = HttpAddRequestHeaders(hRequest,ctheader.data(),ctheader.length(),HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD);  
	if(!bRet)  
	goto Ret0;  

	bRet = HttpSendRequest(hRequest,NULL,0,lpPostData,nPostDataLen);  
	while(TRUE)  
	{  
		char cReadBuffer[4096];  
		unsigned long lNumberOfBytesRead;  
		bRet = InternetReadFile(hRequest,cReadBuffer,sizeof(cReadBuffer) - 1,&lNumberOfBytesRead);  
		if(!bRet || !lNumberOfBytesRead)  
			break;  
		cReadBuffer[lNumberOfBytesRead] = 0;  
		strResponse = strResponse + cReadBuffer;  
	}  

Ret0:  
	if(hRequest)  
		InternetCloseHandle(hRequest);  
	if(hConnect)  
		InternetCloseHandle(hConnect);  
	if(hInternet)  
		InternetCloseHandle(hInternet);  

	return strResponse;  
}