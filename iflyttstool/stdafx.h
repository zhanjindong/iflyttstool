// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

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
// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�


