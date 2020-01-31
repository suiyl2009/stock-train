#include <algorithm>

#include "CondList.h"
#include "Constant.h"
#include "Log.h"
#include "Data.h"
#include "OutInfo.h"

CCondValue::CCondValue()
{
	m_ptrStock = NULL;
	m_bReport = false;
}

int CCondValue::Init(
	StockData *ptrStock
	)
{
	m_ptrStock = ptrStock;
	
	return 0;
}

int CCondValue::GetItem(
	int idx,
	int &iValue
	)
{
	return 1;
}

int CCondValue::GetItem(
    int idx1,
    int idx2,
    int &iValue
    )
{
    return 1;
}

CROECond::CROECond()
{
    m_ptrStock = NULL;
	m_bReport  = true;
}

int CROECond::GetItem(
	int idx,
	int &iValue
	)
{
	if ((0>idx) || (idx >= m_ptrStock->m_vecQuart.size()))
	{
		return 1;
	}

	iValue = m_ptrStock->m_vecQuart[idx].m_rQuart[3].m_iNetAssertRate;
	return 0;
}

int CROECond::GetItem(
    int idx1,
    int idx2,
    int &iValue
    )
{
	if ((0>idx1) || (idx1 >= m_ptrStock->m_vecQuart.size()) || (0 > idx2) || (3 < idx2))
	{
		return 1;
	}

    if (true == m_ptrStock->m_vecQuart[idx1].m_rQuart[idx2].m_strCode.empty())
    {
        return 2;
    }

    iValue = m_ptrStock->m_vecQuart[idx1].m_rQuart[idx2].m_iNetAssertRate;

    return 0;
}


CPrCond::CPrCond()
{
    m_ptrStock = NULL;
	m_bReport  = false;
}

int CPrCond::GetItem(
	int idx,
	int &iValue
	)
{
	if ((0>idx) || (idx >= m_ptrStock->m_vecDayData.size()))
	{
		return 1;
	}

	CDayPInfo objDayPInfo;
	if (0 != m_ptrStock->GetPValue(idx, objDayPInfo))
	{
		return 1;
	}
	iValue = objDayPInfo.pr;
	return 0;
}

//////////////////////////////////////////////////////////////////////
CCondList::CCondList()
{
    m_ptrCondValue = NULL;
	m_vecResult.clear();
	m_vecStock.clear();
	m_vecIdx.clear();
}

CCondList::~CCondList()
{
    if (NULL != m_ptrCondValue)
	{
	    delete m_ptrCondValue;
	    m_ptrCondValue = NULL;
	}
}

