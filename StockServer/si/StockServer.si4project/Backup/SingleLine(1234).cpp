#include "OutInfo.h"
#include "Log.h"
#include "Constant.h"
#include "SingleLine.h"

struct SOneEarn
{
	int iDeclarationDate;
	int iCloseDate;
	int iOneEarn;
	int iOneAsset;
	int iEquity;
	int iYearProfit;
    int iYearROP;
};

CSingleLine::CSingleLine()
{
	m_ptrStock = NULL;
	m_vecQuartReport.clear();
	m_vecDayInfo.clear();
	m_iDate       = -1;
	m_iBeforeCnt  = -1;
	m_iAfterCnt   = -1;
	
	m_idxQBegin   = -1;
	m_idxQEnd     = -1;
	m_idxDBegin   = -1;
	m_idxDEnd     = -1;

	m_mapDYVal.clear();
	m_mapQYVal.clear();
	m_vecQXVal.clear();
	m_vecIdx.clear();
	m_vecDXVal.clear();
	m_iMaxYVal = 0;
	m_iMinYVal = 0;
}

CSingleLine::~CSingleLine()
{
	map<string, CYVal*>::iterator iter;

    for (iter = m_mapQYVal.begin(); iter != m_mapQYVal.end(); iter++)
	{
	    delete iter->second;
	}
	for (iter = m_mapDYVal.begin(); iter != m_mapDYVal.end(); iter++)
	{
	    delete iter->second;
	}
	
}

int CSingleLine::GetLines(    // 得到单条或多条曲线
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
    size_t sJsonSize = 0;
	string strCode = "";
	int iDate = 0;
	int iBeforeCnt = 0;
	int iAfterCnt  = 0;
	vector<string> vecItems;
	map<string, StockData*>::iterator iter;
	StockData *ptrStock = NULL;

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

	json_t *jItem = json_object_get(jReq, "item");
    if (!jItem || !json_is_array(jItem)) {
        strRetCode = CErrJsonType;
		ERROR("GetListAll Get json item error. strRetCode: "<<strRetCode);
		return 1;
    }
	sJsonSize = json_array_size(jItem);
	if (0 == sJsonSize)
	{
        strRetCode = CErrJsonType;
		ERROR("GetListAll Get json item error. strRetCode: "<<strRetCode);
		return 1;
	}
	for (int i=0; i<sJsonSize; i++)
	{
	    vecItems.push_back(json_string_value(json_array_get(jItem, i)));
	}

	// 从stockmap中找到满足条件的元素
	iter = CStockMap::Instance()->m_mapStock.find(strCode);
	if (iter == CStockMap::Instance()->m_mapStock.end())
	{
	    strRetCode = CErrOutOfRange;
	    ERROR("GetLines strCode not found. strCode: "<<strCode<<", strRetCode: "<<strRetCode);
		return 1;
	}

    // 初始化stock数据
    if (0 != Init(iter->second))
	{
	    strRetCode = CErrItemNone;
	    ERROR("Call Init error. strCode: "<<strCode<<", strRetCode: "<<strRetCode);
		return 1;
	}

    // 确定显示的区间范围
	if (0 != InfirmShowIdx(iDate, iBeforeCnt, iAfterCnt, strRetCode))
	{
	    ERROR("Call InfirmShowIdx error. strRetCode: "<<strRetCode<<", strCode: "<<strCode<<", iDate: "
			<<iDate<<", iBeforeCnt: "<<iBeforeCnt<<", iAfterCnt: "<<iAfterCnt);
		return 1;
	}

    // 生成曲线坐标
    if (0 != CreateLine(vecItems, strRetCode))
	{
	    ERROR("Call CreateLine error. strRetCode: "<<strRetCode<<", strCode: "<<strCode<<", iDate: "
			<<iDate<<", iBeforeCnt: "<<iBeforeCnt<<", iAfterCnt: "<<iAfterCnt);
		return 1;
	}
	
	// 生成返回的信息
	json_object_set_new(jRes, "method", json_string("single.line"));
	if (0 != ShowResult(jRes, strRetCode))
	{
	    ERROR("Call ShowResult error. strRetCode: "<<strRetCode);
		return 1;
	}
	
    return 0;

}

int CSingleLine::Init(
	StockData *ptrStock
	)
{
    int iRet = 0;
    if (NULL == ptrStock)
	{
	    ERROR("CSingleLine::Init NULL == ptrStock.");
	    return 1;
	}
    m_ptrStock = ptrStock;
	
	iRet = GetQuartReport();
	if (0 != iRet)
	{
	    ERROR("Call GetQuartReport error. m_strCode: "<<ptrStock->m_strCode);
		return iRet;
	}
	iRet = GetDayInfo();
	if (0 != iRet)
	{
	    ERROR("Call GetDayInfo error. m_strCode: "<<ptrStock->m_strCode);
		return iRet;
	}

	/*iRet = GetMapIndex();
	if (0 != iRet)
	{
		ERROR("GetMapIndex error. m_strCode: "<<ptrStock->m_strCode);
		return iRet;
	}*/

    return iRet;
}

