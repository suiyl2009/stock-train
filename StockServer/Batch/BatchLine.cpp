#include <algorithm>
#include <set>

#include "Constant.h"
#include "Log.h"
#include "OutInfo.h"
#include "BatchLine.h"
#include "SingleLine.h"
#include <algorithm>

int CBValue::Init(CBatchLine *ptrLine, string &strRetCode)
{
    m_ptrLine = ptrLine;
	
    return 0;
}

int CBRevenue::Item(int i, int j)
{
    return m_ptrLine->m_vecBatchQuart[i].m_vecReport[j].m_iRevenue;
}

int CBProfit::Item(int i, int j)
{
    return m_ptrLine->m_vecBatchQuart[i].m_vecReport[j].m_iNetProfit;
}

int CBPrice::Item(int i, int j)
{
    return m_ptrLine->m_vecBatchDay[i].m_vecDayPInfo[j].close;
}

int CBReinPrice::Item(int i, int j)
{
    return m_ptrLine->m_vecBatchDay[i].m_vecDayPInfo[j].close;
}

CBatchLine::CBatchLine()
{
	m_vecStock.clear();
	m_vecBatchQuart.clear();
	m_vecBatchDay.clear();
	m_iDate       = 0;
	m_iBeforeCnt  = 0;
	m_iAfterCnt   = 0;

    // 显示信息
    m_strItem  = "";
	m_vecBatchYVal.clear();
	m_vecXVal.clear();
	m_iMaxYVal = 0;
	m_iMinYVal = 0;
}

CBatchLine::~CBatchLine()
{

}

/*
请求字段列表
字段名称	值类型	备注
method	字符串batch.line	
code	数组[]	股票代码
item	字符串	需要返回的曲线
date	整型或空	结束日期，例如20170121
before	整数	以date为标准向前数量，以财报为准数量
after	整数	以date为标准向后数量，以财报数据为准

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
result	数组[{},{},{}…]	响应字段值。数组元素为{“cde”: xxx, ”item1”: xxx, …}

result
字段名称	值类型	备注
max	float	最大值
min	float	最小值
yline	{{type, mval, []}, {type, mval, []}, …}	
xline	[]	
*/
int CBatchLine::GetLines(    // 得到单条或多条某一类型的曲线
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
    size_t sJsonSize = 0;
	string strCode = "";
	string strItem = "";
	int iDate = 0;
	int iBeforeCnt = 0;
	int iAfterCnt  = 0;
	vector<string> vecCode;
	StockData *ptrStock = NULL;

    // 解析json，得到各字段item,date
	json_t *jCode = json_object_get(jReq, "code");
	if (!jCode || !json_is_array(jCode)) {
		strRetCode = CErrJsonType;
		ERROR("GetListShow Get json code error. strRetCode: "<<strRetCode);
		return 1;
	}
	sJsonSize = json_array_size(jCode);
	if (0 == sJsonSize)
	{
		strRetCode = CErrJsonType;
		ERROR("GetListShow Get json code error. strRetCode: "<<strRetCode);
		return 1;
	}
	for (int i=0; i<sJsonSize; i++)
	{
		vecCode.push_back(json_string_value(json_array_get(jCode, i)));
	}

	json_t *jItem = json_object_get(jReq, "item");
    if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
	    ERROR("GetLines json code is not string. strRetCode: "<<strRetCode);
		return 1;
    }
	strItem = json_string_value(jItem);

    json_t *jDate = json_object_get(jReq, "date");
    if (jDate && json_is_integer(jDate)) 
	{
		iDate = json_integer_value(json_object_get(jReq, "date"));
		if (((0 != iDate) && (iDate < 19800000)) || (iDate > 99999999))
		{
	        strRetCode = CErrJsonType;
			ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", iDate: "<<iDate);
			return 1;
		}
    }

    json_t *jBefore = json_object_get(jReq, "before");
    if (jBefore && json_is_integer(jBefore)) 
	{
		iBeforeCnt = json_integer_value(json_object_get(jReq, "before"));
		if (iBeforeCnt < -1)
		{
	        strRetCode = CErrJsonType;
			ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", iBeforeCnt: "<<iBeforeCnt);
			return 1;
		}
    }

	json_t *jAfter = json_object_get(jReq, "after");
    if (jAfter && json_is_integer(jAfter)) 
	{
		iAfterCnt = json_integer_value(json_object_get(jReq, "after"));
		if (iAfterCnt < 0)
		{
	        strRetCode = CErrJsonType;
			ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", iAfterCnt: "<<iAfterCnt);
			return 1;
		}
    }

    // 赋值
	m_strItem = strItem;
	m_iDate = iDate;
	m_iBeforeCnt = iBeforeCnt;
	m_iAfterCnt = iAfterCnt;

	// 从stockmap中找到满足条件的元素
	if (0 != Init(vecCode, strRetCode))
	{
	    ERROR("GetLines call Init error. strRetCode: "<<strRetCode);
		return 1;
	}

    // 判断对应的曲线生成函数是否存在
	if (0 == COutInfo::Instance()->m_mapBLine.count(m_strItem))
	{
	    strRetCode = CErrItemNone;
		ERROR("GetLines line func not exist. m_strItem: "<<m_strItem<<", strRetCode: "<<strRetCode);
		return 1;
	}

	// 调用对应的曲线生成函数
	if (0 != (this->*(COutInfo::Instance()->m_mapBLine[m_strItem]))(strRetCode))
	{
	    ERROR("GetLines call create line func error. m_strItem: "<<m_strItem<<", strRetCode: "<<strRetCode);
	    return 1;
	}
	
	// 生成返回的信息
	json_object_set_new(jRes, "method", json_string("batch.line"));
	if (0 != ShowResult(jRes, strRetCode))
	{
	    ERROR("Call ShowResult error. strRetCode: "<<strRetCode);
		return 1;
	}
	
    return 0;

}