/*
字段名称	值类型	备注
method	字符串cond.filter	
code	数组	代码列表[code1, code2, code3, …]
item	字符串	需要比较的字段
date	整型或空	结束日期，例如20170121
traverse	整数	遍历数量
match	整数	匹配数量
value	整数	基准值，traverse有match个值大于value
sort	字符串	match个值的平均做asc或者desc

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
result	数组[{},{},{}…]	响应字段。数组元素{“cde”: xxx, ”name”: xxx, ”avg”: xxx, “match”:xxx, “rank”:xxx}
*/
int CCondList::Filter(
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
    int iRet           = 0;
    size_t sJsonSize   = 0;
	int iDate          = 0;
	string strItem     = "";
	int iTraverse      = 0;
	int iMatch         = 0;
	int iValue         = 0;
	bool bGreater      = false;
	string strSort     = "asc";
	vector<string> vecCode;
	vecCode.clear();
	
	map<string, StockData*>::iterator iter;
	StockData *ptrStock = NULL;
	json_t *jItem = NULL;

    // 解析json，得到各字段item,date
    jItem = json_object_get(jReq, "date");
    if (jItem && json_is_integer(jItem)) 
	{
		iDate = json_integer_value(json_object_get(jReq, "date"));
		if (((0 != iDate) && (iDate < 19800000)) || (iDate > 99999999))
		{
	        strRetCode = CErrJsonType;
			ERROR("Filter Get json date value error. strRetCode: "<<strRetCode<<", iDate: "<<iDate);
			return 1;
		}
    }

	jItem = json_object_get(jReq, "item");
    if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("Filter Get json item value error. strRetCode: "<<strRetCode);
		return 1;
    }
	strItem = json_string_value(jItem);

	jItem = json_object_get(jReq, "code");
    if (!jItem || !json_is_array(jItem)) {
        strRetCode = CErrJsonType;
		ERROR("Filter Get json item error. strRetCode: "<<strRetCode);
		return 1;
    }
	sJsonSize = json_array_size(jItem);
	if (0 == sJsonSize)
	{
        strRetCode = CErrJsonType;
		ERROR("Filter Get json item error. strRetCode: "<<strRetCode);
		return 1;
	}
	for (int i=0; i<sJsonSize; i++)
	{
	    vecCode.push_back(json_string_value(json_array_get(jItem, i)));
	}

	jItem = json_object_get(jReq, "traverse");
	if (!jItem || !json_is_integer(jItem)) 
	{
        strRetCode = CErrJsonType;
		ERROR("Filter Get json traverse error. strRetCode: "<<strRetCode);
		return 1;
	}
	iTraverse = json_integer_value(jItem);

	jItem = json_object_get(jReq, "match");
	if (!jItem || !json_is_integer(jItem)) 
	{
        strRetCode = CErrJsonType;
		ERROR("Filter Get json match error. strRetCode: "<<strRetCode);
		return 1;
	}
	iMatch = json_integer_value(jItem);

	jItem = json_object_get(jReq, "value");
	if (!jItem || !json_is_integer(jItem)) 
	{
        strRetCode = CErrJsonType;
		ERROR("Filter Get json match error. strRetCode: "<<strRetCode);
		return 1;
	}
	iValue = json_integer_value(jItem);

	jItem = json_object_get(jReq, "greater");
	if (jItem && json_is_integer(jItem)) 
	{
		bGreater = json_integer_value(json_object_get(jReq, "greater"));
	}

    jItem = json_object_get(jReq, "sort");
    if (jItem && json_is_string(jItem)) 
	{
	    strSort = json_string_value(jItem);
    }

    iRet = Init(vecCode, iDate, strRetCode);
	if (0 != iRet)
	{
	    ERROR("Call Init error. strRetCode: "<<strRetCode);
		return iRet;
	}

    if (1 != COutInfo::Instance()->m_mapCond.count(strItem))
	{
	    strRetCode = CErrMethodNone;
		ERROR("Call 1 != COutInfo::Instance()->m_mapCond.count. strItem: "<<strItem<<", strRetCode: "<<strRetCode);
		return 1;
	}
	m_ptrCondValue = (COutInfo::Instance()->m_mapCond[strItem])();

	if (true == m_ptrCondValue->m_bReport)
	{
		iRet = FilterReport(iTraverse, iMatch, bGreater, iValue, strRetCode);
		if (0 != iRet)
		{
			ERROR("Call FilterReport error. strRetCode: "<<strRetCode);
			return iRet;
		}
	}
	else
	{
		iRet = FilterPrice(iTraverse, iMatch, bGreater, iValue, strRetCode);
		if (0 != iRet)
		{
			ERROR("Call FilterPrice error. strRetCode: "<<strRetCode);
			return iRet;
		}
	}

    if (0 < m_vecResult.size())
    {
    	QSort(m_vecResult, 0, m_vecResult.size()-1);
    }

    if ("asc" != strSort)
    {
		reverse(m_vecResult.begin(), m_vecResult.end());
    }
	
	iRet = ShowFilterResult(jRes, strRetCode);
	if (0 != iRet)
	{
	    ERROR("Call ShowResult error. strRetCode: "<<strRetCode);
		return iRet;
	}

	return iRet;
}