int CSingleLine::GetQuartReport()
{
	m_vecQuartReport.clear();

	ReportData objReport1;
	ReportData objReport2;

	for (int i=0; i<m_ptrStock->m_vecQuart.size(); i++)
	{
	    for (int j=4; j>0; j--)
    	{
    	    objReport1 = m_ptrStock->m_vecQuart[i].m_rQuart[j-1];
			if (objReport1.m_strCode != "")
			{
			    m_vecQuartReport.push_back(objReport1);
			    DEBUG("m_ptrStock->m_vecQuart year: "<<m_ptrStock->m_vecQuart[i].m_strYear<<", i: "<<i<<", quart "<<j);
			}
			else
			{
			    DEBUG("code empty. m_ptrStock->m_vecQuart year: "<<m_ptrStock->m_vecQuart[i].m_strYear<<", i: "<<i<<", quart "<<j);
				if (m_vecQuartReport.size() > 0)
				{
					m_vecQuartReport.push_back(objReport1);
				}
			}
    	}
		
		
		/*// 处理四季度到二季度信息
		for (int j=3; j>0; j--)
		{
			objReport1 = m_ptrStock->m_vecQuart[i].m_rQuart[j-1];
			objReport2 = m_ptrStock->m_vecQuart[i].m_rQuart[j];
			if (objReport2.m_strCode != "")
			{
				if (objReport1.m_strCode != "")
				{
					objReport2.m_iEarnOne1 -= objReport1.m_iEarnOne1;
					objReport2.m_iEarnOne2 -= objReport1.m_iEarnOne2;
					objReport2.m_iRevenue -= objReport1.m_iRevenue;
					objReport2.m_iNetProfit -= objReport1.m_iNetProfit;
				}
				else
				{
					objReport2.m_iEarnOne1	= objReport2.m_iEarnOne1/4;
					objReport2.m_iEarnOne2	= objReport2.m_iEarnOne2/4;
					objReport2.m_iRevenue	= objReport2.m_iRevenue/4;
					objReport2.m_iNetProfit = objReport2.m_iNetProfit/4;
				}
				m_vecQuartReport.push_back(objReport2);
			}
			else
			{
				if (m_vecQuartReport.size() > 0)
				{
					m_vecQuartReport.push_back(objReport2);
				}
				DEBUG("m_ptrStock->m_vecQuart year: "<<m_ptrStock->m_vecQuart[i].m_strYear<<", i: "<<i<<", quart "<<j);
			}
		}

		// 处理一季度信息
		objReport1 = m_ptrStock->m_vecQuart[i].m_rQuart[0];
		if (objReport1.m_strCode != "")
		{
			m_vecQuartReport.push_back(objReport1);
		}
		else
		{
			if (m_vecQuartReport.size() > 0)
			{
				m_vecQuartReport.push_back(objReport1);
			}
			DEBUG("m_ptrStock->m_vecQuart year: "<<m_ptrStock->m_vecQuart[i].m_strYear<<", i: "<<i<<", quart 1");
		}*/
	}

	for (int i=m_vecQuartReport.size()-1; i>=0; )
	{
		if (m_vecQuartReport[i].m_strCode == "")
		{
			m_vecQuartReport.pop_back();
			i = m_vecQuartReport.size()-1;
		}
		else
		{
			break;
		}
	}

	return 0;
}

