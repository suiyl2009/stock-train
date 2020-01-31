#include "KLine.h"
#include "OutInfo.h"
#include "Log.h"
#include "Constant.h"
#include "Calendar.h"
#include <algorithm>

CKLine::CKLine()
{
	// 初值及参数值
	m_ptrStock = NULL;
    m_strCycle = "";
    m_iRecover = 0;
    
	m_iDate       = 0;
	m_iBeforeCnt  = 0;
	m_iAfterCnt   = 0;

	// 显示区段坐标
	m_idxPivot  = 0;
	m_idxDBegin = 0;
	m_idxDEnd   = 0;

    // 显示信息
    m_vecKLine.clear();
    m_vecTotalCnt.clear();
    m_vecReportDate.clear();
	m_iKMaxVal = 0;
	m_iKMinVal = 0;
	m_iVolMax  = 0;
	m_iVolMin  = 0;
}

CKLine::~CKLine()
{
}

int CKLine::GetLines(    // 
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
    int iRet = 0;
	size_t sJsonSize = 0;
	string strCode = "";
    map<string, StockData*>::iterator iter;
	json_t *jItem;
    
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
	m_strCycle = json_string_value(jItem);

	jItem = json_object_get(jReq, "recover");
    if (!jItem || !json_is_integer(jItem)) {
		strRetCode = CErrJsonType;
	    ERROR("GetLines json recover is not string. strRetCode: "<<strRetCode);
		return 1;
    }
	m_iRecover = json_integer_value(jItem);
	
    jItem = json_object_get(jReq, "date");
    if (jItem && json_is_integer(jItem)) 
	{
		m_iDate = json_integer_value(json_object_get(jReq, "date"));
		if (((0 != m_iDate) && (m_iDate < 19800000)) || (m_iDate > 99999999))
		{
	        strRetCode = CErrJsonType;
			ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", iDate: "<<m_iDate);
			return 1;
		}
    }
	
    jItem = json_object_get(jReq, "before");
    if (jItem && json_is_integer(jItem)) 
	{
		m_iBeforeCnt = json_integer_value(json_object_get(jReq, "before"));
		if (m_iBeforeCnt < -1)
		{
	        strRetCode = CErrJsonType;
			ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", iBeforeCnt: "<<m_iBeforeCnt);
			return 1;
		}
    }

	jItem = json_object_get(jReq, "after");
    if (jItem && json_is_integer(jItem)) 
	{
		m_iAfterCnt = json_integer_value(json_object_get(jReq, "after"));
		if (m_iAfterCnt < 0)
		{
	        strRetCode = CErrJsonType;
			ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", iAfterCnt: "<<m_iAfterCnt);
			return 1;
		}
    }

	// 从stockmap中找到满足条件的元素
	iter = CStockMap::Instance()->m_mapStock.find(strCode);
	if (iter == CStockMap::Instance()->m_mapStock.end())
	{
	    strRetCode = CErrOutOfRange;
	    ERROR("GetLines strCode not found. strCode: "<<strCode<<", strRetCode: "<<strRetCode);
		return 1;
	}

	iter = CStockMap::Instance()->m_mapStock.find(strCode);
	if (iter == CStockMap::Instance()->m_mapStock.end())
	{
	    strRetCode = CErrOutOfRange;
	    ERROR("GetLines strCode not found. strCode: "<<strCode<<", strRetCode: "<<strRetCode);
		return 1;
	}

    iRet = Init(iter->second, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call Init error. strRetCode: "<<strRetCode);
        return iRet;
    }

    // 确定枢纽坐标
    iRet = InfirmPivotIdx(m_iDate, m_idxPivot, strRetCode);
	if (0 != iRet)
	{
	    ERROR("Call InfirmShowIdx error. m_iDate: "<<m_iDate<<", m_iBeforeCnt: "<<m_iBeforeCnt
			<<", m_iAfterCnt: "<<m_iAfterCnt<<", strRetCode: "<<strRetCode);
		return iRet;
	}

    if (1 != COutInfo::Instance()->m_mapKLine.count(m_strCycle))
    {
        strRetCode = CErrItemNone;
        ERROR("Not support this cycle line. m_strCycle: "<<m_strCycle<<", strRetCode: "<<strRetCode);
        return 1;
    }
    
    iRet = (this->*(COutInfo::Instance()->m_mapKLine[m_strCycle]))(strRetCode);
    if (0 != iRet)
    {
        ERROR("Call (this->*(COutInfo::Instance()->m_mapKLine[m_strCycle])) error. strRetCode: "<<strRetCode);
        return iRet;
    }

    json_object_set_new(jRes, "method", json_string("k.line"));
    iRet = ShowResult(jRes, strRetCode);
	if (0 != iRet)
	{
	    ERROR("Call ShowResult error. strRetCode: "<<strRetCode);
		return iRet;
	}
    
    return iRet;
}