/*
请求字段列表
字段名称	值类型	备注
method	字符串cond.compare	
code	数组	代码列表[code1, code2, code3, …]
item	字符串	需要返回的曲线
date	整型或空	结束日期，例如20170121
value	整数	基准值，item的值大于等于value
greater	整型	0或非0
sort	字符串	Value值做asc或者desc

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
result	数组[{},{},{}…]	响应字段值。数组元素为{“cde”: xxx, ”name”: xxx, “value”: xxx, “date”: xxx, “rank”: xxx}
*/
int CCondList::Compare(
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
	int iRet		   = 0;
	size_t sJsonSize   = 0;
	int iDate		   = 0;
	string strItem	   = "";
	int iValue		   = 0;
	bool bGreater      = false;
	string strSort	   = "asc";
	vector<string> vecCode;
	vecCode.clear();
	
	map<string, StockData*>::iterator iter;
	StockData *ptrStock = NULL;
	json_t *jItem = NULL;
	
	// 解析json，得到各字段item,date
	jItem = json_object_get(jReq, "date");
	if (jItem && json_is_integer(jItem)) 
	{
		iDate = json_integer_value(json_object_get(jReq, "date"));
		if (((0 != iDate) && (iDate < 19800000)) || (iDate > 99999999))
		{
			strRetCode = CErrJsonType;
			ERROR("Filter Get json date value error. strRetCode: "<<strRetCode<<", iDate: "<<iDate);
			return 1;
		}
	}
	
	jItem = json_object_get(jReq, "item");
	if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("Filter Get json item value error. strRetCode: "<<strRetCode);
		return 1;
	}
	strItem = json_string_value(jItem);
	
	jItem = json_object_get(jReq, "code");
	if (!jItem || !json_is_array(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("Filter Get json item error. strRetCode: "<<strRetCode);
		return 1;
	}
	sJsonSize = json_array_size(jItem);
	if (0 == sJsonSize)
	{
		strRetCode = CErrJsonType;
		ERROR("Filter Get json item error. strRetCode: "<<strRetCode);
		return 1;
	}
	for (int i=0; i<sJsonSize; i++)
	{
		vecCode.push_back(json_string_value(json_array_get(jItem, i)));
	}
	
	jItem = json_object_get(jReq, "value");
	if (!jItem || !json_is_integer(jItem)) 
	{
		strRetCode = CErrJsonType;
		ERROR("Filter Get json match error. strRetCode: "<<strRetCode);
		return 1;
	}
	iValue = json_integer_value(jItem);

	jItem = json_object_get(jReq, "greater");
	if (jItem && json_is_integer(jItem)) 
	{
		bGreater = json_integer_value(json_object_get(jReq, "greater"));
	}
	
	jItem = json_object_get(jReq, "sort");
	if (jItem && json_is_string(jItem)) 
	{
		strSort = json_string_value(jItem);
	}
	
	iRet = Init(vecCode, iDate, strRetCode);
	if (0 != iRet)
	{
		ERROR("Call Init error. strRetCode: "<<strRetCode);
		return iRet;
	}
	
	if (1 != COutInfo::Instance()->m_mapCond.count(strItem))
	{
		strRetCode = CErrMethodNone;
		ERROR("Call 1 != COutInfo::Instance()->m_mapCond.count. strItem: "<<strItem<<", strRetCode: "<<strRetCode);
		return 1;
	}
	m_ptrCondValue = (COutInfo::Instance()->m_mapCond[strItem])();
	
	if (true == m_ptrCondValue->m_bReport)
	{
		iRet = CompareReport(bGreater, iValue, strRetCode);
		if (0 != iRet)
		{
			ERROR("Call GreaterReport error. strRetCode: "<<strRetCode);
			return iRet;
		}
	}
	else
	{
		iRet = ComparePrice(bGreater, iValue, strRetCode);
		if (0 != iRet)
		{
			ERROR("Call FilterPrice error. strRetCode: "<<strRetCode);
			return iRet;
		}
	}
	
	if (0 < m_vecResult.size())
	{
		QSort(m_vecResult, 0, m_vecResult.size()-1);
	}
	
	if ("asc" != strSort)
	{
		reverse(m_vecResult.begin(), m_vecResult.end());
	}
	
	iRet = ShowCompareResult(jRes, strRetCode);
	if (0 != iRet)
	{
		ERROR("Call ShowResult error. strRetCode: "<<strRetCode);
		return iRet;
	}
	
	return iRet;

}

int CCondList::SubLeader( // roe, profit top 10; 1/3roe,1/3profit,1/3revenue
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int nRet = 0;
    int nDate = 0;
    map<string, vector<CListIdxStock> > mapSubStock;
    CListIdxStock objListIdxStock;
    vector<CListIdxStock> vecListIdxStock;
    vector<string> vecCode;
    set<string> setSub;
	StockData *ptrStock = NULL;
	json_t *jItem = NULL;

    setSub.clear();
	jItem = json_object_get(jReq, "sub");
	if (jItem && json_is_array(jItem)) {
        size_t sJsonSize = json_array_size(jItem);
        for (int i=0; i<sJsonSize; i++)
        {
            setSub.insert(json_string_value(json_array_get(jItem, i)));
        }
	}

    // 解析json，得到各字段item,date
    jItem = json_object_get(jReq, "date");
    if (!jItem || !json_is_integer(jItem)) 
	{
		strRetCode = CErrJsonType;
		ERROR("IndustryLeader Get json item value error. strRetCode: "<<strRetCode);
		return 1;
    }
	nDate = json_integer_value(json_object_get(jReq, "date"));
	if ((nDate < 19800000) || (nDate > 99999999))
	{
        strRetCode = CErrJsonType;
		ERROR("Filter Get json date value error. strRetCode: "<<strRetCode<<", nDate: "<<nDate);
		return 1;
	}

    // 查找对应行业元素集合
    for (map<string, StockData*>::iterator iter = CStockMap::Instance()->m_mapStock.begin(); 
         iter != CStockMap::Instance()->m_mapStock.end(); iter++)
    {
        if ((0 < setSub.size()) && (0 == setSub.count(iter->second->m_objInfo.m_strSubIndustry)))
        {
            continue;
        }
        nRet = PushItem(iter->second, nDate, false, strRetCode);
        if (0 != nRet)
        {
            ERROR("Call PushItem error. iter->first: "<<iter->first<<", nDate: "<<nDate
                <<", strRetCode: "<<strRetCode);
            return nRet;
        }
    }
    DEBUG("m_vecStock.size(): "<<m_vecStock.size()<<", nDate: "<<nDate);

    //  构造合适数据格式
    vecListIdxStock.clear();
    mapSubStock.clear();
    nDate = GetPreDate(nDate);
    for (int i=0; i<m_vecStock.size(); i++)
    {
        //DEBUG("nDate: "<<nDate<<", ReportDate: "<<m_vecStock[i]->m_vecQuart[m_vecIdx[i].m_iQuartIdx1].m_rQuart[m_vecIdx[i].m_iQuartIdx2].m_iDate);
        if (nDate != m_vecStock[i]->m_vecQuart[m_vecIdx[i].m_iQuartIdx1].m_rQuart[m_vecIdx[i].m_iQuartIdx2].m_iDate)
        {
            continue;  // 
        }
        
        if (0 == mapSubStock.count(m_vecStock[i]->m_objInfo.m_strSubIndustry))
        {
            mapSubStock[m_vecStock[i]->m_objInfo.m_strSubIndustry] = vecListIdxStock;
        }
        objListIdxStock.m_pStock = m_vecStock[i];
        objListIdxStock.m_objIdx = m_vecIdx[i];
        mapSubStock[m_vecStock[i]->m_objInfo.m_strSubIndustry].push_back(objListIdxStock);
    }
    m_vecStock.clear();
    m_vecIdx.clear();
    DEBUG("mapSubStock.size(): "<<mapSubStock.size());

    // 选择各行业龙头企业
    vecCode.clear();
    for (map<string, vector<CListIdxStock> >::iterator iter = mapSubStock.begin(); iter != mapSubStock.end(); iter++)
    {
        nRet = SelectLeader(iter->second, vecCode, strRetCode);
        if (0 != nRet)
        {
            ERROR("Call SelectLeader error. subindustry: "<<iter->first<<", nDate: "<<nDate<<", strRetCode: "<<strRetCode);
            return nRet;
        }
    }
    DEBUG("vecCode.size(): "<<vecCode.size());

    // 生成元素列表
    nRet = ShowCodeListResult(jRes, vecCode, strRetCode);
    if (0 != nRet)
    {
        ERROR("Call ShowCodeListResult error. strRetCode: "<<strRetCode);
        return nRet;
    }

    return nRet;
}