int CBatchLine::GetSub(   // 得到一个细分行业某一类型的曲线
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
	return 0;
}

int CBatchLine::GetRevenueLine(string &strRetCode)
{
    CBValue *ptrValue = new CBRevenue;
	if (0 != ptrValue->Init(this, strRetCode))
	{
	    ERROR("GetRevenueLine Init error. strRetCode: "<<strRetCode<<", m_strItem: "<<m_strItem);
		delete ptrValue;
		ptrValue = NULL;
		return 1;
	}
	
	if (0 != CreateQuartLine(ptrValue, strRetCode))
	{
	    ERROR("Call CreateQuartLine error. strRetCode: "<<strRetCode);
	    delete ptrValue;
		ptrValue = NULL;
	}
	delete ptrValue;
	ptrValue = NULL;
	
	return 0;
}

int CBatchLine::GetProfitLine(string &strRetCode)
{
    CBValue *ptrValue = new CBProfit;
	if (0 != ptrValue->Init(this, strRetCode))
	{
	    ERROR("GetProfitLine Init error. strRetCode: "<<strRetCode<<", m_strItem: "<<m_strItem);
		delete ptrValue;
		ptrValue = NULL;
		return 1;
	}
	
	if (0 != CreateQuartLine(ptrValue, strRetCode))
	{
	    ERROR("Call CreateQuartLine error. strRetCode: "<<strRetCode);
	    delete ptrValue;
		ptrValue = NULL;
	}
	delete ptrValue;
	ptrValue = NULL;
	
	return 0;

}

int CBatchLine::GetROELine(string &strRetCode)
{
	return 0;
}

int CBatchLine::GetDivRateLine(string &strRetCode)
{
	return 0;
}

int CBatchLine::GetPRLine(string &strRetCode)
{
	return 0;
}

int CBatchLine::GetPR2Line(string &strRetCode)
{
    return 0;
}

int CBatchLine::GetPriceLine(string &strRetCode)
{
    CBValue *ptrValue = new CBPrice;
	if (0 != ptrValue->Init(this, strRetCode))
	{
	    ERROR("GetProfitLine Init error. strRetCode: "<<strRetCode<<", m_strItem: "<<m_strItem);
		delete ptrValue;
		ptrValue = NULL;
		return 1;
	}
	
	if (0 != CreateDayLine(ptrValue, strRetCode))
	{
	    ERROR("Call CreateDayLine error. strRetCode: "<<strRetCode);
	    delete ptrValue;
		ptrValue = NULL;
	}
	delete ptrValue;
	ptrValue = NULL;
	
	return 0;
}