int CKLine::GetDayLine(string &strRetCode)
{
    int iRet = 0;

	// 价格显示区段下标范围
	if (0 == m_ptrStock->m_vecDayData.size())
	{
	    m_idxDEnd = m_idxDBegin = -1;
		return 0;
	}

	m_idxDBegin = m_idxPivot - m_iAfterCnt;
	if (0 > m_idxDBegin)
	{
	    m_idxDBegin = 0;
	}
	m_idxDEnd = m_idxPivot + m_iBeforeCnt;
	if ((m_idxDEnd >= m_ptrStock->m_vecDayData.size()) || (m_iBeforeCnt < 0))
	{
	    m_idxDEnd = m_ptrStock->m_vecDayData.size() - 1;
	}
	DEBUG("m_idxDBegin: "<<m_idxDBegin<<", m_idxDEnd: "<<m_idxDEnd);
	
	iRet = GetDayData(strRetCode); // 得到日线数据
	if (0 != iRet)
	{
		ERROR("Call GetDayData error. strRetCode: "<<strRetCode);
		return iRet;
	}
	
	iRet = Recover(strRetCode);  // 复权
	if (0 != iRet)
	{
		ERROR("Call Recover error. strRetCode: "<<strRetCode);
		return iRet;
	}

    return 0;
}

int CKLine::GetWeekLine(string &strRetCode)
{
    int iRet   = 0;
    int iCount = 0;
	int bCount = false;
	int idxVecDay = 0;
    int iDate = m_ptrStock->m_vecDayData[m_idxPivot].date;
	CCalendar objCal;
	CCalendar objCalPivot;
	CKData objKData;
	vector<CKData> vecKAfter;
	int iInterval = objCalPivot.IntervalDays(iDate);
	if (0 == iInterval)
	{
	    strRetCode = CErrOutOfRange;
	    ERROR("Call objCal.IntervalDays error. iDate: "<<iDate<<", strRetCode: "<<strRetCode);
	    return 1;
	}
	iInterval = iInterval - 2*iInterval;
	if (iDate != objCalPivot.AddDay(iInterval)) // objCal 设置为iDate
	{
		strRetCode = CErrOutOfRange;
	    ERROR("Call objCal.AddDay error. iDate: "<<iDate<<", objCalPivot: "<<objCalPivot.GetDate()
			<<", iInterval: "<<iInterval<<", strRetCode: "<<strRetCode);
	    return 1;
	}
	if (5 != objCalPivot.m_iWeekDay)
	{
		if (0 == objCalPivot.SubDay((7+objCalPivot.m_iWeekDay-5)%7)) // 向前移动到周线收盘处
		{
			strRetCode = CErrOutOfRange;
		    ERROR("Call objCal.SubDay error. iDate: "<<iDate<<", day: "<<(7+objCalPivot.m_iWeekDay-5)%7
				<<", strRetCode: "<<strRetCode);
		    return 1;
		}
	}
	iDate = objCalPivot.GetDate(); // 记录周线枢纽位置的日期

	//////////////////////////begin: before///////////////////////////
	objCal = objCalPivot;
	idxVecDay = m_idxPivot;
	while ((idxVecDay < m_ptrStock->m_vecDayData.size()) && (m_ptrStock->m_vecDayData[idxVecDay].date > iDate) )
		idxVecDay++;

    iCount = 0;
	m_vecKLine.clear();
	while ((iCount<m_iBeforeCnt) && (m_ptrStock->m_vecDayData.size() > idxVecDay))
	{
		iDate = objCal.SubDay(7);
		bCount = false;
		for (; (m_ptrStock->m_vecDayData.size() > idxVecDay) && 
			(m_ptrStock->m_vecDayData[idxVecDay].date > iDate); idxVecDay++)
		{
		    if (false == bCount)
	    	{
	    	    bCount = true;
				iCount++;
			}
				
			objKData.amount = m_ptrStock->m_vecDayData[idxVecDay].amount;
			objKData.close  = m_ptrStock->m_vecDayData[idxVecDay].close;
			objKData.date = m_ptrStock->m_vecDayData[idxVecDay].date;
			objKData.dayCnt = 1;
			objKData.high = m_ptrStock->m_vecDayData[idxVecDay].high;
			objKData.low = m_ptrStock->m_vecDayData[idxVecDay].low;
			objKData.open = m_ptrStock->m_vecDayData[idxVecDay].open;
			objKData.vol = m_ptrStock->m_vecDayData[idxVecDay].vol;
			m_vecKLine.push_back(objKData);
		}
	}
	m_idxDEnd = (m_ptrStock->m_vecDayData.size() <= idxVecDay) ? (m_ptrStock->m_vecDayData.size() - 1) : idxVecDay;
	//////////////////////////end: before///////////////////////////

	//////////////////////////begin: after///////////////////////////
	idxVecDay = m_idxPivot;
	objCal = objCalPivot;
	iDate = objCal.AddDay(3);
	while ((idxVecDay >= 0) && (m_ptrStock->m_vecDayData[idxVecDay].date < iDate) )
		idxVecDay--;

	iCount = 0;
	vecKAfter.clear();
	while ((iCount<m_iAfterCnt) && (0 <= idxVecDay))
	{
		iDate = objCal.AddDay(7);
		bCount = false;
		for (; (0 <= idxVecDay) && (m_ptrStock->m_vecDayData[idxVecDay].date < iDate); idxVecDay--)
		{
		    if (false == bCount)
	    	{
	    	    bCount = true;
				iCount++;
		    }
			objKData.amount = m_ptrStock->m_vecDayData[idxVecDay].amount;
			objKData.close  = m_ptrStock->m_vecDayData[idxVecDay].close;
			objKData.date = m_ptrStock->m_vecDayData[idxVecDay].date;
			objKData.dayCnt = 1;
			objKData.high = m_ptrStock->m_vecDayData[idxVecDay].high;
			objKData.low = m_ptrStock->m_vecDayData[idxVecDay].low;
			objKData.open = m_ptrStock->m_vecDayData[idxVecDay].open;
			objKData.vol = m_ptrStock->m_vecDayData[idxVecDay].vol;
			vecKAfter.push_back(objKData);
		}
	}
	m_idxDBegin = (0 <= idxVecDay) ? idxVecDay : 0;
	reverse(vecKAfter.begin(), vecKAfter.end());
	//////////////////////////end: after///////////////////////////

	vecKAfter.insert(vecKAfter.end(), m_vecKLine.begin(), m_vecKLine.end());
	m_vecKLine = vecKAfter;

    iRet = Recover(strRetCode);
	if (0 != iRet)
	{
		ERROR("Call Recover error. strRetCode: "<<strRetCode);
		return 1;
	}
	
	iRet = GetWeekData(strRetCode);
	if (0 != iRet)
	{
	    ERROR("Call GetWeekData error. strRetCode: "<<strRetCode);
		return 1;
	}
	
    return 0;
}