int CSingleLine::GetDayInfo()
{
	vector<SOneEarn> vecOneEarn;
	vecOneEarn.clear();
	SOneEarn objOneEarn;
	ReportData *ptrReport = NULL;
	for (int i=0; i<m_ptrStock->m_vecQuart.size(); i++)  // 得到日期列表及对应每股年收益
	{
		for (int j=3; j>=0; j--)
		{
			if (m_ptrStock->m_vecQuart[i].m_rQuart[j].m_strCode.empty())
			{
				continue;
			}
			ptrReport = &(m_ptrStock->m_vecQuart[i].m_rQuart[j]);
			if ((NULL != ptrReport->m_ptrDivide) && (ptrReport->m_ptrDivide->m_iSzzbl > 0))
			{
				objOneEarn.iDeclarationDate = DateStr2Int(ptrReport->m_ptrDivide->m_strCqcxr);
				objOneEarn.iCloseDate		= objOneEarn.iDeclarationDate;
				objOneEarn.iOneEarn 		= ptrReport->m_iOneEarnYear*100/(100+ptrReport->m_ptrDivide->m_iSzzbl);
				objOneEarn.iOneAsset		= ptrReport->m_iNetAssertOne*100/(100+ptrReport->m_ptrDivide->m_iSzzbl);
				objOneEarn.iEquity          = ptrReport->m_iEquity*(100+ptrReport->m_ptrDivide->m_iSzzbl)/100;
				objOneEarn.iYearProfit      = ptrReport->m_iYearProfit;
                objOneEarn.iYearROP         = ptrReport->m_iYearROP;;
				vecOneEarn.push_back(objOneEarn);
			}
			objOneEarn.iDeclarationDate = DateStr2Int(ptrReport->m_strDeclarationDate);
			objOneEarn.iCloseDate		= DateStr2Int(ptrReport->m_strClosingDate);
			objOneEarn.iOneEarn 		= ptrReport->m_iOneEarnYear;
			objOneEarn.iOneAsset		= ptrReport->m_iNetAssertOne;
			objOneEarn.iEquity          = ptrReport->m_iEquity;
			objOneEarn.iYearProfit      = ptrReport->m_iYearProfit;
            objOneEarn.iYearROP         = ptrReport->m_iYearROP;
			vecOneEarn.push_back(objOneEarn);
		}
	}

	m_vecDayInfo.clear();
	CDayPInfo objDayPInfo;
	for (int i=0; i<m_ptrStock->m_vecDayData.size(); i++)
	{
		objDayPInfo.date   = m_ptrStock->m_vecDayData[i].date;
		objDayPInfo.close  = m_ptrStock->m_vecDayData[i].close;
		m_vecDayInfo.push_back(objDayPInfo);
	}

	int j=0;
	int t=0;
	for (int i=0; i<vecOneEarn.size(); i++)  // 计算pe及pb
	{
		//DEBUG("vecOneEarn[i].iOneEarn: "<<vecOneEarn[i].iOneEarn<<", vecOneEarn[i].iOneAsset: "<<vecOneEarn[i].iOneAsset
		//	<<", vecOneEarn[i].iDate: "<<vecOneEarn[i].iDate);
		if ((0 == vecOneEarn[i].iDeclarationDate) || (0 == vecOneEarn[i].iCloseDate))
		{
			continue;
		}
		for (; (j< m_vecDayInfo.size()) && (m_vecDayInfo[j].date>vecOneEarn[i].iDeclarationDate); j++)
		{
			m_vecDayInfo[j].pe = (vecOneEarn[i].iOneEarn != 0) ? 100*m_vecDayInfo[j].close/vecOneEarn[i].iOneEarn : 0;
			if (m_vecDayInfo[j].pe>10000) m_vecDayInfo[j].pe = 10000;
			if (m_vecDayInfo[j].pe<-10000) m_vecDayInfo[j].pe = -10000;

			m_vecDayInfo[j].pb = (vecOneEarn[i].iOneAsset != 0) ? 100*m_vecDayInfo[j].close/vecOneEarn[i].iOneAsset : 0;
			if (m_vecDayInfo[j].pb>5000) m_vecDayInfo[j].pb = 5000;
			if (m_vecDayInfo[j].pb<-5000) m_vecDayInfo[j].pb = -5000;

			m_vecDayInfo[j].pr = (0 == vecOneEarn[i].iOneEarn) ? 0 : (m_vecDayInfo[j].close*vecOneEarn[i].iOneAsset)/(vecOneEarn[i].iOneEarn*vecOneEarn[i].iOneEarn);
			if ((0 > vecOneEarn[i].iOneEarn) && (0 < m_vecDayInfo[j].pr)) m_vecDayInfo[j].pr = m_vecDayInfo[j].pr - 2*m_vecDayInfo[j].pr;
            if (m_vecDayInfo[j].pr>500) m_vecDayInfo[j].pr = 500;
			if (m_vecDayInfo[j].pr<-200) m_vecDayInfo[j].pr = -200;

			m_vecDayInfo[j].totalValue = vecOneEarn[i].iEquity*m_vecDayInfo[j].close/100; // 市值

			m_vecDayInfo[j].covery = vecOneEarn[i].iYearProfit == 0 ? 0 : 100 * m_vecDayInfo[j].totalValue/vecOneEarn[i].iYearProfit;
			m_vecDayInfo[j].covery = m_vecDayInfo[j].covery < 0 ? -100 : m_vecDayInfo[j].covery;
			m_vecDayInfo[j].covery = m_vecDayInfo[j].covery > 10000 ? 10000 : m_vecDayInfo[j].covery;

            m_vecDayInfo[j].pp = (0 == vecOneEarn[i].iYearROP) ? 0 : 100*m_vecDayInfo[j].pe / vecOneEarn[i].iYearROP;
            if (((0 > m_vecDayInfo[j].pe) || (0 > vecOneEarn[i].iYearROP))&& (0 < m_vecDayInfo[j].pp)) m_vecDayInfo[j].pp = m_vecDayInfo[j].pp - 2*m_vecDayInfo[j].pp;
            if (m_vecDayInfo[j].pp>500) m_vecDayInfo[j].pp = 500;
			if (m_vecDayInfo[j].pp<-200) m_vecDayInfo[j].pp = -200;
		}

		for (; (t< m_vecDayInfo.size()) && (m_vecDayInfo[t].date>vecOneEarn[i].iCloseDate); t++)
		{
			m_vecDayInfo[t].pr2 = (0 == vecOneEarn[i].iOneEarn) ? 0 : (m_vecDayInfo[t].close*vecOneEarn[i].iOneAsset) / (vecOneEarn[i].iOneEarn*vecOneEarn[i].iOneEarn);
            if ((0 > vecOneEarn[i].iOneEarn) && (0 < m_vecDayInfo[t].pr2)) m_vecDayInfo[t].pr2 = m_vecDayInfo[t].pr2 - 2*m_vecDayInfo[t].pr2;
			if (m_vecDayInfo[t].pr2>500) m_vecDayInfo[t].pr2 = 500;
			if (m_vecDayInfo[t].pr2<-200) m_vecDayInfo[t].pr2 = -200;
		}
	}

	j = 0;
	for (int i=0; i<m_ptrStock->m_vecDivide.size(); i++) // 计算以业绩披露日期为基准的divide rate 
	{
		if (m_ptrStock->m_vecDivide[i].m_strYaggr.empty())
		{
			continue;
		}

		for (; (j<m_vecDayInfo.size()) && (DateInt2Str(m_vecDayInfo[j].date) > m_ptrStock->m_vecDivide[i].m_strYaggr); j++)
		{
			m_vecDayInfo[j].divideRate = 10000*m_ptrStock->m_vecDivide[i].m_iXjfh/m_vecDayInfo[j].close;
			//m_vecDayInfo[j].totalValue = m_ptrStock->m_vecDivide[i].m_iTotalEquity/100000000 * m_vecDayInfo[j].close;
			if ((DateInt2Str(m_vecDayInfo[j].date) > m_ptrStock->m_vecDivide[i].m_strCqcxr) && (m_ptrStock->m_vecDivide[i].m_iSzzbl > 0))
			{
				m_vecDayInfo[j].divideRate = m_vecDayInfo[j].divideRate*100/(100+m_ptrStock->m_vecDivide[i].m_iSzzbl);
				//m_vecDayInfo[j].totalValue = m_vecDayInfo[j].totalValue * (100+m_ptrStock->m_vecDivide[i].m_iSzzbl)/100;
			}
		}
	}

	return 0;
}