int CBatchLine::GetReinPriceLine(string &strRetCode)
{
    CBValue *ptrValue = new CBPrice;
	if (0 != ptrValue->Init(this, strRetCode))
	{
	    ERROR("GetProfitLine Init error. strRetCode: "<<strRetCode<<", m_strItem: "<<m_strItem);
		delete ptrValue;
		ptrValue = NULL;
		return 1;
	}
	
	if (0 != CreateDayLine(ptrValue, strRetCode))
	{
	    ERROR("Call CreateDayLine error. strRetCode: "<<strRetCode);
	    delete ptrValue;
		ptrValue = NULL;
	}
	delete ptrValue;
	ptrValue = NULL;
	
	return 0;

}

int CBatchLine::Init(  // 根据传入的代码列表初始化
	vector<string> vecCode,
	string &strRetCode
	)
{
	map<string, StockData*>::iterator iter;

	for (int i=0; i<vecCode.size(); i++)
	{
		iter = CStockMap::Instance()->m_mapStock.find(vecCode[i]);
		if (iter == CStockMap::Instance()->m_mapStock.end())
		{
			continue;
		}
		m_vecStock.push_back(iter->second);
	}

    return 0;
}

int CBatchLine::Init( // 根据传入的细分行业初始化
	string strSub,
	string &strRetCode
	)
{
    return 0;
}

int CBatchLine::GetQuartReport(
    string &strRetCode
    )
{
    m_vecBatchQuart.clear();

	CBatchQuart objBatchQuart;
    CSingleLine *ptrSingleLine = new CSingleLine;
	StockData *ptrStock = NULL;
    for (int i=0; i<m_vecStock.size(); i++)
	{
	    ptrStock = m_vecStock[i];
		if (NULL == ptrStock)
		{
		    continue;
		}
		ptrSingleLine->m_ptrStock = ptrStock;
		if (0 != ptrSingleLine->GetQuartReport())
		{
		    delete ptrSingleLine;
			ptrSingleLine = NULL;
			strRetCode = CErrGetReport;
			ERROR("Call ptrSingleLine->GetQuartReport. strCode: "<<ptrStock->m_strCode
				<<", strRetCode: "<<strRetCode);
			return 1;
		}
		if (0 == ptrSingleLine->m_vecQuartReport.size())
		{
		    continue;
		}

		objBatchQuart.m_strCode = ptrStock->m_strCode;
		objBatchQuart.m_strName = ptrStock->m_strName;
		objBatchQuart.m_vecReport = ptrSingleLine->m_vecQuartReport;
		m_vecBatchQuart.push_back(objBatchQuart);
	}
	delete ptrSingleLine;
	ptrSingleLine = NULL;
	
	return 0;
}

int CBatchLine::InfirmQuartIdx( // 确定显示区域坐标, 生成x轴坐标
	string &strRetCode
	)
{
    int i=0;
	int j=0;
	int t=0;
	int iBegin = 0;
	int iEnd   = 0;
    set<int> setDate;
	setDate.clear();
	set<int>::iterator iter;
	vector<int> vecDate;
	vecDate.clear();
	
	CBatchQuart *ptrBatchQuart = NULL;
	for (int i=0; i<m_vecBatchQuart.size(); i++) // 得到所有有效日期
	{
	    ptrBatchQuart = &(m_vecBatchQuart[i]);
		for (int j=0; j<ptrBatchQuart->m_vecReport.size(); j++)
		{
		    if (0 < ptrBatchQuart->m_vecReport[j].m_iDate)
	    	{
	    	    setDate.insert(ptrBatchQuart->m_vecReport[j].m_iDate);
	    	}
		}
	}

	vecDate.clear();
	for(iter=setDate.begin(); iter != setDate.end(); iter++)
	{
	    vecDate.push_back(*iter);
	}
	if (0 == vecDate.size())
	{
	    strRetCode = CErrSizeZero;
	    ERROR("Quart Report size is 0. strRetCode: "<<strRetCode);
	    return 1;
	}
	reverse(vecDate.begin(), vecDate.end());  // 日期设为从大到小排列

    if (0 != m_iDate)
    {
		// 二分查找确定有效日期范围
		i = 0;
		j = vecDate.size()-1;
		t = 0;
		while (i < j)
		{
		    t = (i + j)/2;
			if (vecDate[t] == m_iDate)
			{
			    break;
			}
			else if (vecDate[t] > m_iDate)
			{
			    i = t + 1;
			}
			else
			{
			    j = t - 1;
			}
		}
    }
	else
	{
	    t = 0;
	}
	
	iBegin = t - m_iAfterCnt;
	iEnd   = t + m_iBeforeCnt;
	if (iBegin < 0)
	{
	    iBegin = 0;
	}
	if ((iEnd >= vecDate.size()) || (m_iBeforeCnt < 0))
	{
	    iEnd = vecDate.size() - 1;
	}

    // 生成X轴坐标
	m_vecXVal.clear();
	m_vecXVal.assign(vecDate.begin()+iBegin, vecDate.begin()+iEnd+1);
	if (0 == m_vecXVal.size())
	{
	    strRetCode = CErrSizeZero;
	    ERROR("Quart m_vecXVal size is 0. strRetCode: "<<strRetCode);
	    return 1;
	}
		
    return 0;
}

