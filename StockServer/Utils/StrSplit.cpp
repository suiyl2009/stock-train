//#include "stdafx.h"
#include "StrSplit.h"

int CStrSplit::SetString(
	string strValue,
	string strPattern
	)
{
	m_vecItem.clear();
	std::string::size_type pos;
	strValue+=strPattern;//扩展字符串以方便操作
	int size=strValue.size();
 
	for(int i=0; i<size; i++)
	{
		pos=strValue.find(strPattern,i);
		if(pos<size)
		{
			std::string s=strValue.substr(i,pos-i);
			m_vecItem.push_back(s);
			i=pos+strPattern.size()-1;
		}
	}
	return m_vecItem.size();
}

string CStrSplit::Item(
	int i
	)
{
	if (i >= m_vecItem.size())
	{
		return "";
	}
	return m_vecItem[i];
}

int CStrSplit::Count()
{
	return m_vecItem.size();
}