int CSingleLine::GetMapIndex() // 生成财报与股价日期的对应关系，并将财报记录与对应关系记录左对齐
{
	if (0 == m_vecDayInfo.size())
	{
		return 0;
	}
	if (0 == m_vecQuartReport.size())
	{
		return 0;
	}

	int iDayDate   = 0;
	int iQuartDate = 0;
	ReportData objReport;
	iDayDate = GetPreDate(m_vecDayInfo[0].date);
	iQuartDate = DateStr2Int(m_vecQuartReport[0].m_strClosingDate);
	if (0 == iDayDate || 0 == iQuartDate)
	{
        ERROR("GetMapIndex date error. iDayDate: "<<iDayDate<<", iQuartDate: "<<iQuartDate);
	    return 1;
	}
	iQuartDate = GetNextDate(iQuartDate);

	if (iDayDate < iQuartDate) iDayDate = iQuartDate;
	while(iDayDate > iQuartDate) // 将QuartReport开头缺少的记录补齐
	{
	    //objReport.m_iDate = iQuartDate;
		m_vecQuartReport.insert(m_vecQuartReport.begin(), objReport);
		iQuartDate = GetNextDate(iQuartDate);
	}

	int j=0;
	for (int i=0; (i<m_vecQuartReport.size()) && (m_vecQuartReport[i].m_iIdx == -1); i++) // 初始化m_vecIndex中DayData对应的下标
	{
		if (m_vecQuartReport[i].m_iDate == GetNextDate(m_vecDayInfo[j].date))
		{
			m_vecQuartReport[i].m_iIdx = j;
		}
		else if (m_vecQuartReport[i].m_iDate < GetNextDate(m_vecDayInfo[j].date))
		{
			for (; j<m_vecDayInfo.size(); j++)
			{
				if (m_vecQuartReport[i].m_iDate >= m_vecDayInfo[j].date)
				{
					m_vecQuartReport[i].m_iIdx = j;
					break;
				}
			}
			if (j >= m_vecDayInfo.size())
			{
				break;
			}
		}
		DEBUG("i: "<<i<<", iDate: "<<m_vecQuartReport[i].m_iDate<<", iReportIdx: "<<i<<", m_iIdx: "<<m_vecQuartReport[i].m_iIdx);
	}

/*	for (int i=0; i<m_vecQuartReport.size(); i++)
	{
		DEBUG("i: "<<i<<", iDate: "<<m_vecQuartReport[i].m_iDate<<", iReportIdx: "<<i<<", m_iIdx: "<<m_vecQuartReport[i].m_iIdx);
	}
*/
	return 0;
}

int CSingleLine::InfirmShowIdx( // 确定显示区域坐标
	int iDate,
	int iBeforeCnt,
	int iAfterCnt,
	string &strRetCode
	)
{
    int i,j,t;
    int iQDate;
	
	if (0 == m_vecQuartReport.size())
	{
	    strRetCode = CErrCount;
		ERROR("0 == m_vecQuartReport.size(). iDate: "<<iDate<<", strCode: "<<m_ptrStock->m_strCode<<", strRetCode: "<<strRetCode);
	    return 1;
	}

    m_iDate       = iDate;
	m_iBeforeCnt  = iBeforeCnt;
	m_iAfterCnt   = iAfterCnt;
	
    if (0 != m_iDate)
    {
		iQDate = GetPreDate(iDate);
		i = 0;
		j = m_vecQuartReport.size()-1;
		t = 0;
		while (i < j) // 二分查找
		{
		    t = (i+j)/2;
			if (iQDate == m_vecQuartReport[t].m_iDate)
			{
				break;
			}
			else if (iQDate < m_vecQuartReport[t].m_iDate)
			{
			    i = t+1;
			}
			else
			{
			    j = t-1;
			}
		}
		DEBUG("iDate: "<<iDate<<", iQDate: "<<iQDate);
    }
	else
	{
	    t = 0;
	}

	// 财报显示区段下标
	m_idxQBegin = t - m_iAfterCnt;
	if (0 > m_idxQBegin)
	{
	    m_idxQBegin = 0;
	}
	m_idxQEnd = t + m_iBeforeCnt;
	if ((m_idxQEnd >= m_vecQuartReport.size()) || (m_iBeforeCnt < 0))
	{
	    m_idxQEnd = m_vecQuartReport.size() - 1;
	}

	// 价格显示区段下标
	if (0 == m_vecDayInfo.size())
	{
	    m_idxDEnd = m_idxDBegin = -1;
		return 0;
	}
	m_idxDEnd = m_vecQuartReport[m_idxQEnd].m_iIdx;
	if (0 > m_idxDEnd)
	{
	    m_idxDEnd = m_vecDayInfo.size() - 1;
	}
	m_idxDBegin = m_vecQuartReport[m_idxQBegin].m_iIdx;
	if ((0 > m_idxDBegin) || (0 == m_iDate))
	{
	    m_idxDBegin = 0;
	}
	while ((m_idxDBegin > 0) && (m_vecDayInfo[m_idxDBegin].date < m_iDate))
	{
	    m_idxDBegin--;
	}
	DEBUG("m_idxQBegin: "<<m_idxQBegin<<", m_idxQEnd: "<<m_idxQEnd<<". m_idxDBegin: "
	    <<m_idxDBegin<<", m_idxDEnd: "<<m_idxDEnd);
	
    return 0;
}

int CSingleLine::CreateLine(
    const vector<string> &vecItems, 
    string &strRetCode
    )
{
    int iRet = 0;
	int iMax = 0;
	int iMin = 0;
	
	// 生成y坐标
	for (int i=0; i<vecItems.size(); i++)
	{
		if (1 != COutInfo::Instance()->m_mapSLine.count(vecItems[i]))
		{
		    ERROR("Not support this line. line: "<<vecItems[i]);
			continue;
		}
	
		iRet = (this->*(COutInfo::Instance()->m_mapSLine[vecItems[i]]))(iMax, iMin, strRetCode);
		if (0 != iRet)
		{
			ERROR("Call m_mapSLine error. line: "<<vecItems[i]<<", strCode: "<<m_ptrStock->m_strCode
				<<", strRetCode: "<<strRetCode);
			return 1;
		}

		if (0 == i)
		{
		    m_iMaxYVal = iMax;
			m_iMinYVal = iMin;
		}
		else
		{
		    m_iMaxYVal = (m_iMaxYVal>iMax ? m_iMaxYVal : iMax);
			m_iMinYVal = (m_iMinYVal<iMin ? m_iMinYVal : iMin);
		}
	}

	iRet = AdjustYVal(strRetCode);
	if (0 != iRet)
	{
	    ERROR("Call AdjustYVal error. strRetCode: "<<strRetCode);
		return 1;
	}

	iRet = CreateXLine(strRetCode);
	if (0 != iRet)
	{
	    ERROR("Call CreateXLine error. strRetCode: "<<strRetCode);
		return 1;
	}

    return 0;
}