int CBatchLine::CreateQuartLine(
	CBValue *ptrValue,
	string &strRetCode
	)
{
	CBatchQuart *ptrBatchQuart = NULL;
	CBatchYVal objBatchYVal;
	int t1 = 0;
	int t2 = 0;
	int t3 = 0;
	int tbak = 0;

    if (0 != GetQuartReport(strRetCode))
	{
	    ERROR("Call GetQuartReport error. strRetCode: "<<strRetCode);
		return 1;
	}

	if (0 != InfirmQuartIdx(strRetCode))
	{
	    ERROR("Call InfirmQuartIdx error. strRetCode: "<<strRetCode);
		return 1;
	}

	// 生成y轴坐标，此处的m_vecBatchQuart和m_vecXVal元素个数在前边代码已经判断,已经都不是0
	m_iMaxYVal = 0;
	m_iMinYVal = 0;
	for (int i=0; i<m_vecBatchQuart.size(); i++)
	{
		ptrBatchQuart = &(m_vecBatchQuart[i]);
	    objBatchYVal.m_strCode = ptrBatchQuart->m_strCode;
		objBatchYVal.m_strName = ptrBatchQuart->m_strName;
		objBatchYVal.m_iMax    = 0;
		objBatchYVal.m_iMin    = 0;
		objBatchYVal.m_vecItems.clear();
		
	    // 二分查找得到起始点
		t1 = 0;
		t2 = ptrBatchQuart->m_vecReport.size() - 1;
		t3 = 0;
		while (t1 < t2)
		{
		    t3 = (t1 + t2)/2;
		    if (ptrBatchQuart->m_vecReport[t3].m_iDate == m_vecXVal[0])
	    	{
	    	    break;
	    	}
			else if (ptrBatchQuart->m_vecReport[t3].m_iDate > m_vecXVal[0])
			{
			    t1 = t3 + 1;
			}
			else
			{
			    t2 = t3 - 1;
			}
		}
        DEBUG("t1: "<<t1<<", t2: "<<t2<<", t3: "<<t3);
		
        // 生成Y轴坐标值
        int iTemp = 0;
	    for (int j=0; j<m_vecXVal.size(); j++)
    	{
    	    while ((t3 < ptrBatchQuart->m_vecReport.size()) && (ptrBatchQuart->m_vecReport[t3].m_iDate > m_vecXVal[j]))
	    	{
	    	    t3++;
	    	}
            DEBUG("j: "<<j<<", m_vecXVal[j]: "<<m_vecXVal[j]<<", t3: "<<t3<<", t3 date: "<<ptrBatchQuart->m_vecReport[t3].m_iDate);
			
			if ((t3 < ptrBatchQuart->m_vecReport.size()) && (ptrBatchQuart->m_vecReport[t3].m_iDate == m_vecXVal[j]))
			{
			    iTemp = ptrValue->Item(i, t3);
			    objBatchYVal.m_vecItems.push_back(iTemp);
				t3++;

                // 计算最值
				if (0 == j)
				{
				    objBatchYVal.m_iMax = objBatchYVal.m_iMin = iTemp;
				}
				else
				{
				    objBatchYVal.m_iMax = (objBatchYVal.m_iMax < iTemp) ? iTemp : objBatchYVal.m_iMax;
					objBatchYVal.m_iMin = (objBatchYVal.m_iMin > iTemp) ? iTemp : objBatchYVal.m_iMin;
				}
			}
			else
			{
			    objBatchYVal.m_vecItems.push_back(0);
				t3++;
			}
    	}

		if (m_vecXVal.size() != objBatchYVal.m_vecItems.size()) // 检查元素数量是否对应
		{
		    strRetCode = CErrCount;
			ERROR("m_vecXVal.size() != objBatchYVal.m_vecItems.size(). "<<m_vecXVal.size()<<" != "
				<<objBatchYVal.m_vecItems.size()<<". strRetCode: "<<strRetCode<<", strCode: "
				<<objBatchYVal.m_strCode);
			return 1;
		}

        // 计算全局最值
		if (0 == i)
		{
		    m_iMaxYVal = objBatchYVal.m_iMax;
			m_iMinYVal = objBatchYVal.m_iMin;
		}
		else
		{
		    m_iMaxYVal = m_iMaxYVal > objBatchYVal.m_iMax ? m_iMaxYVal : objBatchYVal.m_iMax;
			m_iMinYVal = m_iMinYVal < objBatchYVal.m_iMin ? m_iMinYVal : objBatchYVal.m_iMin;
		}

		m_vecBatchYVal.push_back(objBatchYVal);
	}
	
    return 0;
}
	