int CKLine::GetMonthLine(string &strRetCode)
{
    int iRet        = 0;
	int iMonth      = 0;
	int iCount      = 0;
	int idxVecDay   = 0;
	bool bCount     = 0;
	CKData objKData;

	iMonth = m_ptrStock->m_vecDayData[m_idxPivot].date/100;
	while ((0 <= m_idxPivot) && (m_ptrStock->m_vecDayData[m_idxPivot].date/100 == iMonth))
	{
	    m_idxPivot--;
	}

	//////////////////////////begin: after///////////////////////////
    iCount = 0;
	m_vecKLine.clear();
	idxVecDay = m_idxPivot;
	while ((iCount<m_iAfterCnt) && (0 <= idxVecDay))
	{
	    bCount = false;
	    iMonth = m_ptrStock->m_vecDayData[idxVecDay].date/100;
		for (; (idxVecDay>=0) && (iMonth == m_ptrStock->m_vecDayData[idxVecDay].date/100); idxVecDay--)
		{
			if (false == bCount)
	    	{
	    	    bCount = true;
				iCount++;
		    }
			
		    objKData.amount = m_ptrStock->m_vecDayData[idxVecDay].amount;
			objKData.close  = m_ptrStock->m_vecDayData[idxVecDay].close;
			objKData.date = m_ptrStock->m_vecDayData[idxVecDay].date;
			objKData.dayCnt = 1;
			objKData.high = m_ptrStock->m_vecDayData[idxVecDay].high;
			objKData.low = m_ptrStock->m_vecDayData[idxVecDay].low;
			objKData.open = m_ptrStock->m_vecDayData[idxVecDay].open;
			objKData.vol = m_ptrStock->m_vecDayData[idxVecDay].vol;
			m_vecKLine.push_back(objKData);
		}
	}
	m_idxDBegin = (0 <= idxVecDay) ? idxVecDay : 0;
	reverse(m_vecKLine.begin(), m_vecKLine.end());
	//////////////////////////end: after///////////////////////////

	//////////////////////////begin: before///////////////////////////
	iCount = 0;
	idxVecDay = m_idxPivot + 1;
	while ((iCount<m_iBeforeCnt) && (m_ptrStock->m_vecDayData.size() > idxVecDay))
	{
	    bCount = false;
		iMonth = m_ptrStock->m_vecDayData[idxVecDay].date/100;
		for (; (idxVecDay < m_ptrStock->m_vecDayData.size()) 
			&& (iMonth == m_ptrStock->m_vecDayData[idxVecDay].date/100); idxVecDay++)
		{
		    if (false == bCount)
	    	{
	    	    bCount = true;
				iCount++;
	    	}
			objKData.amount = m_ptrStock->m_vecDayData[idxVecDay].amount;
			objKData.close  = m_ptrStock->m_vecDayData[idxVecDay].close;
			objKData.date = m_ptrStock->m_vecDayData[idxVecDay].date;
			objKData.dayCnt = 1;
			objKData.high = m_ptrStock->m_vecDayData[idxVecDay].high;
			objKData.low = m_ptrStock->m_vecDayData[idxVecDay].low;
			objKData.open = m_ptrStock->m_vecDayData[idxVecDay].open;
			objKData.vol = m_ptrStock->m_vecDayData[idxVecDay].vol;
			m_vecKLine.push_back(objKData);
		}
	}
	m_idxDEnd = (m_ptrStock->m_vecDayData.size() <= idxVecDay) ? (m_ptrStock->m_vecDayData.size() - 1) : idxVecDay;
	//////////////////////////end: before///////////////////////////

    iRet = Recover(strRetCode);
	if (0 != iRet)
	{
		ERROR("Call Recover error. strRetCode: "<<strRetCode);
		return 1;
	}
	
	iRet = GetMonthData(strRetCode);
	if (0 != iRet)
	{
	    ERROR("Call GetWeekData error. strRetCode: "<<strRetCode);
		return 1;
	}
	

    return 0;
}