int CCondList::Init(
	vector<string> vecCode,
	int iBeginDate,
	string &strRetCode
	)
{
    m_vecStock.clear();
	m_vecIdx.clear();
	
    map<string, StockData*>::iterator iter;
	for (int i=0; i<vecCode.size(); i++)
	{
		iter = CStockMap::Instance()->m_mapStock.find(vecCode[i]);
		if (iter == CStockMap::Instance()->m_mapStock.end())
		{
			continue;
		}
		//m_vecStock.push_back(iter->second);
		PushItem(iter->second, iBeginDate, false, strRetCode);
	}

	return 0;
}

int CCondList::FilterReport(
	int iTraverseCnt,
	int iMatchCnt,
	bool bGreater,
	int iValue,
	string &strRetCode
	)
{
	int idx = 0;
	int iStockCnt = 0;
	int iStockValue = 0;
	double dAvgValue = 0;
    CCondResult objResult;

	m_vecResult.clear();	
	for (int i=0; i<m_vecStock.size(); i++)
	{
		iStockCnt = 0;
		idx = m_vecIdx[i].m_iQuartIdx1;
		if (3 != m_vecIdx[i].m_iQuartIdx2)
		{
			idx++;
		}
	
		if (0 != m_ptrCondValue->Init(m_vecStock[i]))
		{
		    strRetCode = CErrOutOfRange;
			ERROR("ptrCondValue->Init(m_vecStock[i]) error. m_vecStock[i]: "<<m_vecStock[i]);
			return 1;
		}
		dAvgValue = 0;
		for (int j=0; j<iTraverseCnt; j++)
		{
			if (0 != m_ptrCondValue->GetItem(j+idx, iStockValue))
			{
				continue;
			}
			if ((iStockValue >= iValue) && (true == bGreater)) // 大于等于的情况
			{
				iStockCnt++;
				dAvgValue = iStockValue*1.0/iStockCnt + (iStockCnt-1)*1.0/iStockCnt*dAvgValue;
				DEBUG("greater iStockValue: "<<iStockValue<<", iValue: "<<iValue<<", bGreater: "<<bGreater<<", iStockCnt: "<<iStockCnt);
			}
			if ((iStockValue <= iValue) && (false == bGreater)) // 小于等于的情况
			{
				iStockCnt++;
				dAvgValue = iStockValue*1.0/iStockCnt + (iStockCnt-1)*1.0/iStockCnt*dAvgValue;
				DEBUG("less iStockValue: "<<iStockValue<<", iValue: "<<iValue<<", bGreater: "<<bGreater<<", iStockCnt: "<<iStockCnt);
			}
		}
		
		if (iStockCnt >= iMatchCnt)
		{
		    objResult.m_iDate    = m_vecStock[i]->m_vecQuart[idx].m_rQuart[3].m_iDate;
			objResult.m_iMatch   = iStockCnt;
			objResult.m_iRank    = 0;
			objResult.m_iValue   = (int)(dAvgValue);
			objResult.m_strCode  = m_vecStock[i]->m_strCode;
			objResult.m_strName  = m_vecStock[i]->m_strName;
			m_vecResult.push_back(objResult);
		}
	}
	
	return 0;
}