int CBatchLine::GetDayInfo(
    string &strRetCode
    )
{
    m_vecBatchDay.clear();
    
    CBatchDayPInfo objBatchDay;
    CSingleLine *ptrSingleLine = new CSingleLine;
    StockData *ptrStock = NULL;
    for (int i=0; i<m_vecStock.size(); i++)
    {
        ptrStock = m_vecStock[i];
        if (NULL == ptrStock)
        {
            continue;
        }
        ptrSingleLine->m_ptrStock = ptrStock;
        if (0 != ptrSingleLine->GetDayInfo())
        {
            delete ptrSingleLine;
            ptrSingleLine = NULL;
            strRetCode = CErrGetReport;
            ERROR("Call ptrSingleLine->GetQuartReport. strCode: "<<ptrStock->m_strCode
                <<", strRetCode: "<<strRetCode);
            return 1;
        }
        if (0 == ptrSingleLine->m_vecDayInfo.size())
        {
            continue;
        }
    
        objBatchDay.m_strCode = ptrStock->m_strCode;
        objBatchDay.m_strName = ptrStock->m_strName;
        objBatchDay.m_vecDayPInfo = ptrSingleLine->m_vecDayInfo;
        m_vecBatchDay.push_back(objBatchDay);
    }
    delete ptrSingleLine;
    ptrSingleLine = NULL;
    
    return 0;
}