int CKLine::GetYearLine(string &strRetCode)
{
    int iRet        = 0;
	int iYear       = 0;
	int iCount      = 0;
	int idxVecDay   = 0;
	bool bCount     = 0;
	CKData objKData;

	iYear = m_ptrStock->m_vecDayData[m_idxPivot].date/10000;
	while ((0 <= m_idxPivot) && (m_ptrStock->m_vecDayData[m_idxPivot].date/10000 == iYear))
	{
	    m_idxPivot--;
	}

	//////////////////////////begin: after///////////////////////////
    iCount = 0;
	m_vecKLine.clear();
	idxVecDay = m_idxPivot;
	while ((iCount<m_iAfterCnt) && (0 <= idxVecDay))
	{
	    bCount = false;
	    iYear = m_ptrStock->m_vecDayData[idxVecDay].date/10000;
		for (; (idxVecDay>=0) && (iYear == m_ptrStock->m_vecDayData[idxVecDay].date/10000); idxVecDay--)
		{
			if (false == bCount)
	    	{
	    	    bCount = true;
				iCount++;
		    }
			
		    objKData.amount = m_ptrStock->m_vecDayData[idxVecDay].amount;
			objKData.close  = m_ptrStock->m_vecDayData[idxVecDay].close;
			objKData.date = m_ptrStock->m_vecDayData[idxVecDay].date;
			objKData.dayCnt = 1;
			objKData.high = m_ptrStock->m_vecDayData[idxVecDay].high;
			objKData.low = m_ptrStock->m_vecDayData[idxVecDay].low;
			objKData.open = m_ptrStock->m_vecDayData[idxVecDay].open;
			objKData.vol = m_ptrStock->m_vecDayData[idxVecDay].vol;
			m_vecKLine.push_back(objKData);
		}
	}
	m_idxDBegin = (0 <= idxVecDay) ? idxVecDay : 0;
	reverse(m_vecKLine.begin(), m_vecKLine.end());
	//////////////////////////end: after///////////////////////////

	//////////////////////////begin: before///////////////////////////
	iCount = 0;
	idxVecDay = m_idxPivot + 1;
	while ((iCount<m_iBeforeCnt) && (m_ptrStock->m_vecDayData.size() > idxVecDay))
	{
	    bCount = false;
		iYear = m_ptrStock->m_vecDayData[idxVecDay].date/10000;
		for (; (idxVecDay < m_ptrStock->m_vecDayData.size()) 
			&& (iYear == m_ptrStock->m_vecDayData[idxVecDay].date/10000); idxVecDay++)
		{
		    if (false == bCount)
	    	{
	    	    bCount = true;
				iCount++;
	    	}
			objKData.amount = m_ptrStock->m_vecDayData[idxVecDay].amount;
			objKData.close  = m_ptrStock->m_vecDayData[idxVecDay].close;
			objKData.date = m_ptrStock->m_vecDayData[idxVecDay].date;
			objKData.dayCnt = 1;
			objKData.high = m_ptrStock->m_vecDayData[idxVecDay].high;
			objKData.low = m_ptrStock->m_vecDayData[idxVecDay].low;
			objKData.open = m_ptrStock->m_vecDayData[idxVecDay].open;
			objKData.vol = m_ptrStock->m_vecDayData[idxVecDay].vol;
			m_vecKLine.push_back(objKData);
		}
	}
	m_idxDEnd = (m_ptrStock->m_vecDayData.size() <= idxVecDay) ? (m_ptrStock->m_vecDayData.size() - 1) : idxVecDay;
	//////////////////////////end: before///////////////////////////

    iRet = Recover(strRetCode);
	if (0 != iRet)
	{
		ERROR("Call Recover error. strRetCode: "<<strRetCode);
		return 1;
	}
	
	iRet = GetYearData(strRetCode);
	if (0 != iRet)
	{
	    ERROR("Call GetWeekData error. strRetCode: "<<strRetCode);
		return 1;
	}
	

    return 0;

}