int CSingleLine::GetRevenueYLine(int &iMax, int &iMin, string &strRetCode)
{
    if ((1 == m_mapQYVal.count("Revenue")) || (m_vecQuartReport.size() <= m_idxQBegin))
	{
	    return 0;
	}

	int iTemp = 0;
    iMax = iMin = m_vecQuartReport[m_idxQBegin].m_iRevenue;
	CYVal * ptrYVal = new CYVal;
	for (int i=m_idxQBegin; i<=m_idxQEnd; i++)
	{
	    iTemp = m_vecQuartReport[i].m_iRevenue;
	    ptrYVal->m_vecItems.push_back(iTemp);
		iMax = (iMax>iTemp ? iMax : iTemp);
		iMin = (iMin<iTemp ? iMin : iTemp);
		DEBUG("Revenue m_vecQuartReport[i].m_iRevenue: "<<m_vecQuartReport[i].m_iRevenue<<", iTemp: "<<iTemp
			<<", m_strClosingDate: "<<m_vecQuartReport[i].m_strClosingDate<<", code: "<<m_vecQuartReport[i].m_strCode
			<<", i: "<<i);
	}
	ptrYVal->m_iMax = iMax;
	ptrYVal->m_iMin = iMin;
	m_mapQYVal["Revenue"] = ptrYVal;
	
    return 0;
}

int CSingleLine::GetProfitYLine(int &iMax, int &iMin, string &strRetCode)
{
    if ((1 == m_mapQYVal.count("Profit")) || (m_vecQuartReport.size() <= m_idxQBegin))
	{
	    return 0;
	}

	int iTemp = 0;
    iMax = iMin = m_vecQuartReport[m_idxQBegin].m_iNetProfit;
	CYVal * ptrYVal = new CYVal;
	for (int i=m_idxQBegin; i<=m_idxQEnd; i++)
	{
	    iTemp = m_vecQuartReport[i].m_iNetProfit;
	    ptrYVal->m_vecItems.push_back(iTemp);
		iMax = (iMax>iTemp ? iMax : iTemp);
		iMin = (iMin<iTemp ? iMin : iTemp);
		DEBUG("Profit m_vecQuartReport[i].m_iNetProfit: "<<m_vecQuartReport[i].m_iNetProfit<<", iTemp: "<<iTemp
			<<", m_strClosingDate: "<<m_vecQuartReport[i].m_strClosingDate<<", code: "<<m_vecQuartReport[i].m_strCode
			<<", i: "<<i);
	}
	ptrYVal->m_iMax = iMax;
	ptrYVal->m_iMin = iMin;
	m_mapQYVal["Profit"] = ptrYVal;

    return 0;
}

int CSingleLine::GetROEYLine(int &iMax, int &iMin, string &strRetCode)
{
    if ((1 == m_mapQYVal.count("ROE")) || (m_vecQuartReport.size() <= m_idxQBegin))
	{
	    return 0;
	}

	int iTemp = 0;
    iMax = iMin = m_vecQuartReport[m_idxQBegin].m_iNetAssertRate;
	CYVal * ptrYVal = new CYVal;
	for (int i=m_idxQBegin; i<=m_idxQEnd; i++)
	{
	    iTemp = m_vecQuartReport[i].m_iNetAssertRate;
	    ptrYVal->m_vecItems.push_back(iTemp);
		iMax = (iMax>iTemp ? iMax : iTemp);
		iMin = (iMin<iTemp ? iMin : iTemp);
		DEBUG("ROE m_vecQuartReport[i].m_iNetAssertRate: "<<m_vecQuartReport[i].m_iNetAssertRate<<", iTemp: "<<iTemp
			<<", m_strClosingDate: "<<m_vecQuartReport[i].m_strClosingDate<<", code: "<<m_vecQuartReport[i].m_strCode
			<<", i: "<<i);
	}
	ptrYVal->m_iMax = iMax;
	ptrYVal->m_iMin = iMin;
	m_mapQYVal["ROE"] = ptrYVal;

    return 0;

}

int CSingleLine::GetDivRateYLine(int &iMax, int &iMin, string &strRetCode)
{
    if ((1 == m_mapQYVal.count("DivRate")) || (m_vecQuartReport.size() <= m_idxQBegin))
	{
	    return 0;
	}

	int iTemp = 0;
    iMax = iMin = m_vecQuartReport[m_idxQBegin].m_iDividentRate;
	CYVal * ptrYVal = new CYVal;
	for (int i=m_idxQBegin; i<=m_idxQEnd; i++)
	{
	    iTemp = m_vecQuartReport[i].m_iDividentRate;
	    ptrYVal->m_vecItems.push_back(iTemp);
		iMax = (iMax>iTemp ? iMax : iTemp);
		iMin = (iMin<iTemp ? iMin : iTemp);
		DEBUG("DivRate m_vecQuartReport[i].m_iDividentRate: "<<m_vecQuartReport[i].m_iDividentRate<<", iTemp: "<<iTemp
			<<", m_strClosingDate: "<<m_vecQuartReport[i].m_strClosingDate<<", code: "<<m_vecQuartReport[i].m_strCode
			<<", i: "<<i);
	}
	ptrYVal->m_iMax = iMax;
	ptrYVal->m_iMin = iMin;
	m_mapQYVal["DivRate"] = ptrYVal;
	
    return 0;

}

