#ifndef __STRINGTONUM_H__
#define __STRINGTONUM_H__

#include <iostream>  
#include <sstream>    //使用stringstream需要引入这个头文件  
using namespace std; 

template <class Type>  
Type StringToNum(const string& str)  
{  
	istringstream iss(str);  
	Type num;  
	iss >> num;  
	return num;      
}  

#endif