int CKLine::Init(
	StockData *ptrStock,
	string &strRetCode
	)
{
	m_ptrStock = ptrStock;

    return 0;
}

int CKLine::InfirmPivotIdx(
	int iDate,
	int &idxPivot,
	string &strRetCode
	)
{
	int i,j,t;
	if (0 != iDate)
	{
		i = 0;
		j = m_ptrStock->m_vecDayData.size()-1;
		t = 0;
		while (i < j) // 二分查找
		{
			t = (i+j)/2;
			if (iDate == m_ptrStock->m_vecDayData[t].date)
			{
				break;
			}
			else if (iDate < m_ptrStock->m_vecDayData[t].date)
			{
				i = t+1;
			}
			else
			{
				j = t-1;
			}
		}
		DEBUG("iDate: "<<iDate);
	}
	else
	{
		t = 0;
	}
	
	idxPivot = t;
	return 0;
}

int CKLine::GetDayData(string &strRetCode)
{
	int iRet = 0;
    int nTempDate = 0;
	
	// 得到显示数据
	m_vecKLine.clear();
	m_vecTotalCnt.clear();
	m_iKMaxVal = m_iKMinVal = m_iVolMax = m_iVolMin = 0;
	CKData objKData;
	if ((0 > m_idxDBegin) || (0 > m_idxDEnd))
	{
		DEBUG("m_idxDBegin: "<<m_idxDBegin<<", m_idxDEnd: "<<m_idxDEnd);
		return 0;
	}
	
	for (int i=m_idxDBegin; i<=m_idxDEnd; i++)
	{
		objKData.amount  = m_ptrStock->m_vecDayData[i].amount;
		objKData.close	 = m_ptrStock->m_vecDayData[i].close;
		objKData.date	 = m_ptrStock->m_vecDayData[i].date;
		objKData.high	 = m_ptrStock->m_vecDayData[i].high;
		objKData.low	 = m_ptrStock->m_vecDayData[i].low;
		objKData.open	 = m_ptrStock->m_vecDayData[i].open;
		objKData.vol	 = m_ptrStock->m_vecDayData[i].vol;
		objKData.dayCnt  = 1;

		if (i == m_idxDBegin)
		{
		    m_iKMaxVal = objKData.high;
			m_iKMinVal = objKData.low;
			m_iVolMax  = objKData.vol;
			m_iVolMin  = objKData.vol;
		}
		else
		{
		    m_iKMaxVal = (objKData.high > m_iKMaxVal) ? objKData.high : m_iKMaxVal;
			m_iKMinVal = (objKData.low < m_iKMinVal) ? objKData.low : m_iKMinVal;
			m_iVolMax  = (objKData.vol > m_iVolMax) ? objKData.vol : m_iVolMax;
			m_iVolMin  = (objKData.vol < m_iVolMin) ? objKData.vol : m_iVolMin;
		}
		//DEBUG("objKData.amount: "<<objKData.amount<<", "<<m_ptrStock->m_vecDayData[i].amount);
		//DEBUG("objKData.vol: "<<objKData.vol<<", "<<m_ptrStock->m_vecDayData[i].vol);
		m_vecKLine.push_back(objKData);
	}

    m_vecReportDate.clear();
    if (m_vecKLine.size() > 0)
    {
        for (int i=0; i<m_ptrStock->m_vecQuart.size(); i++)  // 财报公告日期
        {
            for (int j=3; j>=0; j--)
            {
                nTempDate = DateStr2Int(m_ptrStock->m_vecQuart[i].m_rQuart[j].m_strDeclarationDate);
                if ((m_vecKLine[0].date >= nTempDate) && (m_vecKLine[m_vecKLine.size()-1].date <= nTempDate))
                {
                    m_vecReportDate.push_back(nTempDate);
                }
            }
        }
    }

	return iRet;
}

