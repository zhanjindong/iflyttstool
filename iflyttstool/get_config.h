/****************************************************************************
*   ����:  jasitzhang(����)
*   ����:  2011-10-2
*   Ŀ��:  ��ȡ�����ļ�����Ϣ����map����ʽ����
*   Ҫ��:  �����ļ��ĸ�ʽ����#��Ϊ��ע�ͣ����õ���ʽ��key = value���м���пո�Ҳ��û�пո�
*****************************************************************************/
#ifndef _GET_CONFIG_H_
#define _GET_CONFIG_H_
 
#include "stdafx.h"
 
#define COMMENT_CHAR '#'
 
bool ReadConfig(const string & filename, map<string, string> & m);
void PrintConfig(const map<string, string> & m);
#endif