int CCondList::FilterPrice(
	int iTraverseCnt,
	int iMatchCnt,
	bool bGreater,
	int iValue,
	string &strRetCode
	)
{
	int idx = 0;
	int iStockCnt = 0;
	int iStockValue = 0;
	double dAvgValue = 0;
	CCondResult objResult;
	
	m_vecResult.clear();	
	for (int i=0; i<m_vecStock.size(); i++)
	{
		iStockCnt = 0;
		idx = m_vecIdx[i].m_iDayIdx;
	
		if (0 != m_ptrCondValue->Init(m_vecStock[i]))
		{
		    strRetCode = CErrOutOfRange;
			ERROR("ptrCondValue->Init(m_vecStock[i]) error. m_vecStock[i]: "<<m_vecStock[i]);
			return 1;
		}
		dAvgValue = 0;
		for (int j=0; j<iTraverseCnt; j++)
		{
			if (0 != m_ptrCondValue->GetItem(j+idx, iStockValue))
			{
				continue;
			}
			if ((iStockValue >= iValue) && (true == bGreater)) // 大于等于的情况
			{
				iStockCnt++;
				dAvgValue = iStockValue*1.0/iStockCnt + (iStockCnt-1)*1.0/iStockCnt*dAvgValue;
			}
			if ((iStockValue <= iValue) && (false == bGreater)) // 小于等于的情况
			{
				iStockCnt++;
				dAvgValue = iStockValue*1.0/iStockCnt + (iStockCnt-1)*1.0/iStockCnt*dAvgValue;
			}
		}
		
		if (iStockCnt >= iMatchCnt)
		{
			objResult.m_iDate    = m_vecStock[i]->m_vecDayData[idx].date;
			objResult.m_iMatch   = iStockCnt;
			objResult.m_iRank    = 0;
			objResult.m_iValue   = (int)(dAvgValue);
			objResult.m_strCode  = m_vecStock[i]->m_strCode;
			objResult.m_strName  = m_vecStock[i]->m_strName;
			m_vecResult.push_back(objResult);
		}
	}
	
	return 0;
}

int CCondList::CompareReport(
    bool bGreater,
	int iValue,
	string &strRetCode
	)
{
	int idx = 0;
	int iStockCnt = 0;
	int iStockValue = 0;
	CCondResult objResult;
	
	m_vecResult.clear();	
	for (int i=0; i<m_vecStock.size(); i++)
	{
		iStockCnt = 0;
		idx = m_vecIdx[i].m_iQuartIdx1;
		if (3 != m_vecIdx[i].m_iQuartIdx2)
		{
			idx++;
		}
	
		if (0 != m_ptrCondValue->Init(m_vecStock[i]))
		{
		    strRetCode = CErrOutOfRange;
			ERROR("ptrCondValue->Init(m_vecStock[i]) error. m_vecStock[i]: "<<m_vecStock[i]);
			return 1;
		}

		if (0 != m_ptrCondValue->GetItem(idx, iStockValue))
		{
			continue;
		}
		if ((iStockValue >= iValue) && (true == bGreater)) // 大于等于的情况
		{
		    objResult.m_iDate    = m_vecStock[i]->m_vecQuart[idx].m_rQuart[3].m_iDate;
			objResult.m_iMatch   = 0;
			objResult.m_iRank    = 0;
			objResult.m_iValue   = iStockValue;
			objResult.m_strCode  = m_vecStock[i]->m_strCode;
			objResult.m_strName  = m_vecStock[i]->m_strName;
			m_vecResult.push_back(objResult);

		}
		if ((iStockValue <= iValue) && (false == bGreater)) // 小于等于的情况
		{
		    objResult.m_iDate    = m_vecStock[i]->m_vecQuart[idx].m_rQuart[3].m_iDate;
			objResult.m_iMatch   = 0;
			objResult.m_iRank    = 0;
			objResult.m_iValue   = iStockValue;
			objResult.m_strCode  = m_vecStock[i]->m_strCode;
			objResult.m_strName  = m_vecStock[i]->m_strName;
			m_vecResult.push_back(objResult);

		}
	}
	
	return 0;
}

int CCondList::ComparePrice(
    bool bGreater,
	int iValue,
	string &strRetCode
	)
{
	int idx = 0;
	int iStockCnt = 0;
	int iStockValue = 0;
	CCondResult objResult;

	m_vecResult.clear();
	for (int i=0; i<m_vecStock.size(); i++)
	{
		idx = m_vecIdx[i].m_iDayIdx;
		if (0 > idx)
		{
			continue;
		}
	
		if (0 != m_ptrCondValue->Init(m_vecStock[i]))
		{
		    strRetCode = CErrOutOfRange;
			ERROR("ptrCondValue->Init(m_objCurrList.m_vecStock[i]) error. m_vecStock[i]: "<<m_vecStock[i]);
			return 1;
		}
	
		if (0 != m_ptrCondValue->GetItem(idx, iStockValue))
		{
			continue;
		}
	
		if ((iStockValue >= iValue) && (true == bGreater))  // 大于等于的情况
		{
		    objResult.m_iDate    = m_vecStock[i]->m_vecDayData[idx].date;
			objResult.m_iMatch   = 0;
			objResult.m_iRank    = 0;
			objResult.m_iValue   = iStockValue;
			objResult.m_strCode  = m_vecStock[i]->m_strCode;
			objResult.m_strName  = m_vecStock[i]->m_strName;
			m_vecResult.push_back(objResult);
		}

		if ((iStockValue <= iValue) && (false == bGreater))  // 小于等于的情况
		{
		    objResult.m_iDate    = m_vecStock[i]->m_vecDayData[idx].date;
			objResult.m_iMatch   = 0;
			objResult.m_iRank    = 0;
			objResult.m_iValue   = iStockValue;
			objResult.m_strCode  = m_vecStock[i]->m_strCode;
			objResult.m_strName  = m_vecStock[i]->m_strName;
			m_vecResult.push_back(objResult);
		}
	}
	
	return 0;

}

