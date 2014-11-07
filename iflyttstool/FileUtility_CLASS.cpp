#include "stdafx.h"
#include <iostream>  
#include <fstream> 
#include <string>
#include <vector> 
#include <fstream>
#include <sstream>
#include "errorcode.h"
#include "FILEUTILITY_CLASS.h"
#include "TimeUtility_CLASS.h"

string FileUtility::ReadFile(const char * fileName,int * ret)
{
	  ifstream ifs(fileName);
      if (!ifs)
      {
		  * ret=-1;
		  return "";
	  }
      ostringstream buffer;
      buffer << ifs.rdbuf();
      string context(buffer.str());
      ifs.close();
	  * ret=0;
	  return context;
}

int FileUtility::WriteLogToFlie(string logpath,string content)
{

	logpath+=TimeUtility::GetLocalTimeString()+".log";
	FILE *fp;
	fp=fopen(logpath.data(),"a");
	if(fp==NULL)
	{
		return CUST_ERROR_OPENFILEFAIL;
	}

	string curtime=TimeUtility::GetLocalTimeString2();
	fputs((curtime+":"+content).data(),fp);
	fclose(fp);
	return 0;
}