int CKLine::Recover(
    string &strRetCode
    )
{
    if (m_vecKLine.size() == 0)
	{
	    return 0;
	}

    // 查找范围内的除权信息
	vector<CCqcx> vecCqcx;
    int iBeginDate = m_vecKLine[0].date;
	int iEndDate = m_vecKLine[m_vecKLine.size()-1].date;
	vecCqcx.clear();
	for (int i=0; i<m_ptrStock->m_vecDivide.size(); i++)
	{
	    CCqcx objCqcx;
		objCqcx.iSzzbl = m_ptrStock->m_vecDivide[i].m_iSzzbl;
		objCqcx.iDate = DateStr2Int(m_ptrStock->m_vecDivide[i].m_strCqcxr);
		//DEBUG("Cqcx iBeginDate: "<<iBeginDate<<", iEndDate: "<<iEndDate<<", iDate: "
		//	<<objCqcx.iDate<<", iSzzbl: "<<objCqcx.iSzzbl);
	    if ((objCqcx.iDate > iBeginDate) || (0 == objCqcx.iDate))
    	{
    	    continue;
    	}
		if (objCqcx.iDate < iEndDate)
		{
		    break;
		}
		if (0 == objCqcx.iSzzbl)
		{
		    continue;
		}
		DEBUG("Cqcx iDate: "<<objCqcx.iDate<<", iSzzbl: "<<objCqcx.iSzzbl)
		vecCqcx.push_back(objCqcx);
	}

    // 除权除息复权
    int j=0;
	int iszzbl = 100;
	for (int i=0; i< m_vecKLine.size(); i++)
	{
		if ((j < vecCqcx.size()) && (m_vecKLine[i].date <= vecCqcx[j].iDate))
		{
		    DEBUG("iszzbl: "<<iszzbl);
		    iszzbl = iszzbl*(100 + vecCqcx[j].iSzzbl)/100;
			DEBUG("iszzbl: "<<iszzbl<<", vecCqcx[j].iSzzbl: "<<vecCqcx[j].iSzzbl);
			j++;
		}
	    m_vecKLine[i].open = m_vecKLine[i].open*100/iszzbl;
		m_vecKLine[i].high = m_vecKLine[i].high*100/iszzbl;
		m_vecKLine[i].low = m_vecKLine[i].low*100/iszzbl;
		m_vecKLine[i].close = m_vecKLine[i].close*100/iszzbl;
		m_vecKLine[i].vol = m_vecKLine[i].vol*iszzbl/100;
	}
	
    return 0;
}

int CKLine::GetWeekData(
	string &strRetCode
	)
{
    int nTempDate = 0;
    CKData objKData;
    CCalendar objCal;
    vector<CKData> vecWeekLine;
	vecWeekLine.clear();
	m_iKMaxVal = m_iKMinVal = m_iVolMax = m_iVolMin = 0;

	if (m_vecKLine.size() == 0)
	{
	    return 0;
	}

    m_vecReportDate.clear();
    for (int i=0; i<m_ptrStock->m_vecQuart.size(); i++)  // 财报公告日期
    {
        for (int j=3; j>=0; j--)
        {
            //nTempDate = atoi(m_ptrStock->m_vecQuart[i].m_rQuart[j].m_strDeclarationDate.c_str());
            nTempDate = DateStr2Int(m_ptrStock->m_vecQuart[i].m_rQuart[j].m_strDeclarationDate);
            if ((m_vecKLine[0].date >= nTempDate) && (m_vecKLine[m_vecKLine.size()-1].date <= nTempDate))
            {
                m_vecReportDate.push_back(nTempDate);
            }
        }
    }

	int iInterval = objCal.IntervalDays(m_vecKLine[0].date);
	if (0 >= iInterval)
	{
		if (0 == objCal.AddDay(iInterval-2*iInterval))
		{
		    ERROR("Call objCal.AddDay error. iInterval: "<<iInterval);
			return 1;
		}
	}
	else
	{
	    if (0 == objCal.SubDay(iInterval))
		{
			ERROR("Call objCal.AddDay error. iInterval: "<<iInterval);
			return 1;
		}
	}
	if (0 == objCal.SubDay(7+objCal.m_iWeekDay-5))	// 下一个周五
	{
		ERROR("Call objCal.SubDay error. objCal.date: "<<objCal.GetDate()<<", internal: "
			<<7+objCal.m_iWeekDay-5);
		return 1;
	}

    int i=0;
	int bFirst = false;
	while (i < m_vecKLine.size())
	{
        bFirst = true;
        for (; (i<m_vecKLine.size()) && (m_vecKLine[i].date > objCal.GetDate()); i++)
    	{
    	    if (true == bFirst)
	    	{
	    	    bFirst = false;
				objKData.amount = m_vecKLine[i].amount;
				objKData.close  = m_vecKLine[i].close;
				objKData.date   = m_vecKLine[i].date;
				objKData.dayCnt = 1;
				objKData.high = m_vecKLine[i].high;
				objKData.low = m_vecKLine[i].low;
				objKData.open = m_vecKLine[i].open;
				objKData.vol = m_vecKLine[i].vol;
	    	}
			else
			{
				objKData.amount += m_vecKLine[i].amount;
				//objKData.close  = m_vecKLine[i].close;
				//objKData.date   = m_vecKLine[i].date;
				objKData.dayCnt++;
				objKData.high = (objKData.high > m_vecKLine[i].high) ? objKData.high: m_vecKLine[i].high;
				objKData.low = (objKData.low < m_vecKLine[i].low) ? objKData.low : m_vecKLine[i].low;
				objKData.open = m_vecKLine[i].open;
				objKData.vol += m_vecKLine[i].vol;
			}
			//DEBUG("objKData.vol: "<<objKData.vol<<", i: "<<i<<", m_vecKLine[i].vol: "<<m_vecKLine[i].vol);
    	}
		if (false == bFirst)
		{
		    vecWeekLine.push_back(objKData);
			
			if (1 == vecWeekLine.size())  // 计算最值
			{
				m_iKMaxVal = objKData.high;
				m_iKMinVal = objKData.low;
				m_iVolMax = m_iVolMin = objKData.vol;
			}
			else
			{
				m_iKMaxVal = (m_iKMaxVal > objKData.high) ? m_iKMaxVal : objKData.high;
				m_iKMinVal = (m_iKMinVal < objKData.low) ? m_iKMinVal : objKData.low;
				m_iVolMax = (m_iVolMax > objKData.vol) ? m_iVolMax : objKData.vol;
				m_iVolMin = (m_iVolMin < objKData.vol) ? m_iVolMin : objKData.vol;
			}
		}
		
		if (0 == objCal.SubDay(7))  // 下一个周五
		{
		    ERROR("Call objCal.SubDay error. objCal.date: "<<objCal.GetDate());
			return 1;
		}
	}
	m_vecKLine = vecWeekLine;

	return 0;
}