int CSingleLine::GetPRYLine(int &iMax, int &iMin, string &strRetCode)
{
    if ((1 == m_mapDYVal.count("DayPR")) || (m_idxDBegin >= m_vecDayInfo.size()))
	{
	    return 0;
	}

	int iTemp = 0;
    iMax = iMin = m_vecDayInfo[m_idxDBegin].pr;
	CYVal * ptrYVal = new CYVal;
	for (int i=m_idxDBegin; i<=m_idxDEnd; i++)
	{
	    iTemp = m_vecDayInfo[i].pr;
	    ptrYVal->m_vecItems.push_back(iTemp);
		iMax = (iMax>iTemp ? iMax : iTemp);
		iMin = (iMin<iTemp ? iMin : iTemp);
		//DEBUG("DayPR m_vecDayInfo[i].pr: "<<m_vecDayInfo[i].pr<<", iTemp: "<<iTemp
		//	<<", date: "<<m_vecDayInfo[i].date<<", code: "<<m_ptrStock->m_strCode);
	}
	ptrYVal->m_iMax = iMax;
	ptrYVal->m_iMin = iMin;
	m_mapDYVal["DayPR"] = ptrYVal;
	
    return 0;

}

int CSingleLine::GetPR2YLine(int &iMax, int &iMin, string &strRetCode)
{
    if ((1 == m_mapDYVal.count("DayPR2")) || (m_idxDBegin >= m_vecDayInfo.size()))
	{
	    return 0;
	}

	int iTemp = 0;
    iMax = iMin = m_vecDayInfo[m_idxDBegin].pr2;
	CYVal * ptrYVal = new CYVal;
	for (int i=m_idxDBegin; i<=m_idxDEnd; i++)
	{
	    iTemp = m_vecDayInfo[i].pr2;
	    ptrYVal->m_vecItems.push_back(iTemp);
		iMax = (iMax>iTemp ? iMax : iTemp);
		iMin = (iMin<iTemp ? iMin : iTemp);
		//DEBUG("DayPR m_vecDayInfo[i].pr: "<<m_vecDayInfo[i].pr<<", iTemp: "<<iTemp
		//	<<", date: "<<m_vecDayInfo[i].date<<", code: "<<m_ptrStock->m_strCode);
	}
	ptrYVal->m_iMax = iMax;
	ptrYVal->m_iMin = iMin;
	m_mapDYVal["DayPR2"] = ptrYVal;
	
    return 0;
}

int CSingleLine::GetPriceYLine(int &iMax, int &iMin, string &strRetCode)
{
    if ((1 == m_mapDYVal.count("DayPrice")) || (m_idxDBegin >= m_vecDayInfo.size()))
	{
	    return 0;
	}

	int iTemp = 0;
    iMax = iMin = m_vecDayInfo[m_idxDBegin].close;
	CYVal * ptrYVal = new CYVal;
	for (int i=m_idxDBegin; i<=m_idxDEnd; i++)
	{
	    iTemp = m_vecDayInfo[i].close;
	    ptrYVal->m_vecItems.push_back(iTemp);
		iMax = (iMax>iTemp ? iMax : iTemp);
		iMin = (iMin<iTemp ? iMin : iTemp);
		//DEBUG("DayPR m_vecDayInfo[i].pr: "<<m_vecDayInfo[i].pr<<", iTemp: "<<iTemp
		//	<<", date: "<<m_vecDayInfo[i].date<<", code: "<<m_ptrStock->m_strCode);
	}
	ptrYVal->m_iMax = iMax;
	ptrYVal->m_iMin = iMin;
	m_mapDYVal["DayPrice"] = ptrYVal;
	
    return 0;
}

int CSingleLine::GetReinPriceYLine(int &iMax, int &iMin, string &strRetCode)
{
    if ((1 == m_mapDYVal.count("DayReinPrice")) || (m_idxDBegin >= m_vecDayInfo.size()))
	{
	    return 0;
	}

	int iTemp = 0;
    iMax = iMin = m_vecDayInfo[m_idxDBegin].close;
	CYVal * ptrYVal = new CYVal;

	// 查找范围内的除权信息
	vector<CCqcx> vecCqcx;
    int iBeginDate = m_vecDayInfo[m_idxDBegin].date;
	int iEndDate = m_vecDayInfo[m_idxDEnd].date;
	vecCqcx.clear();
	for (int i=0; i<m_ptrStock->m_vecDivide.size(); i++)
	{
	    CCqcx objCqcx;
		objCqcx.iSzzbl = m_ptrStock->m_vecDivide[i].m_iSzzbl;
		objCqcx.iDate = DateStr2Int(m_ptrStock->m_vecDivide[i].m_strCqcxr);
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
		vecCqcx.push_back(objCqcx);
	}

    // 除权除息复权
    int j=0;
	int iszzbl = 100;
	iMax = iMin = m_vecDayInfo[m_idxDBegin].close;
	for (int i=m_idxDBegin; i<= m_idxDEnd; i++)
	{
		if ((j < vecCqcx.size()) && (m_vecDayInfo[i].date <= vecCqcx[j].iDate))
		{
		    DEBUG("iszzbl: "<<iszzbl);
		    iszzbl = iszzbl*(100 + vecCqcx[j].iSzzbl)/100;
			DEBUG("iszzbl: "<<iszzbl<<", vecCqcx[j].iSzzbl: "<<vecCqcx[j].iSzzbl);
			j++;
		}
		iTemp = m_vecDayInfo[i].close*100/iszzbl;
		ptrYVal->m_vecItems.push_back(iTemp);
		iMax = (iMax>iTemp ? iMax : iTemp);
		iMin = (iMin<iTemp ? iMin : iTemp);
	}
	ptrYVal->m_iMax = iMax;
	ptrYVal->m_iMin = iMin;

	m_mapDYVal["DayReinPrice"] = ptrYVal;
	
    return 0;

}