int CCondList::SelectLeader(
    vector<CListIdxStock> &vecListIdxStock, 
    vector<string> &vecCode,
    string &strRetCode
    )
{
    map<string, int> mapRankCnt;
    SortIntItem objSortItem;
    vector<SortIntItem> vecSort;
    map<int, vector<string> > mapRankRes;
    int nIdx1 = 0;
    int nIdx2 = 0;
    string strCode = "";
    
    for (int i=0; i < vecListIdxStock.size(); i++) // 初始值都设为300，排名靠前再减
    {
        mapRankCnt[vecListIdxStock[i].m_pStock->m_strCode] = 300;
    }

    vecSort.clear();
    for (int i=0; i < vecListIdxStock.size(); i++)  // ROE
    {
        nIdx1 = vecListIdxStock[i].m_objIdx.m_iQuartIdx1;
        nIdx2 = vecListIdxStock[i].m_objIdx.m_iQuartIdx2;
        objSortItem.m_idx = i;
        objSortItem.m_iKey = vecListIdxStock[i].m_pStock->m_vecQuart[nIdx1].m_rQuart[nIdx2].m_iNetAssertRate;
        vecSort.push_back(objSortItem);
        DEBUG("strCode: "<<vecListIdxStock[i].m_pStock->m_strCode<<", vecSort key: "<<vecSort[i].m_iKey
            <<", nIdx1: "<<nIdx1<<", nIdx2: "<<nIdx2<<", year: "<<vecListIdxStock[i].m_pStock->m_vecQuart[nIdx1].m_iYear);
    }
    HeapSortAsc(vecSort, vecSort.size());
    for (int i=vecSort.size()-1; i>=0; i--)
    {
        strCode = vecListIdxStock[vecSort[i].m_idx].m_pStock->m_strCode;
        mapRankCnt[strCode] = mapRankCnt[strCode] - 100 + vecSort.size() - i;
        DEBUG("strCode: "<<strCode<<", vecSort key: "<<vecSort[i].m_iKey);
    }
    
    vecSort.clear();
    for (int i=0; i < vecListIdxStock.size(); i++)  // Profit
    {
        nIdx1 = vecListIdxStock[i].m_objIdx.m_iQuartIdx1;
        nIdx2 = vecListIdxStock[i].m_objIdx.m_iQuartIdx2;
        objSortItem.m_idx = i;
        objSortItem.m_iKey = vecListIdxStock[i].m_pStock->m_vecQuart[nIdx1].m_rQuart[nIdx2].m_iNetProfit;
        vecSort.push_back(objSortItem);
    }
    //HeapSortAsc(vecSort, m_scnSubLeaderTop);
    //for (int i=vecSort.size()-1; (i>=0) && (vecSort.size() - i <= m_scnSubLeaderTop); i--)
    HeapSortAsc(vecSort, vecSort.size());
    for (int i=vecSort.size()-1; i>=0; i--)
    {
        strCode = vecListIdxStock[vecSort[i].m_idx].m_pStock->m_strCode;
        mapRankCnt[strCode] = mapRankCnt[strCode] - 100 + vecSort.size() - i;
    }

    vecSort.clear();
    for (int i=0; i < vecListIdxStock.size(); i++)  // Revenue
    {
        nIdx1 = vecListIdxStock[i].m_objIdx.m_iQuartIdx1;
        nIdx2 = vecListIdxStock[i].m_objIdx.m_iQuartIdx2;
        objSortItem.m_idx = i;
        objSortItem.m_iKey = vecListIdxStock[i].m_pStock->m_vecQuart[nIdx1].m_rQuart[nIdx2].m_iRevenue;
        vecSort.push_back(objSortItem);
    }
    HeapSortAsc(vecSort, vecSort.size());
    for (int i=vecSort.size()-1; i>=0; i--)
    {
        strCode = vecListIdxStock[vecSort[i].m_idx].m_pStock->m_strCode;
        mapRankCnt[strCode] = mapRankCnt[strCode] - 100 + vecSort.size() - i;
    }

    for (map<string, int>::iterator iterCnt = mapRankCnt.begin(); iterCnt != mapRankCnt.end(); iterCnt++)
    {
        if (300 == iterCnt->second)
        {
            continue;
        }
        if (0 == mapRankRes.count(iterCnt->second))
        {
            vector<string> vecTempCode;
            mapRankRes[iterCnt->second] = vecTempCode;
        }
        mapRankRes[iterCnt->second].push_back(iterCnt->first);
    }
    DEBUG("mapRankRes.size(): "<<mapRankRes.size());

    int nSubNum = 0;
    for (map<int, vector<string> >::iterator iterRes = mapRankRes.begin(); iterRes != mapRankRes.end(); iterRes++)
    {
        for (int i=0; i<iterRes->second.size(); i++)
        {
            vecCode.push_back(iterRes->second[i]);
            nSubNum++;
        }
        if (m_scnSubLeaderNum <= nSubNum)
        {
            break;
        }
        if ((m_scnSubLeaderNum <= nSubNum + 1) && (vecListIdxStock.size() < 10))
        {
            break;
        }
    }
    
    return 0;
}

