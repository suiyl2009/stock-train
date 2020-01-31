#include <iostream>
#include <string>
#include <map>
#include <algorithm>

#include "Index.h"
#include "Log.h"
#include "Constant.h"
#include "IndexData.h"
#include "Calendar.h"

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
        if (0 == nDate){
            nIdx = 0;
            if (0 == ptrIndex->m_vecDayData.size()){
                nIdx = -1;
            }
        }
        else {
            nIdx = ptrIndex->GetDayIdx(nDate);
        }
        
		if (0 <= nIdx)
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
        sprintf(temp, "%.2f", m_vecData[i]->m_vecDayData[m_vecIdx[i]].close/100.0);
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

    // 从indexmap中找到满足条件的元素
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
    int iRet = 0;
    string strCode = "";
    string strCycle = "";
    int nDate = 0;
    int nBeforeCnt = 0;
    int nAfterCnt = 0;
    int nPivotIdx = 0;
    json_t *jItem = NULL;
    
    map<string, CIndexData*>::iterator iter;
    CIndexData *ptrIndex = NULL;
    vector<CKData> vecKData;
    int nKMax = 0;
    int nKMin = 0;
    int nVolMax = 0;
    int nVolMin = 0;

    // 解析请求串，得到参数数据
    //strCode = ;
    //m_strCycle = ;
    //m_strRecover = ;
    //时间段
    
    // 解析json，得到各字段item,date
    jItem = json_object_get(jReq, "code");
    if (!jItem || !json_is_string(jItem)) {
        strRetCode = CErrJsonType;
        ERROR("GetLines json code is not string. strRetCode: "<<strRetCode);
        return 1;
    }
    strCode = json_string_value(jItem);
    
    jItem = json_object_get(jReq, "cycle");
    if (!jItem || !json_is_string(jItem)) {
        strRetCode = CErrJsonType;
        ERROR("GetLines json cycle is not string. strRetCode: "<<strRetCode);
        return 1;
    }
    strCycle = json_string_value(jItem);
    
    jItem = json_object_get(jReq, "date");
    if (jItem && json_is_integer(jItem)) 
    {
        nDate = json_integer_value(json_object_get(jReq, "date"));
        if (((0 != nDate) && (nDate < 19800000)) || (nDate > 99999999))
        {
            strRetCode = CErrJsonType;
            ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", nDate: "<<nDate);
            return 1;
        }
    }
    
    jItem = json_object_get(jReq, "before");
    if (jItem && json_is_integer(jItem)) 
    {
        nBeforeCnt = json_integer_value(json_object_get(jReq, "before"));
        if (nBeforeCnt < -1)
        {
            strRetCode = CErrJsonType;
            ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", nBeforeCnt: "<<nBeforeCnt);
            return 1;
        }
    }
    
    jItem = json_object_get(jReq, "after");
    if (jItem && json_is_integer(jItem)) 
    {
        nAfterCnt = json_integer_value(json_object_get(jReq, "after"));
        if (nAfterCnt < 0)
        {
            strRetCode = CErrJsonType;
            ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", nAfterCnt: "<<nAfterCnt);
            return 1;
        }
    }

    // 从indexmap中找到满足条件的元素
    iter = CIndexMap::Instance()->m_mapIndex.find(strCode);
    if (iter == CIndexMap::Instance()->m_mapIndex.end())
    {
        strRetCode = CErrOutOfRange;
        ERROR("GetLines strCode not found. strCode: "<<strCode<<", strRetCode: "<<strRetCode);
        return 1;
    }
	ptrIndex = iter->second;
    if (0 == nDate){
        nPivotIdx = 0;
        if (0 == ptrIndex->m_vecDayData.size()){
            nPivotIdx = -1;
        }
    }
    else {
        nPivotIdx = ptrIndex->GetDayIdx(nDate);
    }
    
    if (0 > nPivotIdx){
        strRetCode = CErrOutOfRange;
        ERROR("ptrIndex->GetDayIdx not found. strCode: "<<strCode<<", nDate: "<<nDate<<", strRetCode: "<<strRetCode);
        return 1;
    }

    if ("day" == strCycle){
        if (0 != GetDayKLine(ptrIndex, nPivotIdx, nBeforeCnt, nAfterCnt, vecKData, strRetCode)){
            ERROR("Call GetDayKLine error. strCode: "<<strCode<<", nDate: "<<nDate<<", strRetCode: "<<strRetCode);
            return 1;
        }
    }
    else if ("week" == strCycle){
        if (0 != GetWeekKLine(ptrIndex, nPivotIdx, nBeforeCnt, nAfterCnt, vecKData, strRetCode)){
            ERROR("Call GetDayKLine error. strCode: "<<strCode<<", nDate: "<<nDate<<", strRetCode: "<<strRetCode);
            return 1;
        }
    }
    else if ("month" == strCycle){
        if (0 != GetMonthKLine(ptrIndex, nPivotIdx, nBeforeCnt, nAfterCnt, vecKData, strRetCode)){
            ERROR("Call GetDayKLine error. strCode: "<<strCode<<", nDate: "<<nDate<<", strRetCode: "<<strRetCode);
            return 1;
        }
    }
    else{
        strRetCode = CErrOutOfRange;
        ERROR("Don't support this cycle: "<<strCycle<<", strCode: "<<strCode<<", strRetCode: "<<strRetCode);
        return 1;
    }

    json_object_set_new(jRes, "method", json_string("index.kline"));
    json_t *jResult = json_object();
	json_t *jKLine = json_array();  // 季度曲线Y轴坐标
	if (0 < vecKData.size()){
        nKMax = vecKData[0].high;
        nKMin = vecKData[0].low;
        nVolMax = nVolMin = vecKData[0].vol;
    }
	for (int i=0; i< vecKData.size(); i++)
	{
	    nKMax = nKMax > vecKData[i].high ? nKMax : vecKData[i].high;
        nKMin = nKMin < vecKData[i].low ? nKMin : vecKData[i].low;
        nVolMax = nVolMax > vecKData[i].vol ? nVolMax : vecKData[i].vol;
        nVolMin = nVolMin < vecKData[i].vol ? nVolMin : vecKData[i].vol;

        json_t *jItems = json_array();
		json_array_append_new(jItems, json_integer(vecKData[i].date));
		json_array_append_new(jItems, json_integer(vecKData[i].open));
		json_array_append_new(jItems, json_integer(vecKData[i].close));
		json_array_append_new(jItems, json_integer(vecKData[i].high));
		json_array_append_new(jItems, json_integer(vecKData[i].low));
		json_array_append_new(jItems, json_integer((long long)(vecKData[i].amount)));
		json_array_append_new(jItems, json_integer(vecKData[i].vol));
		json_array_append_new(jItems, json_integer(vecKData[i].dayCnt));

		json_array_append_new(jKLine, jItems);
		//DEBUG("amount: "<<m_vecKLine[i].amount<<", vol: "<<m_vecKLine[i].vol);
	}
	json_object_set_new(jResult, "kline", jKLine);

	json_object_set_new(jResult, "kmax",   json_integer(nKMax));	// 全局最大值
	json_object_set_new(jResult, "kmin",   json_integer(nKMin));	// 全局最小值
	json_object_set_new(jResult, "volmax", json_integer(nVolMax));	// 全局最大值
	json_object_set_new(jResult, "volmin", json_integer(nVolMin));	// 全局最小值

	json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	json_object_set_new(jRes, "result", jResult);

    return 0;
}

