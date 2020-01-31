#include "OptionList.h"
#include "Data.h"
#include "Log.h"
#include "Constant.h"
#include <algorithm>

/*
请求字段列表
字段名称	值类型	备注
method	字符串opt.tab	
		
响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
result	数组[]	响应字段。数组元素tab字符串
*/
int COptionList::GetTabs( // 有哪些类型
    json_t *jReq,
	json_t *&jRes,
	string &strRetCode
    )
{
    map<string, set<string> >::iterator iter;
	json_t *jResult = json_array();
	for (iter=COptionMap::Instance()->m_mapTabSets.begin(); iter != COptionMap::Instance()->m_mapTabSets.end(); iter++)
	{
		json_array_append_new(jResult, json_string(iter->first.c_str()));
	}
	json_object_set_new(jRes, "method", json_string("opt.tab"));
	json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	json_object_set_new(jRes, "result", jResult);

    return 0;
}

/*
请求字段列表
字段名称	值类型	备注
method	字符串opt.item	
tab	字符串	缺省返回所有item
item	数组[]	需要返回的字段列表
date	整型或空	结束日期，例如20170121
sort	字符串	按照date值做asc或者desc

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
result	数组[{},{},{}…]	响应字段。数组元素{“cde”: xxx, …}
*/
int COptionList::GetItems( // 得到自选列表
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet           = 0;
    size_t sJsonSize   = 0;
    string strTab      = "";
    int iDate          = 0;
    string strSort     = "desc";
	json_t *jItem      = NULL;
	set<string>::iterator iter;
    vector<string> vecItems;
    vecItems.clear();
	
	m_vecOptInfo.clear();
	// 解析json
	jItem = json_object_get(jReq, "tab");
    if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
	    ERROR("GetItems json tab is not string. strRetCode: "<<strRetCode);
		return 1;
    }
	strTab = json_string_value(jItem);
		
	jItem = json_object_get(jReq, "date");
	if (jItem && json_is_integer(jItem)) 
	{
		iDate = json_integer_value(json_object_get(jReq, "date"));
		if (((0 != iDate) && (iDate < 19800000)) || (iDate > 99999999))
		{
			strRetCode = CErrJsonType;
			ERROR("GetItems Get json date value error. strRetCode: "<<strRetCode<<", iDate: "<<iDate);
			return 1;
		}
	}

	jItem = json_object_get(jReq, "item");
	if (!jItem || !json_is_array(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("GetListSub Get json item error. strRetCode: "<<strRetCode);
		return 1;
	}
	sJsonSize = json_array_size(jItem);
	if (0 == sJsonSize)
	{
		strRetCode = CErrJsonType;
		ERROR("GetListSub Get json item error. strRetCode: "<<strRetCode);
		return 1;
	}
	for (int i=0; i<sJsonSize; i++)
	{
		vecItems.push_back(json_string_value(json_array_get(jItem, i)));
	}

	jItem = json_object_get(jReq, "sort");
	if (jItem && json_is_string(jItem)) {
		strSort = json_string_value(jItem);
	}

	if (0 == COptionMap::Instance()->m_mapTabSets.count(strTab))
	{
	    strRetCode = CErrOutOfRange;
		ERROR("Call 0 == COptionMap->m_mapTabSets.count. strTab: "<<strTab<<", strRetCode: "<<strRetCode);
		return 1;
	}

	for (iter=COptionMap::Instance()->m_mapTabSets[strTab].begin(); 
	     iter!=COptionMap::Instance()->m_mapTabSets[strTab].end(); 
		 iter++)
	{
	    if (0 == COptionMap::Instance()->m_mapOption.count(*iter))
    	{
    	    continue;
    	}
		iRet = InsertItem(COptionMap::Instance()->m_mapOption[*iter], iDate, strRetCode);
		if (0 != iRet)
		{
		    ERROR("Call InsertItem(COptionMap->m_mapOption[*iter], iDate, strRetCode error. strRetCode: "
				<<strRetCode<<", iDate: "<<iDate<<", code: "<<*iter);
			return 1;
		}
	}

	if (strSort != "desc")
	{
	    reverse(m_vecOptInfo.begin(), m_vecOptInfo.end());
	}

	// 根据item生成容器元素的信息
	json_object_set_new(jRes, "method", json_string("opt.item"));
	if (0 != ShowResult(vecItems, jRes,  strRetCode))
	{
		ERROR("Call ShowResult error. strRetCode: "<<strRetCode);
		return 1;
	}

    return 0;
}