int CBatchLine::InfirmDayIdx( // 确定显示区域坐标
	string &strRetCode
	)
{
    int i=0;
    int j=0;
    int t=0;
    int iBegin = 0;
    int iEnd   = 0;
    set<int> setDate;
    setDate.clear();
    set<int>::iterator iter;
    vector<int> vecDate;
    vecDate.clear();
    
    CBatchDayPInfo *ptrBatchDay = NULL;
    for (int i=0; i<m_vecBatchDay.size(); i++) // 得到所有有效日期
    {
        ptrBatchDay = &(m_vecBatchDay[i]);
        for (int j=0; j<ptrBatchDay->m_vecDayPInfo.size(); j++)
        {
            if (0 < ptrBatchDay->m_vecDayPInfo[j].date)
            {
               setDate.insert(ptrBatchDay->m_vecDayPInfo[j].date);
            }
        }
    }
    
    vecDate.clear();
    for(iter=setDate.begin(); iter != setDate.end(); iter++)
    {
        vecDate.push_back(*iter);
    }
    if (0 == vecDate.size())
    {
        strRetCode = CErrSizeZero;
        ERROR("Day vecDate size is 0. strRetCode: "<<strRetCode);
        return 1;
    }
    reverse(vecDate.begin(), vecDate.end());  // 日期设为从大到小排列
    
    if (0 != m_iDate)
    {
        // 二分查找确定有效日期范围
        i = 0;
        j = vecDate.size()-1;
        t = 0;
        while (i < j)
        {
            t = (i + j)/2;
            if (vecDate[t] == m_iDate)
            {
                break;
            }
            else if (vecDate[t] > m_iDate)
            {
                i = t + 1;
            }
            else
            {
                j = t - 1;
            }
        }
    }
    else
    {
        t = 0;
    }
    
    iBegin = t - m_iAfterCnt;
    iEnd   = t + m_iBeforeCnt;
    if (iBegin < 0)
    {
        iBegin = 0;
    }
    if ((iEnd >= vecDate.size()) || (m_iBeforeCnt < 0))
    {
        iEnd = vecDate.size() - 1;
    }
    
    // 生成X轴坐标
    m_vecXVal.clear();
    m_vecXVal.assign(vecDate.begin()+iBegin, vecDate.begin()+iEnd+1);
    if (0 == m_vecXVal.size())
    {
        strRetCode = CErrSizeZero;
        ERROR("Day m_vecXVal size is 0. strRetCode: "<<strRetCode);
        return 1;
    }
        
    return 0;
}

int CBatchLine::CreateDayLine(
	CBValue *ptrValue,
	string &strRetCode
	)
{
    CBatchDayPInfo *ptrBatchDay = NULL;
    CBatchYVal objBatchYVal;
    int t1 = 0;
    int t2 = 0;
    int t3 = 0;
    int tbak = 0;
    
    if (0 != GetDayInfo(strRetCode))
    {
        ERROR("Call GetDayInfo error. strRetCode: "<<strRetCode);
        return 1;
    }
    
    if (0 != InfirmDayIdx(strRetCode))
    {
        ERROR("Call InfirmDayIdx error. strRetCode: "<<strRetCode);
        return 1;
    }
    
    // 生成y轴坐标，此处的m_vecBatchQuart和m_vecXVal元素个数在前边代码已经判断,已经都不是0
    m_iMaxYVal = 0;
    m_iMinYVal = 0;
    for (int i=0; i<m_vecBatchDay.size(); i++)
    {
        ptrBatchDay = &(m_vecBatchDay[i]);
        objBatchYVal.m_strCode = ptrBatchDay->m_strCode;
        objBatchYVal.m_strName = ptrBatchDay->m_strName;
        objBatchYVal.m_iMax    = 0;
        objBatchYVal.m_iMin    = 0;
        objBatchYVal.m_vecItems.clear();
        
        // 二分查找得到起始点
        t1 = 0;
        t2 = ptrBatchDay->m_vecDayPInfo.size() - 1;
        t3 = 0;
        while (t1 < t2)
        {
            t3 = (t1 + t2)/2;
            if (ptrBatchDay->m_vecDayPInfo[t3].date == m_vecXVal[0])
            {
                break;
            }
            else if (ptrBatchDay->m_vecDayPInfo[t3].date > m_vecXVal[0])
            {
                t1 = t3 + 1;
            }
            else
            {
                t2 = t3 - 1;
            }
        }
        DEBUG("t1: "<<t1<<", t2: "<<t2<<", t3: "<<t3);
        
        // 生成Y轴坐标值
        int iTemp = 0;
        for (int j=0; j<m_vecXVal.size(); j++)
        {
            while ((t3 < ptrBatchDay->m_vecDayPInfo.size()) && (ptrBatchDay->m_vecDayPInfo[t3].date > m_vecXVal[j]))
            {
                t3++;
            }
            DEBUG("j: "<<j<<", m_vecXVal[j]: "<<m_vecXVal[j]<<", t3: "<<t3<<", m_vecDayPInfo size: "<<ptrBatchDay->m_vecDayPInfo.size());
            
            if ((t3 < ptrBatchDay->m_vecDayPInfo.size()) && (ptrBatchDay->m_vecDayPInfo[t3].date == m_vecXVal[j]))
            {
                iTemp = ptrValue->Item(i, t3);
                objBatchYVal.m_vecItems.push_back(iTemp);
                t3++;
    
                // 计算最值
                if ((0 == j) || (0 == objBatchYVal.m_iMin) || (0 == objBatchYVal.m_iMax))
                {
                    objBatchYVal.m_iMax = objBatchYVal.m_iMin = iTemp;
                }
                else
                {
                    objBatchYVal.m_iMax = (objBatchYVal.m_iMax < iTemp) ? iTemp : objBatchYVal.m_iMax;
                    objBatchYVal.m_iMin = (objBatchYVal.m_iMin > iTemp) ? iTemp : objBatchYVal.m_iMin;
                }
            }
            else
            {
                objBatchYVal.m_vecItems.push_back(0);
                //t3++;
            }
        }
    
        if (m_vecXVal.size() != objBatchYVal.m_vecItems.size()) // 检查元素数量是否对应
        {
            strRetCode = CErrCount;
            ERROR("m_vecXVal.size() != objBatchYVal.m_vecItems.size(). "<<m_vecXVal.size()<<" != "
                <<objBatchYVal.m_vecItems.size()<<". strRetCode: "<<strRetCode<<", strCode: "
                <<objBatchYVal.m_strCode);
            return 1;
        }
    
        // 计算全局最值
        if (0 == i)
        {
            m_iMaxYVal = objBatchYVal.m_iMax;
            m_iMinYVal = objBatchYVal.m_iMin;
        }
        else
        {
            m_iMaxYVal = m_iMaxYVal > objBatchYVal.m_iMax ? m_iMaxYVal : objBatchYVal.m_iMax;
            m_iMinYVal = m_iMinYVal < objBatchYVal.m_iMin ? m_iMinYVal : objBatchYVal.m_iMin;
        }
    
        m_vecBatchYVal.push_back(objBatchYVal);
    }
    
    return 0;
}

