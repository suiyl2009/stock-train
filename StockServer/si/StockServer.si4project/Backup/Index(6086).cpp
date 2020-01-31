#include <iostream>
#include <string>
#include <map>
#include <algorithm>

#include "Index.h"
#include "Log.h"
#include "Constant.h"
#include "IndexData.h"

using namespace std;

int CIndex::list(
    json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
    size_t sJsonSize   = 0;
	int nDate          = 0;
	string strTemp     = "";
	map<string, CIndexData*>::iterator iter;
	CIndexData *ptrIndex = NULL;
    vector<CIndexData*> m_vecData;
    vector<int> m_vecIdx;
    int nIdx = 0;
    char temp[64] = {0};

    // 解析json，得到各字段item,date
    json_t *jDate = json_object_get(jReq, "date");
    if (jDate && json_is_integer(jDate)) 
	{
		nDate = json_integer_value(json_object_get(jReq, "date"));
		if (((0 != nDate) && (nDate < 19800000)) || (nDate > 99999999))
		{
	        strRetCode = CErrJsonType;
			ERROR("GetListAll Get json date value error. strRetCode: "<<strRetCode<<", nDate: "<<nDate);
			return 1;
		}
    }

	// 从stockmap中找到满足条件的放入容器
	for (iter=CIndexMap::Instance()->m_mapIndex.begin(); iter!=CIndexMap::Instance()->m_mapIndex.end(); iter++)
	{
	    ptrIndex = iter->second;
        nIdx = ptrIndex->GetDayIdx(nDate);
		if (0 <= ptrIndex)
		{
		    m_vecData.push_back(ptrIndex);
            m_vecIdx.push_back(nIdx);
		}
	}
	
	// 根据item生成容器元素的信息
	json_object_set_new(jRes, "method", json_string("index.list"));
	json_t *jResult = json_array();
	for (int i=0; i<m_vecData.size(); i++)
	{
		json_t *jItem = json_object();
        json_object_set_new(jItem, "Code", json_string(m_vecData[i]->m_strCode.c_str()));
        json_object_set_new(jItem, "Name", json_string(m_vecData[i]->m_strName.c_str()));
        json_object_set_new(jItem, "Exchange", json_string(m_vecData[i]->m_strExchange.c_str()));
        sprintf(temp, "%d", m_vecData[i]->m_vecDayData[m_vecIdx[i]].close);
        json_object_set_new(jItem, "Close", json_string(temp));
        sprintf(temp, "%d", m_vecData[i]->m_vecDayData[m_vecIdx[i]].date);
        json_object_set_new(jItem, "Date", json_string(temp));
		json_array_append_new(jResult, jItem);
	}
	
	json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	json_object_set_new(jRes, "result", jResult);

    return 0;
}