int CCondList::ShowFilterResult(
	json_t *&jRes,
	string &strRetCode
	)
{
	json_t *jResult = json_array();
	DEBUG("m_vecResult.size(): "<<m_vecResult.size());
	for (int i=0; i<m_vecResult.size(); i++)
	{
		json_t *jItem = json_object();
		json_object_set_new(jItem, "code", json_string(m_vecResult[i].m_strCode.c_str()));
		json_object_set_new(jItem, "name", json_string(m_vecResult[i].m_strName.c_str()));
		json_object_set_new(jItem, "avg", json_integer(m_vecResult[i].m_iValue));
		json_object_set_new(jItem, "match", json_integer(m_vecResult[i].m_iMatch));
		json_object_set_new(jItem, "date", json_integer(m_vecResult[i].m_iDate));
		json_object_set_new(jItem, "rank", json_integer(i+1));
		json_array_append_new(jResult, jItem);
	}
	
	json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	json_object_set_new(jRes, "result", jResult);
	
	return 0;
}

int CCondList::ShowCompareResult(
	json_t *&jRes,
	string &strRetCode
	)
{
	json_t *jResult = json_array();
	DEBUG("m_vecResult.size(): "<<m_vecResult.size());
	for (int i=0; i<m_vecResult.size(); i++)
	{
		json_t *jItem = json_object();
		json_object_set_new(jItem, "code", json_string(m_vecResult[i].m_strCode.c_str()));
		json_object_set_new(jItem, "name", json_string(m_vecResult[i].m_strName.c_str()));
		json_object_set_new(jItem, "value", json_integer(m_vecResult[i].m_iValue));
		json_object_set_new(jItem, "date", json_integer(m_vecResult[i].m_iDate));
		json_object_set_new(jItem, "rank", json_integer(i+1));
		json_array_append_new(jResult, jItem);
	}
	
	json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	json_object_set_new(jRes, "result", jResult);
	
	return 0;
}

int CCondList::ShowCodeListResult(
    json_t *&jRes,
    const vector<string> &vecCode,
    string &strRetCode
    )
{
	json_t *jResult = json_array();
	DEBUG("vecCode.size(): "<<vecCode.size());
	for (int i=0; i<vecCode.size(); i++)
	{
		json_array_append_new(jResult, json_string(vecCode[i].c_str()));
	}
	
	json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	json_object_set_new(jRes, "result", jResult);
	
	return 0;
}

