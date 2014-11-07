#ifndef __FILEUTITLY_H__
#define __FILEUTITLY_H__

#include "stdafx.h"

class FileUtility{
public:
	static string ReadFile(const char * fileName,int * ret);
	static int WriteLogToFlie(string logpath,string content);
};
#endif