int CIndex::single(
    json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
    string strCode = "";
    int nDate = 0;
    int nBeforeCnt = 200;
    int nAfterCnt  = 0;
    int nBeforeIdx = 0;
    int nAfterIdx = 0;
    int nMin = 0;
    int nMax = 0;
    map<string, CIndexData*>::iterator iter;
    CIndexData *ptrIndex = NULL;
    
    // 解析json，得到各字段item,date
    json_t *jCode = json_object_get(jReq, "code");
    if (!jCode || !json_is_string(jCode)) {
        strRetCode = CErrJsonType;
        ERROR("GetLines json code is not string. strRetCode: "<<strRetCode);
        return 1;
    }
    strCode = json_string_value(jCode);
    
    json_t *jDate = json_object_get(jReq, "date");
    if (jDate && json_is_integer(jDate)) 
    {
        nDate = json_integer_value(json_object_get(jReq, "date"));
        if (((0 != nDate) && (nDate < 19800000)) || (nDate > 99999999))
        {
            strRetCode = CErrJsonType;
            ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", nDate: "<<nDate);
            return 1;
        }
    }
    
    json_t *jBefore = json_object_get(jReq, "before");
    if (jBefore && json_is_integer(jBefore)) 
    {
        nBeforeCnt = json_integer_value(json_object_get(jReq, "before"));
        if (nBeforeCnt < -1)
        {
            strRetCode = CErrJsonType;
            ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", nBeforeCnt: "<<nBeforeCnt);
            return 1;
        }
    }
    
    json_t *jAfter = json_object_get(jReq, "after");
    if (jAfter && json_is_integer(jAfter)) 
    {
        nAfterCnt = json_integer_value(json_object_get(jReq, "after"));
        if (nAfterCnt < 0)
        {
            strRetCode = CErrJsonType;
            ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", nAfterCnt: "<<nAfterCnt);
            return 1;
        }
    }

    // 从stockmap中找到满足条件的元素
    iter = CIndexMap::Instance()->m_mapIndex.find(strCode);
    if (iter == CIndexMap::Instance()->m_mapIndex.end())
    {
        strRetCode = CErrOutOfRange;
        ERROR("GetLines strCode not found. strCode: "<<strCode<<", strRetCode: "<<strRetCode);
        return 1;
    }
    ptrIndex = iter->second;

    // 确定显示的区间范围
    if (0 > InfirmShowIdx(ptrIndex, nDate, nBeforeCnt, nAfterCnt, nBeforeIdx, nAfterIdx, strRetCode)){
        ERROR("Call InfirmShowIdx error. strCode: "<<strCode<<", nDate: "<<nDate<<", nBeforeCnt: "<<nBeforeCnt
            <<", nAfterCnt: "<<nAfterCnt<<", strRetCode: "<<strRetCode);
        return 1;
    }
    
    // 生成返回的信息
    json_object_set_new(jRes, "method", json_string("index.single"));
    json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));

    json_t *jYLine = json_array();
    json_t *jXLine = json_array();
    nMax = nMin = ptrIndex->m_vecDayData[nAfterIdx].close;
    for (int i=nAfterIdx; i<=nBeforeIdx; i++){
        json_array_append_new(jXLine, json_integer(ptrIndex->m_vecDayData[i].date));
        json_array_append_new(jYLine, json_integer(ptrIndex->m_vecDayData[i].close));
        nMax = nMax > ptrIndex->m_vecDayData[i].close ? nMax : ptrIndex->m_vecDayData[i].close;
        nMin = nMin < ptrIndex->m_vecDayData[i].close ? nMin : ptrIndex->m_vecDayData[i].close;
    }
    
    json_t *jResult = json_object();
    json_object_set_new(jResult, "code", json_string(strCode.c_str()));
    json_object_set_new(jResult, "max",   json_integer(nMax));	// 全局最大值
	json_object_set_new(jResult, "min",   json_integer(nMin));	// 全局最小值
	json_object_set_new(jResult, "dyline", jYLine);
    json_object_set_new(jResult, "dxline", jXLine);
	json_object_set_new(jRes, "result", jResult);
    
    return 0;
}

int CIndex::batch(
    json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
    return 1;
}

int CIndex::kline(
    json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
    return 1;
}

int CIndex::InfirmShowIdx(CIndexData* ptrIndex, int nDate, int nBeforeCnt, int nAfterCnt, int &nBeforeIdx, int &nAfterIdx, string &strRetCode){
    int nCurrIdx = 0;
    if (0 == nDate){
        nCurrIdx = 0;
    }
    else{
        nCurrIdx = ptrIndex->GetDayIdx(nDate);
        if (0 > nCurrIdx){
            strRetCode = CErrOutOfRange;
            ERROR("Call ptrIndex->GetDayIdx(nDate) error. nDate: "<<nDate<<", strRetCode: "<<strRetCode);
            return -1;
        }
    }
    nBeforeIdx = nCurrIdx + nBeforeCnt;
    if (nBeforeIdx >= ptrIndex->m_vecDayData.size()){
        nBeforeIdx = ptrIndex->m_vecDayData.size() - 1;
    }
    nAfterIdx = nCurrIdx - nAfterCnt;
    if (0 > nAfterIdx){
        nAfterIdx = 0;
    }
    return 0;
}

