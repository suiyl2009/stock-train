#ifndef __STR_SPLIT_H__
#define __STR_SPLIT_H__

#include <iostream>
#include <string>
#include <vector>

#include <cctype>  
#include <iostream>  
#include <algorithm>  
  
using namespace std;  
  
inline string& ltrim(string &str) {  
    string::iterator p = find_if(str.begin(), str.end(), not1(ptr_fun<int, int>(isspace)));  
    str.erase(str.begin(), p);  
    return str;  
}  
  
inline string& rtrim(string &str) {  
    string::reverse_iterator p = find_if(str.rbegin(), str.rend(), not1(ptr_fun<int , int>(isspace)));  
    str.erase(p.base(), str.end());  
    return str;  
}  
  
inline string& trim(string &str) {  
    ltrim(rtrim(str));  
    return str;  
}  

class CStrSplit
{
public:
	int SetString(
		string strValue,
		string strPattern
		);

	string Item(
		int i
		);

	int Count();

	vector<string> m_vecItem;
};

#endif