/*
result
字段名称	值类型	备注
max	float	最大值
min	float	最小值
yline	{{type, mval, []}, {type, mval, []}, …}	
xline	[]	

// 显示信息
string m_strItem;
vector<CBatchYVal> m_vecBatchYVal;
vector<int> m_vecXVal;
int m_iMaxYVal;
int m_iMinYVal;

*/
int CBatchLine::ShowResult(
	json_t *jRes,
	string &strRetCode
	)
{
	json_t *jResult = json_object();
	json_object_set_new(jResult, "item", json_string(m_strItem.c_str()));
	json_object_set_new(jResult, "max", json_integer(m_iMaxYVal));	// 全局最大值
	json_object_set_new(jResult, "min", json_integer(m_iMinYVal));	// 全局最小值
	
	json_t *jBatchLine = json_array();  // 季度曲线Y轴坐标
	for (int i=0; i<m_vecBatchYVal.size(); i++)
	{
		json_t *jYLine = json_object();
		json_object_set_new(jYLine, "code", json_string(m_vecBatchYVal[i].m_strCode.c_str()));
		json_object_set_new(jYLine, "name", json_string(m_vecBatchYVal[i].m_strName.c_str()));
		json_object_set_new(jYLine, "max", json_integer(m_vecBatchYVal[i].m_iMax));
		json_object_set_new(jYLine, "min", json_integer(m_vecBatchYVal[i].m_iMin));
		json_t *jItems = json_array();
		for (int j=0; j<m_vecBatchYVal[i].m_vecItems.size(); j++)
		{
			json_array_append_new(jItems, json_integer(m_vecBatchYVal[i].m_vecItems[j]));
		}
		json_object_set_new(jYLine, "items", jItems);
		json_array_append_new(jBatchLine, jYLine);
	}
	json_object_set_new(jResult, "yline", jBatchLine);
	
	json_t *jXLine = json_array();  // 季度曲线X轴坐标
	for (int i=0; i<m_vecXVal.size(); i++)
	{
		json_array_append_new(jXLine, json_integer(m_vecXVal[i]));
	}
	json_object_set_new(jResult, "xline", jXLine);
	
	json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	json_object_set_new(jRes, "result", jResult);

    return 0;
}