int CKLine::GetMonthData(
	string &strRetCode
	)
{
    int iMonth = 0;
	CKData objKData;
	CCalendar objCal;
	vector<CKData> vecMonthLine;
	vecMonthLine.clear();
	m_iKMaxVal = m_iKMinVal = m_iVolMax = m_iVolMin = 0;
	
	if (m_vecKLine.size() == 0)
	{
		return 0;
	}
	
	int i=0;
	int bFirst = false;
	while (i < m_vecKLine.size())
	{
		bFirst = true;
		iMonth = m_vecKLine[i].date/100;
		for (; (i<m_vecKLine.size()) && (m_vecKLine[i].date/100 == iMonth); i++)
		{
			if (true == bFirst)
			{
				bFirst = false;
				objKData.amount = m_vecKLine[i].amount;
				objKData.close	= m_vecKLine[i].close;
				objKData.date	= m_vecKLine[i].date;
				objKData.dayCnt = 1;
				objKData.high = m_vecKLine[i].high;
				objKData.low = m_vecKLine[i].low;
				objKData.open = m_vecKLine[i].open;
				objKData.vol = m_vecKLine[i].vol;
			}
			else
			{
				objKData.amount += m_vecKLine[i].amount;
				//objKData.close  = m_vecKLine[i].close;
				//objKData.date   = m_vecKLine[i].date;
				objKData.dayCnt++;
				objKData.high = (objKData.high > m_vecKLine[i].high) ? objKData.high: m_vecKLine[i].high;
				objKData.low = (objKData.low < m_vecKLine[i].low) ? objKData.low : m_vecKLine[i].low;
				objKData.open = m_vecKLine[i].open;
				objKData.vol += m_vecKLine[i].vol;
			}
			DEBUG("objKData.vol: "<<objKData.vol<<", i: "<<i<<", m_vecKLine[i].vol: "<<m_vecKLine[i].vol);
		}
		if (false == bFirst)
		{
			vecMonthLine.push_back(objKData);
			
			if (1 == vecMonthLine.size())  // 计算最值
			{
				m_iKMaxVal = objKData.high;
				m_iKMinVal = objKData.low;
				m_iVolMax = m_iVolMin = objKData.vol;
			}
			else
			{
				m_iKMaxVal = (m_iKMaxVal > objKData.high) ? m_iKMaxVal : objKData.high;
				m_iKMinVal = (m_iKMinVal < objKData.low) ? m_iKMinVal : objKData.low;
				m_iVolMax = (m_iVolMax > objKData.vol) ? m_iVolMax : objKData.vol;
				m_iVolMin = (m_iVolMin < objKData.vol) ? m_iVolMin : objKData.vol;
			}
		}
	}
	m_vecKLine = vecMonthLine;
	
	return 0;

}