int CSingleLine::GetTotalValYLine(int &iMax, int &iMin, string &strRetCode)
{
    if ((1 == m_mapQYVal.count("DayValue")) || (m_idxDBegin >= m_vecDayInfo.size()))
	{
	    return 0;
	}

	int iTemp = 0;
    iMax = iMin = m_vecDayInfo[m_idxDBegin].totalValue;
	CYVal * ptrYVal = new CYVal;
	for (int i=m_idxDBegin; i<=m_idxDEnd; i++)
	{
	    iTemp = m_vecDayInfo[i].totalValue;
	    ptrYVal->m_vecItems.push_back(iTemp);
		iMax = (iMax>iTemp ? iMax : iTemp);
		iMin = (iMin<iTemp ? iMin : iTemp);
		//DEBUG("DayPR m_vecDayInfo[i].pr: "<<m_vecDayInfo[i].pr<<", iTemp: "<<iTemp
		//	<<", date: "<<m_vecDayInfo[i].date<<", code: "<<m_ptrStock->m_strCode);
	}
	ptrYVal->m_iMax = iMax;
	ptrYVal->m_iMin = iMin;
	m_mapDYVal["DayValue"] = ptrYVal;
	
    return 0;
}

int CSingleLine::GetCoveryYLine(int &iMax, int &iMin, string &strRetCode)
{
    if ((1 == m_mapDYVal.count("DayCovery")) || (m_idxDBegin >= m_vecDayInfo.size()))
	{
	    return 0;
	}

	int iTemp = 0;
    iMax = iMin = m_vecDayInfo[m_idxDBegin].covery;
	CYVal * ptrYVal = new CYVal;
	for (int i=m_idxDBegin; i<=m_idxDEnd; i++)
	{
	    iTemp = m_vecDayInfo[i].covery;
	    ptrYVal->m_vecItems.push_back(iTemp);
		iMax = (iMax>iTemp ? iMax : iTemp);
		iMin = (iMin<iTemp ? iMin : iTemp);
		//DEBUG("DayPR m_vecDayInfo[i].pr: "<<m_vecDayInfo[i].pr<<", iTemp: "<<iTemp
		//	<<", date: "<<m_vecDayInfo[i].date<<", code: "<<m_ptrStock->m_strCode);
	}
	ptrYVal->m_iMax = iMax;
	ptrYVal->m_iMin = iMin;
	m_mapDYVal["DayCovery"] = ptrYVal;
	
    return 0;
}

int CSingleLine::GetROPYLine(int &iMax, int &iMin, string &strRetCode)
{
    if ((1 == m_mapQYVal.count("ROP")) || (m_vecQuartReport.size() <= m_idxQBegin))
	{
	    return 0;
	}

	int iTemp = 0;
    iMax = iMin = m_vecQuartReport[m_idxQBegin].m_iROP;
	CYVal * ptrYVal = new CYVal;
	for (int i=m_idxQBegin; i<=m_idxQEnd; i++)
	{
	    iTemp = m_vecQuartReport[i].m_iROP;
	    ptrYVal->m_vecItems.push_back(iTemp);
		iMax = (iMax>iTemp ? iMax : iTemp);
		iMin = (iMin<iTemp ? iMin : iTemp);
		DEBUG("Revenue m_vecQuartReport[i].m_iROP: "<<m_vecQuartReport[i].m_iROP<<", iTemp: "<<iTemp
			<<", m_strClosingDate: "<<m_vecQuartReport[i].m_strClosingDate<<", code: "<<m_vecQuartReport[i].m_strCode
			<<", i: "<<i);
	}
	ptrYVal->m_iMax = iMax;
	ptrYVal->m_iMin = iMin;
	m_mapQYVal["ROP"] = ptrYVal;
	
    return 0;

}

int CSingleLine::GetPPYLine(int &iMax, int &iMin, string &strRetCode)
{
    if ((1 == m_mapDYVal.count("DayPP")) || (m_idxDBegin >= m_vecDayInfo.size()))
	{
	    return 0;
	}

	int iTemp = 0;
    iMax = iMin = m_vecDayInfo[m_idxDBegin].pp;
	CYVal * ptrYVal = new CYVal;
	for (int i=m_idxDBegin; i<=m_idxDEnd; i++)
	{
	    iTemp = m_vecDayInfo[i].pp;
	    ptrYVal->m_vecItems.push_back(iTemp);
		iMax = (iMax>iTemp ? iMax : iTemp);
		iMin = (iMin<iTemp ? iMin : iTemp);
		//DEBUG("DayPR m_vecDayInfo[i].pr: "<<m_vecDayInfo[i].pr<<", iTemp: "<<iTemp
		//	<<", date: "<<m_vecDayInfo[i].date<<", code: "<<m_ptrStock->m_strCode);
	}
	ptrYVal->m_iMax = iMax;
	ptrYVal->m_iMin = iMin;
	m_mapDYVal["DayPP"] = ptrYVal;
	
    return 0;

}