/*
请求字段列表
字段名称	值类型	备注
method	字符opt.check	
code	数组[]	检查的代码列表[code1, code2, code3, …]
item	数组[]	需要返回的字段列表
date	整型或空	结束日期，例如20170121
sort	字符串	按照date值做asc或者desc

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
result	数组[{},{},{}…]	响应字段。数组元素{“cde”: xxx, …}
*/
int COptionList::CheckItems( // 传入列表，判断哪些符合
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
	int iRet		   = 0;
	size_t sJsonSize   = 0;
	int iDate		   = 0;
	string strSort	   = "desc";
	json_t *jItem	   = NULL;
	vector<string> vecItems;
	vector<string> vecCodes;
	vecItems.clear();
	vecCodes.clear();
	
	m_vecOptInfo.clear();
	// 解析json
	jItem = json_object_get(jReq, "code");
	if (!jItem || !json_is_array(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("GetListSub Get json code error. strRetCode: "<<strRetCode);
		return 1;
	}
	sJsonSize = json_array_size(jItem);
	if (0 == sJsonSize)
	{
		strRetCode = CErrJsonType;
		ERROR("GetListSub Get json code error. strRetCode: "<<strRetCode);
		return 1;
	}
	for (int i=0; i<sJsonSize; i++)
	{
		vecCodes.push_back(json_string_value(json_array_get(jItem, i)));
	}
	
	jItem = json_object_get(jReq, "item");
	if (!jItem || !json_is_array(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("GetListSub Get json item error. strRetCode: "<<strRetCode);
		return 1;
	}
	sJsonSize = json_array_size(jItem);
	if (0 == sJsonSize)
	{
		strRetCode = CErrJsonType;
		ERROR("GetListSub Get json item error. strRetCode: "<<strRetCode);
		return 1;
	}
	for (int i=0; i<sJsonSize; i++)
	{
		vecItems.push_back(json_string_value(json_array_get(jItem, i)));
	}
	
	jItem = json_object_get(jReq, "date");
	if (jItem && json_is_integer(jItem)) 
	{
		iDate = json_integer_value(json_object_get(jReq, "date"));
		if (((0 != iDate) && (iDate < 19800000)) || (iDate > 99999999))
		{
			strRetCode = CErrJsonType;
			ERROR("GetItems Get json date value error. strRetCode: "<<strRetCode<<", iDate: "<<iDate);
			return 1;
		}
	}
	
	jItem = json_object_get(jReq, "sort");
	if (jItem && json_is_string(jItem)) {
		strSort = json_string_value(jItem);
	}
	
	for (int i=0; i<vecCodes.size(); i++)
	{
		if (0 == COptionMap::Instance()->m_mapOption.count(vecCodes[i]))
		{
			continue;
		}
		
		iRet = InsertItem(COptionMap::Instance()->m_mapOption[vecCodes[i]], iDate, strRetCode);
		if (0 != iRet)
		{
			ERROR("Call InsertItem(COptionMap->m_mapOption[vecCodes[i]], iDate, strRetCode) error. strRetCode: "
				<<strRetCode<<", iDate: "<<iDate<<", code: "<<vecCodes[i]);
			return 1;
		}
	}

	if (strSort != "desc")
	{
	    reverse(m_vecOptInfo.begin(), m_vecOptInfo.end());
	}
	
	// 根据item生成容器元素的信息
	json_object_set_new(jRes, "method", json_string("opt.check"));
	if (0 != ShowResult(vecItems, jRes,  strRetCode))
	{
		ERROR("Call ShowResult error. strRetCode: "<<strRetCode);
		return 1;
	}
	
	return 0;
}

/*
请求字段列表
字段名称	值类型	备注
method	字符串opt.modify	
code	字符串	需要修改的编码
item	对象	需要修改的字段值{“tab”: xxx, “note”: xxx, …}

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
*/
int COptionList::ModifyItems(  // 修改列表
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
	)
{
    return 1;
}

int COptionList::InsertItem( // 根据iDate过滤，根据日期降序排列
	COptionInfo* ptrItem, 
	int iDate, 
	string &strRetCode
	)
{
    if ((0 != iDate) && (iDate < ptrItem->m_iCreateDate))
	{
	    return 0;
	}

    int i=0;
	for (; i<m_vecOptInfo.size(); i++)
	{
	    if (ptrItem->m_iCreateDate > m_vecOptInfo[i]->m_iCreateDate)
	    {
	    	break;
	    }
	}

	if (i >= m_vecOptInfo.size())
	{
	    m_vecOptInfo.push_back(ptrItem);
	}
	else
	{
	   m_vecOptInfo.insert(m_vecOptInfo.begin()+i, ptrItem);
	}
	
    return 0;
}

int COptionList::ShowResult(
	vector<string> vecItems,
	json_t *&jRes,
	string &strRetCode
	)
{
    char caTemp[64];
	string strItemVal = "";
	json_t *jResult = json_array();

	DEBUG("m_vecOptInfo.size(): "<<m_vecOptInfo.size());
	for (int i=0; i<m_vecOptInfo.size(); i++)
	{
		json_t *jItem = json_object();
		DEBUG("vecItems.size(): "<<vecItems.size());
		for (int j=0; j<vecItems.size(); j++)
		{
		    strItemVal = "";
		    if (vecItems[j] == "code")
	    	{
	    	    strItemVal = m_vecOptInfo[i]->m_strCode;
	    	}
			if (vecItems[j] == "tab")
			{
				strItemVal = m_vecOptInfo[i]->m_strTab;
			}
			if (vecItems[j] == "note")
			{
				strItemVal = m_vecOptInfo[i]->m_strNote;
			}
			if (vecItems[j] == "date")
			{
			    sprintf(caTemp, "%d", m_vecOptInfo[i]->m_iCreateDate);
				strItemVal = caTemp;
			}
			if (false == strItemVal.empty())
			{
				json_object_set_new(jItem, vecItems[j].c_str(), json_string(strItemVal.c_str()));
			}
		}
		json_array_append_new(jResult, jItem);
	}
	
	json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	json_object_set_new(jRes, "result", jResult);
	
	return 0;
}