int CKLine::GetYearData(
	string &strRetCode
	)
{
	int iYear = 0;
	CKData objKData;
	CCalendar objCal;
	vector<CKData> vecYearLine;
	vecYearLine.clear();
	m_iKMaxVal = m_iKMinVal = m_iVolMax = m_iVolMin = 0;
	
	if (m_vecKLine.size() == 0)
	{
		return 0;
	}
	
	int i=0;
	int bFirst = false;
	while (i < m_vecKLine.size())
	{
		bFirst = true;
		iYear = m_vecKLine[i].date/10000;
		for (; (i<m_vecKLine.size()) && (m_vecKLine[i].date/10000 == iYear); i++)
		{
			if (true == bFirst)
			{
				bFirst = false;
				objKData.amount = m_vecKLine[i].amount;
				objKData.close	= m_vecKLine[i].close;
				objKData.date	= m_vecKLine[i].date;
				objKData.dayCnt = 1;
				objKData.high = m_vecKLine[i].high;
				objKData.low = m_vecKLine[i].low;
				objKData.open = m_vecKLine[i].open;
				objKData.vol = m_vecKLine[i].vol;
			}
			else
			{
				objKData.amount += m_vecKLine[i].amount;
				//objKData.close  = m_vecKLine[i].close;
				//objKData.date   = m_vecKLine[i].date;
				objKData.dayCnt++;
				objKData.high = (objKData.high > m_vecKLine[i].high) ? objKData.high: m_vecKLine[i].high;
				objKData.low = (objKData.low < m_vecKLine[i].low) ? objKData.low : m_vecKLine[i].low;
				objKData.open = m_vecKLine[i].open;
				objKData.vol += m_vecKLine[i].vol;
			}
			DEBUG("objKData.vol: "<<objKData.vol<<", i: "<<i<<", m_vecKLine[i].vol: "<<m_vecKLine[i].vol);
		}
		if (false == bFirst)
		{
			vecYearLine.push_back(objKData);
			
			if (1 == vecYearLine.size())  // 计算最值
			{
				m_iKMaxVal = objKData.high;
				m_iKMinVal = objKData.low;
				m_iVolMax = m_iVolMin = objKData.vol;
			}
			else
			{
				m_iKMaxVal = (m_iKMaxVal > objKData.high) ? m_iKMaxVal : objKData.high;
				m_iKMinVal = (m_iKMinVal < objKData.low) ? m_iKMinVal : objKData.low;
				m_iVolMax = (m_iVolMax > objKData.vol) ? m_iVolMax : objKData.vol;
				m_iVolMin = (m_iVolMin < objKData.vol) ? m_iVolMin : objKData.vol;
			}
		}
	}
	m_vecKLine = vecYearLine;
	
	return 0;

}

int CKLine::ShowResult(
	json_t *jRes,
	string &strRetCode
	)
{
	json_t *jResult = json_object();
	json_object_set_new(jResult, "kmax",   json_integer(m_iKMaxVal));	// 全局最大值
	json_object_set_new(jResult, "kmin",   json_integer(m_iKMinVal));	// 全局最小值
	json_object_set_new(jResult, "volmax", json_integer(m_iVolMax));	// 全局最大值
	json_object_set_new(jResult, "volmin", json_integer(m_iVolMin));	// 全局最小值
	
	json_t *jKLine = json_array();  // 季度曲线Y轴坐标
	for (int i=0; i< m_vecKLine.size(); i++)
	{
		json_t *jItems = json_array();
		json_array_append_new(jItems, json_integer(m_vecKLine[i].date));
		json_array_append_new(jItems, json_integer(m_vecKLine[i].open));
		json_array_append_new(jItems, json_integer(m_vecKLine[i].close));
		json_array_append_new(jItems, json_integer(m_vecKLine[i].high));
		json_array_append_new(jItems, json_integer(m_vecKLine[i].low));
		json_array_append_new(jItems, json_integer((long long)(m_vecKLine[i].amount)));
		json_array_append_new(jItems, json_integer(m_vecKLine[i].vol));
		json_array_append_new(jItems, json_integer(m_vecKLine[i].dayCnt));

		json_array_append_new(jKLine, jItems);
		//DEBUG("amount: "<<m_vecKLine[i].amount<<", vol: "<<m_vecKLine[i].vol);
	}
	json_object_set_new(jResult, "kline", jKLine);

    if (m_vecReportDate.size() > 0)  // 财报公告日期
    {
        json_t *jReportDate = json_array();
        for (int i=0; i<m_vecReportDate.size(); i++)
        {
            json_array_append_new(jReportDate, json_integer(m_vecReportDate[i]));
        }
        json_object_set_new(jResult, "reportdate", jReportDate);
    }
	
	json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	json_object_set_new(jRes, "result", jResult);

    return 0;
}
/*
// 显示信息
vector<CKData> m_vecKLine;
vector<CTotalCnt> m_vecTotalCnt;
int m_iMaxVal;
int m_iMinVal;
*/