int CIndex::InfirmShowIdx(
    CIndexData* ptrIndex, 
    int nDate, 
    int nBeforeCnt, 
    int nAfterCnt, 
    int &nBeforeIdx, 
    int &nAfterIdx, 
    string &strRetCode
    )
{
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

int CIndex::GetDayKLine(
    CIndexData *ptrIndex,
    int nIndex,
    int nBeforeCnt,
    int nAfterCnt,
    vector<CKData> &vecKData,
    string &strRetCode
    )
{
    int nBeforeIdx = 0;
    int nAfterIdx = 0;

    nBeforeIdx = nIndex + nBeforeCnt + 1;
    if (nBeforeIdx >= ptrIndex->m_vecDayData.size()){
        nBeforeIdx = ptrIndex->m_vecDayData.size() - 1;
    }
    nAfterIdx = nIndex - nAfterCnt - 1;
    if (0 > nAfterIdx){
        nAfterIdx = 0;
    }

    vecKData.clear();
    for (int i=nAfterIdx; i<nBeforeIdx; i++){
        CKData objKData;
        objKData.amount  = ptrIndex->m_vecDayData[i].amount;
        objKData.close   = ptrIndex->m_vecDayData[i].close;
        objKData.date    = ptrIndex->m_vecDayData[i].date;
        objKData.high    = ptrIndex->m_vecDayData[i].high;
        objKData.low     = ptrIndex->m_vecDayData[i].low;
        objKData.open    = ptrIndex->m_vecDayData[i].open;
        objKData.vol     = ptrIndex->m_vecDayData[i].vol;
        objKData.dayCnt  = 1;
        vecKData.push_back(objKData);
    }

    return 0;
}

int CIndex::GetWeekKLine(
    CIndexData *ptrIndex,
    int nIndex,
    int nBeforeCnt,
    int nAfterCnt,
    vector<CKData> &vecKData,
    string &strRetCode
    )
{
    int nDayIdx = 0;
    int nDate = ptrIndex->m_vecDayData[nIndex].date;
    CKData objKData;
    CCalendar objCal;
    CCalendar objCalPivot;
    int nInterval = objCalPivot.IntervalDays(nDate);
    if (0 == nInterval)
    {
        strRetCode = CErrOutOfRange;
        ERROR("Call objCal.IntervalDays error. iDate: "<<nDate<<", strRetCode: "<<strRetCode);
        return 1;
    }
    nInterval = nInterval - 2*nInterval;
    if (nDate != objCalPivot.AddDay(nInterval)) // objCal 设置为iDate
    {
        strRetCode = CErrOutOfRange;
        ERROR("Call objCal.AddDay error. nDate: "<<nDate<<", objCalPivot: "<<objCalPivot.GetDate()
            <<", nInterval: "<<nInterval<<", strRetCode: "<<strRetCode);
        return 1;
    }
    if (5 != objCalPivot.m_iWeekDay)  // 只显示满一周的k线，最后日期如果不满一周，则不显示其周k线
    {
        if (0 == objCalPivot.SubDay((7+objCalPivot.m_iWeekDay-5)%7)) // 向前移动到周线收盘处
        {
            strRetCode = CErrOutOfRange;
            ERROR("Call objCal.SubDay error. nDate: "<<nDate<<", day: "<<(7+objCalPivot.m_iWeekDay-5)%7
                <<", strRetCode: "<<strRetCode);
            return 1;
        }
    }
    nDate = objCalPivot.GetDate();

	while ((nIndex < ptrIndex->m_vecDayData.size()) && (ptrIndex->m_vecDayData[nIndex].date > nDate) )
		nIndex++;

    //DEBUG("nDate: "<<nDate<<", nIndex: "<<nIndex<<", DayDate: "<<ptrIndex->m_vecDayData[nIndex].date)
    // 到此, nDate为周线周五枢纽位置，nIndex为与之对应的m_vecDayData下标
    nDayIdx = nIndex - 1;
    objCal = objCalPivot;
    vecKData.clear();
    for (int i=0; i<=nAfterCnt; i++){
        objKData.date    = 0;
        objKData.open    = 0;
        objKData.high    = 0;
        objKData.low     = 0;
        objKData.close   = 0;
        objKData.amount  = 0;
        objKData.vol     = 0;
        objKData.dayCnt  = 0;
        nDate = objCal.AddDay(7);
        for (; (0 <= nDayIdx) && (ptrIndex->m_vecDayData[nDayIdx].date <= nDate); nDayIdx--){
            if (0 == objKData.dayCnt){
                objKData.date    = nDate;
                objKData.open    = ptrIndex->m_vecDayData[nDayIdx].open;
                objKData.high    = ptrIndex->m_vecDayData[nDayIdx].high;
                objKData.low     = ptrIndex->m_vecDayData[nDayIdx].low;
                objKData.close   = ptrIndex->m_vecDayData[nDayIdx].close;
                objKData.amount  = ptrIndex->m_vecDayData[nDayIdx].amount/100000;
                objKData.vol     = ptrIndex->m_vecDayData[nDayIdx].vol/10000;
                objKData.dayCnt  = 1;
                //DEBUG("after i: "<<i<<", nDayIdx: "<<nDayIdx<<", nDate: "<<nDate);
            }
            else{
                objKData.high    = objKData.high > ptrIndex->m_vecDayData[nDayIdx].high 
                                   ? objKData.high : ptrIndex->m_vecDayData[nDayIdx].high;
                objKData.low     = objKData.low < ptrIndex->m_vecDayData[nDayIdx].low
                                   ? objKData.low : ptrIndex->m_vecDayData[nDayIdx].low;
                objKData.close   = ptrIndex->m_vecDayData[nDayIdx].close;
                objKData.amount  += ptrIndex->m_vecDayData[nDayIdx].amount/100000;
                objKData.vol     += ptrIndex->m_vecDayData[nDayIdx].vol/10000;
                objKData.dayCnt  += 1;
            }
        }
        if (0 < objKData.dayCnt){
            vecKData.push_back(objKData);
        }
    }
    reverse(vecKData.begin(), vecKData.end());

    nDayIdx = nIndex;
    objCal = objCalPivot;
    for (int i=0; i<=nBeforeCnt; i++){
        objKData.open    = 0;
        objKData.high    = 0;
        objKData.low     = 0;
        objKData.close   = 0;
        objKData.amount  = 0;
        objKData.vol     = 0;
        objKData.dayCnt  = 0;
        objKData.date    = objCal.GetDate();
        nDate = objCal.SubDay(7);
        for (; (ptrIndex->m_vecDayData.size() > nDayIdx) && (ptrIndex->m_vecDayData[nDayIdx].date > nDate); nDayIdx++){
            if (0 == objKData.dayCnt){
                //objKData.date    = nDate;
                objKData.open    = ptrIndex->m_vecDayData[nDayIdx].open;
                objKData.high    = ptrIndex->m_vecDayData[nDayIdx].high;
                objKData.low     = ptrIndex->m_vecDayData[nDayIdx].low;
                objKData.close   = ptrIndex->m_vecDayData[nDayIdx].close;
                objKData.amount  = ptrIndex->m_vecDayData[nDayIdx].amount/100000;
                objKData.vol     = ptrIndex->m_vecDayData[nDayIdx].vol/10000;
                objKData.dayCnt  = 1;
                //DEBUG("before i: "<<i<<", nDayIdx: "<<nDayIdx<<", nDate: "<<nDate);
            }
            else{
                objKData.high    = objKData.high > ptrIndex->m_vecDayData[nDayIdx].high 
                                   ? objKData.high : ptrIndex->m_vecDayData[nDayIdx].high;
                objKData.low     = objKData.low < ptrIndex->m_vecDayData[nDayIdx].low
                                   ? objKData.low : ptrIndex->m_vecDayData[nDayIdx].low;
                objKData.open   = ptrIndex->m_vecDayData[nDayIdx].open;
                objKData.amount  += ptrIndex->m_vecDayData[nDayIdx].amount/100000;
                objKData.vol     += ptrIndex->m_vecDayData[nDayIdx].vol/10000;
                objKData.dayCnt  += 1;
            }
        }
        if (0 < objKData.dayCnt){
            vecKData.push_back(objKData);
        }
    }

    return 0;
}

int CIndex::GetMonthKLine(
    CIndexData *ptrIndex,
    int nIndex,
    int nBeforeCnt,
    int nAfterCnt,
    vector<CKData> &vecKData,
    string &strRetCode
    )
{
    int nMonth      = 0;
    int nDayIdx = 0;
    CKData objKData;
    
    nMonth = ptrIndex->m_vecDayData[nIndex].date/100;
    while ((0 <= nIndex) && (ptrIndex->m_vecDayData[nIndex].date/100 == nMonth))
    {
        nIndex--;
    }

    nDayIdx = nIndex;
    vecKData.clear();
    for (int i=0; i<=nAfterCnt; i++){
        if (0 > nDayIdx){
            break;
        }
        nMonth = ptrIndex->m_vecDayData[nDayIdx].date/100;
        objKData.date    = ptrIndex->m_vecDayData[nDayIdx].date;
        objKData.open    = ptrIndex->m_vecDayData[nDayIdx].open;
        objKData.high    = ptrIndex->m_vecDayData[nDayIdx].high;
        objKData.low     = ptrIndex->m_vecDayData[nDayIdx].low;
        objKData.close   = ptrIndex->m_vecDayData[nDayIdx].close;
        objKData.amount  = ptrIndex->m_vecDayData[nDayIdx].amount/100000;
        objKData.vol     = ptrIndex->m_vecDayData[nDayIdx].vol/10000;
        objKData.dayCnt  = 1;
        nDayIdx--;
        for (; (0 <= nDayIdx) && (nMonth == ptrIndex->m_vecDayData[nDayIdx].date/100); nDayIdx--){
            objKData.date    = ptrIndex->m_vecDayData[nDayIdx].date;
            objKData.high    = objKData.high > ptrIndex->m_vecDayData[nDayIdx].high
                               ? objKData.high : ptrIndex->m_vecDayData[nDayIdx].high;
            objKData.low     = objKData.low < ptrIndex->m_vecDayData[nDayIdx].low
                               ? objKData.low : ptrIndex->m_vecDayData[nDayIdx].low;
            objKData.close   = ptrIndex->m_vecDayData[nDayIdx].close;
            objKData.amount  += ptrIndex->m_vecDayData[nDayIdx].amount/100000;
            objKData.vol     += ptrIndex->m_vecDayData[nDayIdx].vol/10000;
            objKData.dayCnt++;
        }
        vecKData.push_back(objKData);
    }
    reverse(vecKData.begin(), vecKData.end());

    nDayIdx = nIndex + 1;
    for (int i=0; i<=nBeforeCnt; i++){
        if (ptrIndex->m_vecDayData.size() <= nDayIdx){
            break;
        }
        nMonth = ptrIndex->m_vecDayData[nDayIdx].date/100;
        objKData.date    = ptrIndex->m_vecDayData[nDayIdx].date;
        objKData.open    = ptrIndex->m_vecDayData[nDayIdx].open;
        objKData.high    = ptrIndex->m_vecDayData[nDayIdx].high;
        objKData.low     = ptrIndex->m_vecDayData[nDayIdx].low;
        objKData.close   = ptrIndex->m_vecDayData[nDayIdx].close;
        objKData.amount  = ptrIndex->m_vecDayData[nDayIdx].amount/100000;
        objKData.vol     = ptrIndex->m_vecDayData[nDayIdx].vol/10000;
        objKData.dayCnt  = 1;
        nDayIdx++;
        for (; (ptrIndex->m_vecDayData.size() > nDayIdx)&&(nMonth == ptrIndex->m_vecDayData[nDayIdx].date/100); nDayIdx++){
            objKData.open   = ptrIndex->m_vecDayData[nDayIdx].open;
            objKData.high    = objKData.high > ptrIndex->m_vecDayData[nDayIdx].high
                               ? objKData.high : ptrIndex->m_vecDayData[nDayIdx].high;
            objKData.low     = objKData.low < ptrIndex->m_vecDayData[nDayIdx].low
                               ? objKData.low : ptrIndex->m_vecDayData[nDayIdx].low;
            objKData.amount  += ptrIndex->m_vecDayData[nDayIdx].amount/100000;
            objKData.vol     += ptrIndex->m_vecDayData[nDayIdx].vol/10000;
            objKData.dayCnt++;
        }
        vecKData.push_back(objKData);
    }

    return 0;
}