int CSingleLine::AdjustYVal(string &strRetCode)
{
    double dTimes = 1;
	double dTemp = 0;
    if (1 == m_mapDYVal.count("DayPR"))
	{
	    dTemp = (m_mapDYVal["DayPR"]->m_iMin == 0) ? 0 : 1.0*m_mapDYVal["DayPR"]->m_iMax / m_mapDYVal["DayPR"]->m_iMin;
		DEBUG("AdjustYVal DayPR: "<<dTemp);
		if ((dTemp > 100) || (dTemp <= 0))
		{
		    return 0;
		}
		dTimes = dTimes > dTemp ? dTimes : dTemp;
	}
    if (1 == m_mapDYVal.count("DayReinPrice"))
	{
	    dTemp = (m_mapDYVal["DayReinPrice"]->m_iMin == 0) ? 0 : 1.0*m_mapDYVal["DayReinPrice"]->m_iMax / m_mapDYVal["DayReinPrice"]->m_iMin;
		DEBUG("AdjustYVal DayReinPrice: "<<dTemp);
		if ((dTemp > 100) || (dTemp <= 0))
		{
		    return 0;
		}
		dTimes = dTimes > dTemp ? dTimes : dTemp;
	}
    if (1 == m_mapDYVal.count("DayCovery"))
	{
	    dTemp = (m_mapDYVal["DayCovery"]->m_iMin == 0) ? 0 : 1.0*m_mapDYVal["DayCovery"]->m_iMax / m_mapDYVal["DayCovery"]->m_iMin;
		DEBUG("AdjustYVal DayCovery: "<<dTemp);
		if ((dTemp > 100) || (dTemp <= 0))
		{
		    return 0;
		}
		dTimes = dTimes > dTemp ? dTimes : dTemp;
	}
	DEBUG("AdjustYVal dTimes: "<<dTimes);
	if (dTimes < 1.5)
	{
	    return 0;
	}

    if (1 == m_mapDYVal.count("DayPR"))
	{
	    dTemp =  1.0*m_mapDYVal["DayPR"]->m_iMax / m_mapDYVal["DayPR"]->m_iMin;
		m_mapDYVal["DayPR"]->m_iMax = m_mapDYVal["DayPR"]->m_iMax*dTemp/dTimes;
	}
    if (1 == m_mapDYVal.count("DayReinPrice"))
	{
	    dTemp =  1.0*m_mapDYVal["DayReinPrice"]->m_iMax / m_mapDYVal["DayReinPrice"]->m_iMin;
		m_mapDYVal["DayReinPrice"]->m_iMax = m_mapDYVal["DayReinPrice"]->m_iMax*dTemp/dTimes;
	}
    if (1 == m_mapDYVal.count("DayCovery"))
	{
	    dTemp =  1.0*m_mapDYVal["DayCovery"]->m_iMax / m_mapDYVal["DayCovery"]->m_iMin;
		m_mapDYVal["DayCovery"]->m_iMax = m_mapDYVal["DayCovery"]->m_iMax*dTemp/dTimes;
	}
	DEBUG("execute AdjustYVal");
	
    return 0;
}

int CSingleLine::CreateXLine(string &strRetCode) // x坐标的值就是日期
{
    m_vecQXVal.clear();
	m_vecIdx.clear();
	m_vecDXVal.clear();
	int idx = 0;
	
	for (int i=m_idxQBegin; i<=m_idxQEnd; i++)
	{
	    m_vecQXVal.push_back(m_vecQuartReport[i].m_iDate);
		//DEBUG("strClosingData: "<<m_vecQuartReport[i].m_strClosingDate<<", iDate: "<<m_vecQuartReport[i].m_iDate);
		idx = m_vecQuartReport[i].m_iIdx - m_idxDBegin;
		if (0 > idx) 
		{
		    idx = -1;
		}
		m_vecIdx.push_back(idx);
	}

	for (int i=m_idxDBegin; i<=m_idxDEnd; i++)
	{
	    m_vecDXVal.push_back(m_vecDayInfo[i].date);
	}
	
    return 0;
}

int CSingleLine::ShowResult(
    json_t *jRes,
    string &strRetCode)
{
	map<string, CYVal*>::iterator iter;
	json_t *jResult = json_object();
    json_object_set_new(jResult, "max", json_integer(m_iMaxYVal));  // 全局最大值
    json_object_set_new(jResult, "min", json_integer(m_iMinYVal));  // 全局最小值

    json_t *jQYLine = json_array();  // 季度曲线Y轴坐标
	for (iter=m_mapQYVal.begin(); iter != m_mapQYVal.end(); iter++)
	{
	    json_t *jLine = json_object();
		json_object_set_new(jLine, "type", json_string(iter->first.c_str()));
		json_object_set_new(jLine, "max", json_integer(iter->second->m_iMax));
		json_object_set_new(jLine, "min", json_integer(iter->second->m_iMin));
		json_t *jItems = json_array();
		for (int i=0; i<iter->second->m_vecItems.size(); i++)
		{
		    json_array_append_new(jItems, json_integer(iter->second->m_vecItems[i]));
		}
		json_object_set_new(jLine, "items", jItems);
		json_array_append_new(jQYLine, jLine);
	}
	json_object_set_new(jResult, "qyline", jQYLine);

    json_t *jQXLine = json_array();  // 季度曲线X轴坐标
    for (int i=0; i<m_vecQXVal.size(); i++)
	{
	    json_array_append_new(jQXLine, json_integer(m_vecQXVal[i]));
	}
	json_object_set_new(jResult, "qxline", jQXLine);

    if (0 != m_mapDYVal.size())
    {
        json_t *jDYLine = json_array();  // 日曲线Y轴坐标
		//map<string, CYVal*>::iterator iter;
		for (iter=m_mapDYVal.begin(); iter != m_mapDYVal.end(); iter++)
		{
		    json_t *jLine = json_object();
			json_object_set_new(jLine, "type", json_string(iter->first.c_str()));
			json_object_set_new(jLine, "max", json_integer(iter->second->m_iMax));
			json_object_set_new(jLine, "min", json_integer(iter->second->m_iMin));
			json_t *jItems = json_array();
			for (int i=0; i<iter->second->m_vecItems.size(); i++)
			{
			    json_array_append_new(jItems, json_integer(iter->second->m_vecItems[i]));
			}
			json_object_set_new(jLine, "items", jItems);
			json_array_append_new(jDYLine, jLine);
		}
		json_object_set_new(jResult, "dyline", jDYLine);
	
	    json_t *jDXLine = json_array();  // 日曲线X轴坐标
	    for (int i=0; i<m_vecDXVal.size(); i++)
		{
		    json_array_append_new(jDXLine, json_integer(m_vecDXVal[i]));
		}
		json_object_set_new(jResult, "dxline", jDXLine);

		json_t *jIndex = json_array(); // 季度X轴与日X轴对应下标
		for (int i=0; i<m_vecIdx.size(); i++)
		{
		    json_array_append_new(jIndex, json_integer(m_vecIdx[i]));
		}
		json_object_set_new(jResult, "index", jIndex);
    }
	
	json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	json_object_set_new(jRes, "result", jResult);

    return 0;
}