int CCondList::PushItem(
	StockData *ptrStock,
	int iDate,
	bool bReportDate,
	string &strRetCode
	)
{
	if (NULL == ptrStock)
	{
		return 0;
	}

	DEBUG("code: "<<ptrStock->m_strCode<<", iDate: "<<iDate<<", bReportDate: "<<bReportDate);
	
	CListIdx objIdx;
	do
	{
		/////////////// begin: 没有设定截止日期////////////////////////////
		if (0 == iDate)
		{
			if (ptrStock->m_vecQuart.size() > 0)  // 得到对应的坐标
			{
				objIdx.m_iQuartIdx1 = 0;  // 年坐标
				for (int i=3; i>=0; i--)  // 季度坐标
				{
					if (!ptrStock->m_vecQuart[0].m_rQuart[i].m_strCode.empty())
					{
						objIdx.m_iQuartIdx2 = i;
						break;
					}
				}
			}
			if (ptrStock->m_vecDayData.size() > 0)
			{
				objIdx.m_iDayIdx = 0;
			}
			break;
		}
		/////////////// end: 没有设定截止日期////////////////////////////

		
		//////////////begin: 设定了截止日期,且元素最晚日期在截止日期之前////////////////
		objIdx.m_iQuartIdx1 = ptrStock->GetQuartIdx(iDate/10000);
		if (0 > objIdx.m_iQuartIdx1) // 没有找到对应财报年数据
		{
			// 日期在截止日期之前结束，判断是否加入选择
			if ((ptrStock->m_vecQuart.size() > 0) && (ptrStock->m_vecQuart[0].m_iYear < iDate/10000))
			{
				objIdx.m_iQuartIdx1 = 0;  // 年坐标
				for (int i=3; i>=0; i--)  // 季度坐标
				{
					if (!ptrStock->m_vecQuart[0].m_rQuart[i].m_strCode.empty())
					{
						objIdx.m_iQuartIdx2 = i;
						break;
					}
				}
			}
			objIdx.m_iDayIdx = ptrStock->GetDayIdx(iDate);
			if ((ptrStock->m_vecDayData.size() > 0) && (ptrStock->m_vecDayData[0].date < iDate)) // 最新日期在截止日期之前
			{
				objIdx.m_iDayIdx = 0;
			}
			break;
		}
		//////////////end: 设定了截止日期,且元素最晚日期在截止日期之前////////////////

		/////////////begin: 设定了截止日期，且元素日期包含截止日期
		if (objIdx.m_iQuartIdx1 >= ptrStock->m_vecQuart.size())
		{
			strRetCode = CErrOutOfRange;
			ERROR("out of size. strRetCode: "<<strRetCode<<", strCptrStock code: "<<ptrStock->m_strCode<<", ptrStock->m_vecQuart.size(): "
				<<ptrStock->m_vecQuart.size()<<", objIdx.m_iQuartIdx1: "<<objIdx.m_iQuartIdx1);
			return 1;
		}

		DEBUG("iDate: "<<iDate<<", objIdx.m_iQuartIdx1: "<<objIdx.m_iQuartIdx1<<", year: "<<ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_strYear
			<<", m_strDeclarationDate: "<<DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[3].m_strDeclarationDate));

		for (int i=3; i>=0; i--)
		{
			if (false == bReportDate)	// 交易日期为准
			{
				if ((!ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[i].m_strCode.empty()) 
					&& (iDate >= DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[i].m_strDeclarationDate)))
				{
					objIdx.m_iQuartIdx2 = i;
					break;
				}
			}
			else  // 财报日期为准
			{
				if ((!ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[i].m_strCode.empty()) 
					&& (iDate >= DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[i].m_strClosingDate)))
				{
					objIdx.m_iQuartIdx2 = i;
					break;
				}
			}
		}

		if (0 > objIdx.m_iQuartIdx2)  // 公告日期在上一年
		{
			if (objIdx.m_iQuartIdx1 + 1 >= ptrStock->m_vecQuart.size())
			{
				objIdx.m_iQuartIdx1 = -1;
			}
			else
			{
				objIdx.m_iQuartIdx1++;

				if (false == bReportDate)	// 交易日期为准
				{
					if (iDate > DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[3].m_strDeclarationDate))
					{
						objIdx.m_iQuartIdx2 = 3;
					}
					else if (iDate > DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[2].m_strDeclarationDate))
					{
						objIdx.m_iQuartIdx2 = 2;
					}
					else if (iDate > DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[1].m_strDeclarationDate))
					{
						objIdx.m_iQuartIdx2 = 1;
					}
					else // 此处仍然没有公告，则认为有问题
					{
						objIdx.m_iQuartIdx1 = -1;
						objIdx.m_iQuartIdx2 = -1;
					}
				}
				else  // 财报日期为准
				{
					if (iDate > DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[3].m_strClosingDate))
					{
						objIdx.m_iQuartIdx2 = 3;
					}
					else if (iDate > DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[2].m_strClosingDate))
					{
						objIdx.m_iQuartIdx2 = 2;
					}
					else if (iDate > DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[1].m_strClosingDate))
					{
						objIdx.m_iQuartIdx2 = 1;
					}
					else // 此处仍然没有公告，则认为有问题
					{
						objIdx.m_iQuartIdx1 = -1;
						objIdx.m_iQuartIdx2 = -1;
					}
				}
			}
		}
		objIdx.m_iDayIdx = ptrStock->GetDayIdx(iDate);
		if ((ptrStock->m_vecDayData.size() > 0) && (ptrStock->m_vecDayData[0].date < iDate))
		{
			objIdx.m_iDayIdx = 0;
		}
		break;
		/////////////end: 设定了截止日期，且元素日期包含截止日期
	}while (false);
	
	DEBUG("iDate: "<<iDate<<", objIdx.m_iQuartIdx1: "<<objIdx.m_iQuartIdx1<<", objIdx.m_iQuartIdx2: "<<objIdx.m_iQuartIdx1<<", objIdx.m_iDayIdx: "<<objIdx.m_iDayIdx);
	if ((objIdx.m_iDayIdx < 0) || (objIdx.m_iQuartIdx1 < 0) || (objIdx.m_iQuartIdx2 < 0))
	{
		return 0;
	}
	m_vecIdx.push_back(objIdx);
	m_vecStock.push_back(ptrStock);

	return 0;
}

//vector<StockData*> m_vecStock;
//CCondValue *m_ptrCondValue;

