// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <map>

#ifndef WINVER				
#define WINVER 0x0501		
#endif

#ifndef _WIN32_WINNT		
#define _WIN32_WINNT 0x0501	
#endif						

#ifndef _WIN32_WINDOWS		
#define _WIN32_WINDOWS 0x0410 
#endif

#ifndef _WIN32_IE			
#define _WIN32_IE 0x0600	
#endif

#define WIN32_LEAN_AND_MEAN		

#include <windows.h>
#include <mmsystem.h> 


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _BLADEDLL

#include "BladeMP3EncDLL.h"
#include "Format.h"

using namespace std;
// TODO: 在此处引用程序需要的其他头文件


