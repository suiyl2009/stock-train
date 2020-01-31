#include <string>
#include <sstream>
#include "Data.h"
#include "DBData.h"
#include "Log.h"
#include "StrSplit.h"
#include "TravelDir.h"

using namespace std;
	
int QuartData::GetiRate()  // 得到本年的股息率
{
	for (int i=3; i>=0; i--)
	{
		if (m_rQuart[i].m_iDividentRate>0)
			return m_rQuart[i].m_iDividentRate;
	}

	return 0;
}

int StockData::GetLastiRate(
	int idx
	)
{
	int iRate = 0;
	if (idx<m_vecQuart.size())
	{
		iRate = m_vecQuart[idx].GetiRate();
	}
	if (iRate > 1)
	{
		return iRate;
	}

	if (("" == m_vecQuart[idx].m_rQuart[3].m_strCode) && (idx+1<m_vecQuart.size()))
	{
		iRate = m_vecQuart[idx+1].GetiRate();
	}
	return iRate;
}

int StockData::GetOneEarnYear(    // 每股年收益
	int idxYear,
	int idxQuart,
	int &iOneEarnYear
	)
{
	iOneEarnYear = 0;
	if (0 == idxQuart) // 计算一季度的每股年收益
	{
		if (idxYear+1 < m_vecQuart.size())
		{
			if (0 == m_vecQuart[idxYear].m_rQuart[0].m_iNetProfit)
			{
				iOneEarnYear = 4*m_vecQuart[idxYear].m_rQuart[0].m_iEarnOne1;
			}
			else
			{
				iOneEarnYear = m_vecQuart[idxYear].m_rQuart[0].m_iEarnOne1 
					+ m_vecQuart[idxYear].m_rQuart[0].m_iEarnOne1
					*(m_vecQuart[idxYear+1].m_rQuart[3].m_iNetProfit - m_vecQuart[idxYear+1].m_rQuart[0].m_iNetProfit)
					/m_vecQuart[idxYear].m_rQuart[0].m_iNetProfit;
			}
		}
		else
		{
			iOneEarnYear = 4*m_vecQuart[idxYear].m_rQuart[0].m_iEarnOne1;
		}
		//DEBUG("m_vecQuart[idxYear].m_rQuart1.m_strDeclarationDate: "<<m_vecQuart[idxYear].m_rQuart1.m_strDeclarationDate
		//	<<", length: "<<m_vecQuart[idxYear].m_rQuart1.m_strDeclarationDate.length());
	}
	if (1 == idxQuart) // 计算二季度数据
	{
		if (idxYear+1 < m_vecQuart.size())
		{
			if (0 == m_vecQuart[idxYear].m_rQuart[1].m_iNetProfit)
			{
				iOneEarnYear = 2*m_vecQuart[idxYear].m_rQuart[1].m_iEarnOne1;
			}
			else
			{
				iOneEarnYear = m_vecQuart[idxYear].m_rQuart[1].m_iEarnOne1 
					+ m_vecQuart[idxYear].m_rQuart[1].m_iEarnOne1
					*(m_vecQuart[idxYear+1].m_rQuart[3].m_iNetProfit - m_vecQuart[idxYear+1].m_rQuart[1].m_iNetProfit)
					/m_vecQuart[idxYear].m_rQuart[1].m_iNetProfit;
			}
		}
		else
		{
			iOneEarnYear = 2*m_vecQuart[idxYear].m_rQuart[1].m_iEarnOne1;
		}
	}

	if (2 == idxQuart)  // 计算三季度数据
	{
		if (idxYear+1 < m_vecQuart.size())
		{
			if (0 == m_vecQuart[idxYear].m_rQuart[2].m_iNetProfit)
			{
				iOneEarnYear = 4*m_vecQuart[idxYear].m_rQuart[2].m_iEarnOne1/3;
			}
			else
			{
				iOneEarnYear = m_vecQuart[idxYear].m_rQuart[2].m_iEarnOne1 
					+ m_vecQuart[idxYear].m_rQuart[2].m_iEarnOne1
					*(m_vecQuart[idxYear+1].m_rQuart[3].m_iNetProfit - m_vecQuart[idxYear+1].m_rQuart[2].m_iNetProfit)
					/m_vecQuart[idxYear].m_rQuart[2].m_iNetProfit;
			}
		}
		else
		{
			iOneEarnYear = 4*m_vecQuart[idxYear].m_rQuart[2].m_iEarnOne1/3;
		}
	}

	if (3 == idxQuart)  // 计算四季度数据
	{
		iOneEarnYear = m_vecQuart[idxYear].m_rQuart[3].m_iEarnOne1;
	}

	//DEBUG("idxYear: "<<idxYear<<", idxQuart: "<<idxQuart<<", iDeclareDate: "<<iDeclareDate<<", iOneEarnYear: "<<iOneEarnYear
	//	<<", iOneAsset: "<<iOneAsset<<", iDivideDate: "<<iDivideDate<<", dSzzbl: "<<dSzzbl);
	return 0;
}

int StockData::GetYearProfit(        // 每股年收益
	int idxYear,
	int idxQuart,
	int &iYearProfit
	)
{
	iYearProfit = 0;
	if (0 == idxQuart) // 计算一季度的年收益
	{
		if (idxYear+1 < m_vecQuart.size())
		{
			iYearProfit = 
				m_vecQuart[idxYear].m_rQuart[0].m_iNetProfit 
				+ m_vecQuart[idxYear+1].m_rQuart[3].m_iNetProfit 
				- m_vecQuart[idxYear+1].m_rQuart[0].m_iNetProfit;
		}
		else
		{
			iYearProfit = 4*m_vecQuart[idxYear].m_rQuart[0].m_iNetProfit;
		}
		//DEBUG("m_vecQuart[idxYear].m_rQuart1.m_strDeclarationDate: "<<m_vecQuart[idxYear].m_rQuart1.m_strDeclarationDate
		//	<<", length: "<<m_vecQuart[idxYear].m_rQuart1.m_strDeclarationDate.length());
	}
	if (1 == idxQuart) // 计算二季度数据
	{
		if (idxYear+1 < m_vecQuart.size())
		{
			iYearProfit = 
				m_vecQuart[idxYear].m_rQuart[1].m_iNetProfit 
				+ m_vecQuart[idxYear+1].m_rQuart[3].m_iNetProfit 
				- m_vecQuart[idxYear+1].m_rQuart[1].m_iNetProfit;
		}
		else
		{
			iYearProfit = 2*m_vecQuart[idxYear].m_rQuart[1].m_iNetProfit;
		}
	}
	
	if (2 == idxQuart)	// 计算三季度数据
	{
		if (idxYear+1 < m_vecQuart.size())
		{
			iYearProfit = 
				m_vecQuart[idxYear].m_rQuart[2].m_iNetProfit 
				+ m_vecQuart[idxYear+1].m_rQuart[3].m_iNetProfit 
				- m_vecQuart[idxYear+1].m_rQuart[2].m_iNetProfit;
		}
		else
		{
			iYearProfit = 4*m_vecQuart[idxYear].m_rQuart[2].m_iNetProfit/3;
		}
	}
	
	if (3 == idxQuart)	// 计算四季度数据
	{
		iYearProfit = m_vecQuart[idxYear].m_rQuart[3].m_iNetProfit;
	}
	
	//DEBUG("idxYear: "<<idxYear<<", idxQuart: "<<idxQuart<<", iDeclareDate: "<<iDeclareDate<<", iOneEarnYear: "<<iOneEarnYear
	//	<<", iOneAsset: "<<iOneAsset<<", iDivideDate: "<<iDivideDate<<", dSzzbl: "<<dSzzbl);
	return 0;
}

int StockData::GetROP(        // 净利润增长率
	int idxYear,
	int idxQuart,
	int &iROP
	)
{
	iROP = 0;
	if (idxYear + 1 >= m_vecQuart.size())
	{
	    iROP = 0;
		return 0;
	}

    // 得到上一年净利润，如果为零，依次顺延
	int iBforeProfit = 0;
	if (m_vecQuart[idxYear+1].m_rQuart[3].m_iYearProfit > 0)
	{
	    iBforeProfit = m_vecQuart[idxYear+1].m_rQuart[3].m_iYearProfit;
	}
	else if (m_vecQuart[idxYear].m_rQuart[0].m_iYearProfit > 0)
	{
	    iBforeProfit = m_vecQuart[idxYear].m_rQuart[0].m_iYearProfit;
	}
	else if (m_vecQuart[idxYear].m_rQuart[1].m_iYearProfit > 0)
	{
	    iBforeProfit = m_vecQuart[idxYear].m_rQuart[1].m_iYearProfit;
	}
	else if (m_vecQuart[idxYear].m_rQuart[2].m_iYearProfit > 0)
	{
	    iBforeProfit = m_vecQuart[idxYear].m_rQuart[2].m_iYearProfit;
	}
	else if (m_vecQuart[idxYear].m_rQuart[3].m_iYearProfit > 0)
	{
	    iBforeProfit = m_vecQuart[idxYear].m_rQuart[3].m_iYearProfit;
	}
	else
	{
		iROP = 0;
		return 0;
	}
	
	iROP = 10000*(m_vecQuart[idxYear].m_rQuart[idxQuart].m_iNetProfit 
			- m_vecQuart[idxYear+1].m_rQuart[idxQuart].m_iNetProfit)/iBforeProfit;
	iROP = (iROP > 10000) ? 10000 : iROP;
	iROP = (iROP < -10000) ? -10000 : iROP;

	return 0;

}

int StockData::DecorateReport()
{
	int iDeclareDate = 0;
	int iOneEarnYear = 0;
	int iOneAsset = 0;
	int iDivideDate = 0;
	int iDayIdx = 0;
	int iROP = 0;
	for (int i=0; i<m_vecQuart.size(); i++) // 循环处理每个Report数据
	{
	    m_vecQuart[i].m_iYear = atoi(m_vecQuart[i].m_strYear.c_str()); // 日期由字符串转为整数
	    DEBUG("m_iYear: "<<m_vecQuart[i].m_iYear<<", m_strYear: "<<m_vecQuart[i].m_strYear);
		for (int j=3; j>=0; j--)
		{
			
			if (m_vecQuart[i].m_rQuart[j].m_strCode.empty())
			{
				continue;
			}
			
			// 日期转为整数
			m_vecQuart[i].m_rQuart[j].m_iDate = DateStr2Int(m_vecQuart[i].m_rQuart[j].m_strClosingDate.c_str());
			
			 // 计算下表,必须在上一语句m_vecQuart[i].m_rQuart[j].m_iDate赋值后才可执行
			iDayIdx = GetDayIdx(m_vecQuart[i].m_rQuart[j].m_iDate);
			if (0 > iDayIdx)
			{
			    ERROR("GetDayIdx error. date: "<<m_vecQuart[i].m_rQuart[j].m_iDate
					<<", strCode:"<<m_vecQuart[i].m_rQuart[j].m_strCode);
				return 1;
			}
			m_vecQuart[i].m_rQuart[j].m_iIdx = iDayIdx;
			m_vecQuart[i].m_rQuart[j].m_iPrice = m_vecDayData[iDayIdx].close;

			// 得到日期列表及对应每股年收益
			if (0 != GetOneEarnYear(i, j, iOneEarnYear))
			{
				ERROR("GetOneEarnYear error. i: "<<i<<", j: "<<j<<", year: "<<m_vecQuart[i].m_strYear);
				return 1;
			}
			m_vecQuart[i].m_rQuart[j].m_iOneEarnYear = iOneEarnYear;

			if (0 != GetYearProfit(i, j, iOneEarnYear))
			{
				ERROR("GetYearProfit error. i: "<<i<<", j: "<<j<<", year: "<<m_vecQuart[i].m_strYear);
				return 1;
			}
			m_vecQuart[i].m_rQuart[j].m_iYearProfit = iOneEarnYear;

			if (i > 0)
			{
			    if (0 != GetROP(i-1, j, iROP))
		    	{
		    		ERROR("GetROP error. i-1: "<<i-1<<", j: "<<j<<", year: "<<m_vecQuart[i-1].m_strYear);
				    return 1;
		    	}
				m_vecQuart[i-1].m_rQuart[j].m_iROP = iROP;
			}
		}
	}

	return 0;
}

int StockData::GetQuartIdx(    // 二分查找
	int iYear
	)
{
	int i,j,t;
	i=0;
	j=m_vecQuart.size();
	t=-1;
	while (i<j)
	{
		t = (i+j)/2;
		//DEBUG("GetQuartIdx iYear: "<<iYear<<", m_vecQuart[t].m_iYear: "<<m_vecQuart[t].m_iYear);
		if (m_vecQuart[t].m_iYear == iYear)
		{
			return t;
		}
		else if (m_vecQuart[t].m_iYear > iYear)
		{
			i = t+1;
		}
		else
		{
			j = t;
		}
	}

	return -1;
}

int StockData::GetDayIdx(    // 二分查找
	int iDate
	)
{
	if (0 >= m_vecDayData.size())
	{
		return -1;
	}

	int i,j,t;
	i=0;
	j=m_vecDayData.size();
	t=-1;
	while (i<j)
	{
		t = (i+j)/2;
		if (m_vecDayData[t].date == iDate)
		{
			return t;
		}
		else if (m_vecDayData[t].date > iDate)
		{
			i = t+1;
		}
		else
		{
			j = t;
		}
	}

	if (t < 0)
	{
	    return -1;
	}

	if ((t<m_vecDayData.size()) && (m_vecDayData[t].date<iDate))
	{
	    if ((t-1>=0) && (m_vecDayData[t-1].date>iDate))
	    {
		    return t;
	    }
	}
	if ((t+1<m_vecDayData.size()) && (m_vecDayData[t].date>iDate))
	{
		if (m_vecDayData[t+1].date<iDate)
		{
			return (t+1);
		}
	}

	return -1;
}

int StockData::GetPValue(
	int iDayIdx,
	CDayPInfo &objDayPInfo
	)
{
	if ((iDayIdx < 0) || (iDayIdx >= m_vecDayData.size()))
	{
		ERROR("iDayIdx error. iDayIdx: "<<iDayIdx<<", m_vecDayData.size(): "<<m_vecDayData.size());
		return 1;
	}

	char temp[64] = {0};
	int iClose = m_vecDayData[iDayIdx].close;
	int iDate = m_vecDayData[iDayIdx].date;
	objDayPInfo.close = iClose;
	objDayPInfo.date = iDate;
	sprintf(temp, "%d", iDate/10000);
	int iQuartIdxCurr = GetQuartIdx(iDate/10000);
	
	if (0 > iQuartIdxCurr)
	{
		DEBUG("GetPValue iClose: "<<iClose<<", iDate: "<<iDate<<", temp: "<<temp<<", iQuartIdxCurr: "<<iQuartIdxCurr);
		//return 0;
		iQuartIdxCurr = 0;
	}
	string strDayDate = DateInt2Str(iDate);
	int iQuartIdx  = 0;
	int iQuartIdx2 = 0;

	// Divide Rate, total value
	iQuartIdx = iQuartIdxCurr;
	for (int i=0; i<3; i++) // 只找离其最近两年的分红送配信息
	{
		if (iQuartIdx >= m_vecQuart.size())
		{
			break;
		}

		int j=3;
		for (; j>=0; j--)
		{
			if (NULL != m_vecQuart[iQuartIdx].m_rQuart[j].m_ptrDivide) // 找到离其最近的分红送配信息
			{
				DivideInfo *ptrDivide = m_vecQuart[iQuartIdx].m_rQuart[j].m_ptrDivide;
				if (ptrDivide->m_strYaggr < strDayDate)
				{
					objDayPInfo.divideRate = (m_vecDayData[iDayIdx].close != 0) ? (10000*ptrDivide->m_iXjfh/m_vecDayData[iDayIdx].close) : 0;
					objDayPInfo.totalValue = ptrDivide->m_iTotalEquity/100000000 * m_vecDayData[iDayIdx].close;
					if ((ptrDivide->m_strCqcxr < strDayDate) && (ptrDivide->m_iSzzbl > 0))
					{
						objDayPInfo.divideRate = objDayPInfo.divideRate*100/(100+ptrDivide->m_iSzzbl);
						objDayPInfo.totalValue = objDayPInfo.totalValue * (100+ptrDivide->m_iSzzbl)/100;
					}
					break;
				}
			}
		}
		if (j >= 0)
		{
			break;
		}
		iQuartIdx++;
	}

	// pr2
	iQuartIdx  = iQuartIdxCurr;
	iQuartIdx2 = ((iDate%10000)/100-1)/3;
	for (; iQuartIdx2 >= 0; iQuartIdx2--)
	{
		if (iQuartIdx >= m_vecQuart.size())
		{
			break;
		}

		if ((!m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_strCode.empty()) 
			&& (m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_strClosingDate < strDayDate))
		{
			break;
		}
	}
	if (iQuartIdx2 < 0)
	{
		iQuartIdx++;
		iQuartIdx2 = 3;
		for (; iQuartIdx2 >= 0; iQuartIdx2--)
		{
			if (iQuartIdx >= m_vecQuart.size())
			{
				break;
			}
			if ((!m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_strCode.empty()) 
				&& (m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_strClosingDate < strDayDate))
			{
				break;
			}
		}
	}
	//DEBUG("iQuartIdx: "<<iQuartIdx<<", iQuartIdx2: "<<iQuartIdx2);
	if ((iQuartIdx2 >= 0) && (iQuartIdx < m_vecQuart.size()))
	{
		int iOneEarnYear = m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_iOneEarnYear;
		int iOneAsset = m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_iNetAssertOne;
		//DEBUG("iOneEarnYear: "<<iOneEarnYear<<", iOneAsset: "<<iOneAsset);
		if ((NULL != m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_ptrDivide) 
			&& (m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_ptrDivide->m_strCqcxr < strDayDate))
		{
			if (m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_ptrDivide->m_iSzzbl > 0)
			{
				iOneEarnYear = iOneEarnYear*100/(100+m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_ptrDivide->m_iSzzbl);
				iOneAsset = iOneAsset*100/(100+m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_ptrDivide->m_iSzzbl);
			}
		}
		//DEBUG("iOneEarnYear: "<<iOneEarnYear<<", iOneAsset: "<<iOneAsset<<", m_vecDayData[iDayIdx].close: "<<m_vecDayData[iDayIdx].close);
		objDayPInfo.pr2 = (iOneEarnYear != 0) ? m_vecDayData[iDayIdx].close*iOneAsset/(iOneEarnYear*iOneEarnYear) : 0;
		//DEBUG("objDayPInfo.pr2: "<<objDayPInfo.pr2);
		if (objDayPInfo.pr2>1000)objDayPInfo.pr2 = 1000;
		if (objDayPInfo.pr2<-1000) objDayPInfo.pr2 = -1000;
		if ((0 > iOneEarnYear) && (0 < objDayPInfo.pr2)) objDayPInfo.pr2 = objDayPInfo.pr2 - 2*objDayPInfo.pr2;
	}

	// pr
	iQuartIdx  = iQuartIdxCurr;
	iQuartIdx2 = ((iDate%10000)/100-1)/3;
	for (; iQuartIdx2 >= 0; iQuartIdx2--)
	{
		if (iQuartIdx >= m_vecQuart.size())
		{
			break;
		}
		if ((!m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_strCode.empty())
			&& (m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_strDeclarationDate < strDayDate))
		{
			break;
		}
	}
	if (iQuartIdx2 < 0)
	{
		iQuartIdx++;
		iQuartIdx2 = 3;
		for (; iQuartIdx2 >= 0; iQuartIdx2--)
		{
			if (iQuartIdx >= m_vecQuart.size())
			{
				break;
			}
			if ((!m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_strCode.empty()) 
				&& (m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_strDeclarationDate < strDayDate))
			{
				break;
			}
		}
	}
	if ((iQuartIdx2 >= 0)  && (iQuartIdx < m_vecQuart.size()))
	{
		int iOneEarnYear = m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_iOneEarnYear;
		int iOneAsset = m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_iNetAssertOne;
		if ((NULL != m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_ptrDivide) 
			&& (m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_ptrDivide->m_strCqcxr < strDayDate))
		{
			if (m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_ptrDivide->m_iSzzbl > 0)
			{
				iOneEarnYear = iOneEarnYear*100/(100+m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_ptrDivide->m_iSzzbl);
				iOneAsset = iOneAsset*100/(100+m_vecQuart[iQuartIdx].m_rQuart[iQuartIdx2].m_ptrDivide->m_iSzzbl);
			}
		}
		objDayPInfo.pe = (iOneEarnYear != 0) ? 100*m_vecDayData[iDayIdx].close/iOneEarnYear : 0;
		if (objDayPInfo.pe>10000) objDayPInfo.pe = 10000;
		if (objDayPInfo.pe<-10000) objDayPInfo.pe = -10000;
		objDayPInfo.pb = (iOneAsset != 0) ? 100*m_vecDayData[iDayIdx].close/iOneAsset : 0;
		if (objDayPInfo.pb>5000) objDayPInfo.pb = 5000;
		if (objDayPInfo.pb<-5000) objDayPInfo.pb = -5000;
		objDayPInfo.pr = (iOneEarnYear != 0) ? m_vecDayData[iDayIdx].close*iOneAsset/(iOneEarnYear*iOneEarnYear) : 0;
		if (objDayPInfo.pr>1000)objDayPInfo.pr = 1000;
		if (objDayPInfo.pr<-1000) objDayPInfo.pr = -1000;
		if ((0 > iOneEarnYear) && (0 < objDayPInfo.pr)) objDayPInfo.pr = objDayPInfo.pr - 2*objDayPInfo.pr;
	}

	return 0;
}

///////begin: 返回条目列表函数/////////////////////////////
int StockData::CheckReportIdx(CListIdx &objListIdx)
{
	if ((objListIdx.m_iQuartIdx1 < 0) || (objListIdx.m_iQuartIdx1 >= m_vecQuart.size())){return 1;}
	if ((objListIdx.m_iQuartIdx2 < 0) || (objListIdx.m_iQuartIdx2 > 3)){return 1;}
	return 0;
}
string StockData::GetStrCode(CListIdx objListIdx, eItemType &eType, string &strRetCode){ eType = eStr; return m_strCode; }
string StockData::GetstrName(CListIdx objListIdx, eItemType &eType, string &strRetCode){ eType = eStr; return m_strName; }
string StockData::GetstrEarnOne1(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    char temp[64]; eType = eDouble;if (0 != CheckReportIdx(objListIdx)){return "";}
    sprintf(temp, "%.2f", 0.01*m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_iEarnOne1); return temp; 
}
string StockData::GetstrEarnOne2(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    char temp[64]; eType = eDouble;if (0 != CheckReportIdx(objListIdx)){return "";}
    sprintf(temp, "%.2f", 0.01*m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_iEarnOne2); return temp; 
}
string StockData::GetstrRevenue(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    char temp[64]; eType = eDouble;if (0 != CheckReportIdx(objListIdx)){return "";}
    sprintf(temp, "%.2f", 0.01*m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_iRevenue); return temp; 
}
string StockData::GetstrRevenueRose1(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    char temp[64]; eType = eDouble;if (0 != CheckReportIdx(objListIdx)){return "";}
    sprintf(temp, "%.2f", 0.01*m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_iRevenueRose1); return temp; 
}
string StockData::GetstrRevenueRose2(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    char temp[64]; eType = eDouble;if (0 != CheckReportIdx(objListIdx)){return "";}
    sprintf(temp, "%.2f", 0.01*m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_iRevenueRose2); return temp; 
}
string StockData::GetStrNetProfit(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    char temp[64]; eType = eDouble;if (0 != CheckReportIdx(objListIdx)){return "";}
    sprintf(temp, "%.2f", 0.01*m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_iNetProfit); return temp;
}
string StockData::GetstrNetProfitRose1(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    char temp[64]; eType = eDouble;if (0 != CheckReportIdx(objListIdx)){return "";}
    sprintf(temp, "%.2f", 0.01*m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_iNetProfitRose1); return temp;
}
string StockData::GetstrNetProfitRose2(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    char temp[64]; eType = eDouble;if (0 != CheckReportIdx(objListIdx)){return "";}
    sprintf(temp, "%.2f", 0.01*m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_iNetProfitRose2); return temp;
}
string StockData::GetstrNetAssertOne(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    char temp[64]; eType = eDouble;if (0 != CheckReportIdx(objListIdx)){return "";}
    sprintf(temp, "%.2f", 0.01*m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_iNetAssertOne); return temp;
}
string StockData::GetstrNetAssertRate(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    char temp[64]; eType = eDouble;if (0 != CheckReportIdx(objListIdx)){return "";}
    sprintf(temp, "%.2f", 0.01*m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_iNetAssertRate); return temp;
}
string StockData::GetstrCashFlowOne(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    char temp[64]; eType = eDouble;if (0 != CheckReportIdx(objListIdx)){return "";}
    sprintf(temp, "%.2f", 0.01*m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_iCashFlowOne); return temp;
}
string StockData::GetstrGrossProfit(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    char temp[64]; eType = eDouble;if (0 != CheckReportIdx(objListIdx)){return "";}
    sprintf(temp, "%.2f", 0.01*m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_iGrossProfit); return temp;
}
string StockData::GetstrAllocationProfit(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    eType = eStr;if (0 != CheckReportIdx(objListIdx)){return "";}
    return m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_strAllocationProfit;
}
string StockData::GetstrDividentRate(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    char temp[64]; eType = eDouble;if (0 != CheckReportIdx(objListIdx)){return "";}
    sprintf(temp, "%.2f", 0.01*m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_iDividentRate); return temp;
}
string StockData::GetstrDeclarationDate(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    eType = eStr;if (0 != CheckReportIdx(objListIdx)){return "";}
    return m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_strDeclarationDate;
}
string StockData::GetstrClosingDate(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    eType = eStr;if (0 != CheckReportIdx(objListIdx)){return "";}
    return m_vecQuart[objListIdx.m_iQuartIdx1].m_rQuart[objListIdx.m_iQuartIdx2].m_strClosingDate;
}

int StockData::CheckDayIdx(CListIdx &objListIdx)
{
    if ((objListIdx.m_iDayIdx<0) || (objListIdx.m_iDayIdx>=m_vecDayData.size())){return 1;}
	return 0;
}

string StockData::GetStrDate(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    eType=eInt; if (0 != CheckDayIdx(objListIdx)) {return "";}
	if (m_vecDayData[objListIdx.m_iDayIdx].date != m_objDayPInfo.date){
		if (0 != GetPValue(objListIdx.m_iDayIdx, m_objDayPInfo)) return "";}
	char temp[64]; sprintf(temp, "%d", m_objDayPInfo.date); return temp;
}
string StockData::GetStrClose(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    eType=eDouble; if (0 != CheckDayIdx(objListIdx)) {return "";}
	if (m_vecDayData[objListIdx.m_iDayIdx].date != m_objDayPInfo.date){
		if (0 != GetPValue(objListIdx.m_iDayIdx, m_objDayPInfo)) return "";}
	char temp[64]; sprintf(temp, "%.2f", 0.01*m_objDayPInfo.close); return temp;

}
string StockData::GetStrPE(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    eType=eDouble; if (0 != CheckDayIdx(objListIdx)) {return "";}
	if (m_vecDayData[objListIdx.m_iDayIdx].date != m_objDayPInfo.date){
		if (0 != GetPValue(objListIdx.m_iDayIdx, m_objDayPInfo)) return "";}
	char temp[64]; sprintf(temp, "%.2f", 0.01*m_objDayPInfo.pe); return temp;
}
string StockData::GetStrPB(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    eType=eDouble; if (0 != CheckDayIdx(objListIdx)) {return "";}
	if (m_vecDayData[objListIdx.m_iDayIdx].date != m_objDayPInfo.date){
		if (0 != GetPValue(objListIdx.m_iDayIdx, m_objDayPInfo)) return "";}
	char temp[64]; sprintf(temp, "%.2f", 0.01*m_objDayPInfo.pb); return temp;
}
string StockData::GetStrPR(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    eType=eDouble; if (0 != CheckDayIdx(objListIdx)) {return "";}
	if (m_vecDayData[objListIdx.m_iDayIdx].date != m_objDayPInfo.date){
		if (0 != GetPValue(objListIdx.m_iDayIdx, m_objDayPInfo)) return "";}
	char temp[64]; sprintf(temp, "%.2f", 0.01*m_objDayPInfo.pr); return temp;
}
string StockData::GetStrPR2(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    eType=eDouble; if (0 != CheckDayIdx(objListIdx)) {return "";}
	if (m_vecDayData[objListIdx.m_iDayIdx].date != m_objDayPInfo.date){
		if (0 != GetPValue(objListIdx.m_iDayIdx, m_objDayPInfo)) return "";}
	char temp[64]; sprintf(temp, "%.2f", 0.01*m_objDayPInfo.pr2); return temp;
}
string StockData::GetStrDivideRate(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    eType=eDouble; if (0 != CheckDayIdx(objListIdx)) {return "";}
	if (m_vecDayData[objListIdx.m_iDayIdx].date != m_objDayPInfo.date){
		if (0 != GetPValue(objListIdx.m_iDayIdx, m_objDayPInfo)) return "";}
	char temp[64]; sprintf(temp, "%.2f", 0.01*m_objDayPInfo.divideRate); return temp;
}
string StockData::GetStrTotalValue(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{
    eType=eDouble; if (0 != CheckDayIdx(objListIdx)) {return "";}
	if (m_vecDayData[objListIdx.m_iDayIdx].date != m_objDayPInfo.date){
		if (0 != GetPValue(objListIdx.m_iDayIdx, m_objDayPInfo)) return "";}
	char temp[64]; sprintf(temp, "%.2f", 0.01*m_objDayPInfo.totalValue); return temp;
}

string StockData::GetStrSubIndustry(CListIdx objListIdx, eItemType &eType, string &strRetCode)
{eType=eStr; return m_objInfo.m_strSubIndustry;}

///////end: 返回条目列表函数/////////////////////////////

CStockMap* CStockMap::m_pInstance = NULL;

CStockMap* CStockMap::Instance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CStockMap;
	}
	return m_pInstance;
}

void CStockMap::Remove()
{
}

CStockMap::CStockMap()
{
	m_mapStock.clear();
	m_vecSubIndustry.clear();
	m_strEndDate = "";
	m_bReportDate = false;
}

int CStockMap::Init()
{
	if (0 != CDBData::Instance()->GetData(m_mapStock))
	{
		ERROR("CStockMap::Init call CDBData::Instance()->GetData(m_mapStock) failed.");
		m_bInited = false;
		return 1;
	}
	DEBUG("CDBData::Instance()->GetData(m_mapStock)");

	/*if (0 != CheckDBData())
	{
		ERROR("CStockMap::Init call CheckDBData error.");
		m_bInited = false;
		return 1;
	}*/

	if (0 != CDBData::Instance()->GetSubIndustry(m_vecSubIndustry))
	{
		ERROR("CStockMap::Init call CDBData::Instance()->GetSubIndustry(m_vecSubIndustry) failed.");
		m_bInited = false;
		return 1;
	}

	if (0 != InitRawData())
	{
		ERROR("CStockMap::Init call InitRawData failed.");
		return 1;
	}
	DEBUG("Call InitRawData success.");

	for (map<string, StockData*>::iterator iter=m_mapStock.begin(); iter!=m_mapStock.end();)
	{
		if (0 != iter->second->DecorateReport())
		{
			ERROR("Call DecorateReport error. code: "<<iter->first);
			//return 1;
			//delete iter->second;
			//m_mapStock.erase(iter++);
			iter++;
		}
		else
		{
		    iter++;
		}
	}
	
	m_bInited = true;
	return 0;
}

int CStockMap::GetStock(
	string strCode,
	StockData *&ptrStock
	)
{
	ptrStock = NULL;

	map<string, StockData*>::iterator iter;
	iter = m_mapStock.find(strCode);
	if (m_mapStock.end() != iter)
	{
		DEBUG("GetStock success. strCode: "<<strCode);
		ptrStock = iter->second;
	}
	else
	{
		DEBUG("GetStock success. failed: "<<strCode);
		return 1;
	}

	return 0;
}

int CStockMap::GetReport(
	string strCode,
	string strDate,
	ReportData &objReport
	)
{
	return 0;
}

int CStockMap::GetSort(
	string strDate,
	string strItem,
	vector<string> vecSort
	)
{
	return 0;
}

int CStockMap::ShowStockData(
	StockData* ptrStock
	)
{
	if (NULL == ptrStock)
	{
		ERROR("ShowStockData ptrStock == NULL.");
		return 1;
	}
	DEBUG("Start Show stock data\n");
	ostringstream os;
	string strShow = "---show stock data---\n";
	strShow = strShow + "code: " + ptrStock->m_strCode + "\n";
	strShow = strShow + "name: " + ptrStock->m_strName + "\n";
	if (true == ptrStock->m_bInfo)
	{
		os << ptrStock->m_objInfo.m_iFloatStock;
		strShow = strShow + "FloatStock: " + os.str() + ", ";
		os.str("");
		os << ptrStock->m_objInfo.m_iFloatValue;
		strShow = strShow + "FloatValue: " + os.str() + ", ";
		os.str("");
		os << ptrStock->m_objInfo.m_iPE;
		strShow = strShow + "PE: " + os.str() + ", ";
		os.str("");
		os << ptrStock->m_objInfo.m_iPerHolding;
		strShow = strShow + "PerHolding: " + os.str() + ", ";
		os.str("");
		os << ptrStock->m_objInfo.m_iPerValue;
		strShow = strShow + "PerValue: " + os.str() + ", ";
		os.str("");
		os << ptrStock->m_objInfo.m_iShareHolder;
		strShow = strShow + "ShareHolder: " + os.str() + ", ";
		os.str("");
		os << ptrStock->m_objInfo.m_iTotalStock;
		strShow = strShow + "TotalStock: " + os.str() + ", ";
		os.str("");
		os << ptrStock->m_objInfo.m_iTotalValue;
		strShow = strShow + "TotalValue: " + os.str() + ", ";
		strShow = strShow + "Area: " + ptrStock->m_objInfo.m_strArea + ", ";
		strShow = strShow + "Code: " + ptrStock->m_objInfo.m_strCode + ", ";
		strShow = strShow + "GetDate: " + ptrStock->m_objInfo.m_strGetDate + ", ";
		strShow = strShow + "Industry: " + ptrStock->m_objInfo.m_strIndustry + ", ";
		strShow = strShow + "LaunchDate: " + ptrStock->m_objInfo.m_strLaunchDate + ", ";
		strShow = strShow + "Name: " + ptrStock->m_objInfo.m_strName + ", ";
		strShow = strShow + "SubIndustry: " + ptrStock->m_objInfo.m_strSubIndustry + ".\n";
	}
	DEBUG(strShow);

	for (int i=0; i<ptrStock->m_vecQuart.size(); i++)
	{
		for (int j=3; j>=0; j--)
		{
			strShow = "";
			os.str("");
			os << ptrStock->m_vecQuart[i].m_rQuart[j].m_iCashFlowOne;
			strShow = strShow + os.str() + ", ";
			os.str("");
			os << ptrStock->m_vecQuart[i].m_rQuart[j].m_iDividentRate;
			strShow = strShow + os.str() + ", ";
			os.str("");
			os << ptrStock->m_vecQuart[i].m_rQuart[j].m_iEarnOne1;
			strShow = strShow + os.str() + ", ";
			os.str("");
			os << ptrStock->m_vecQuart[i].m_rQuart[j].m_iEarnOne2;
			strShow = strShow + os.str() + ", ";
			os.str("");
			os << ptrStock->m_vecQuart[i].m_rQuart[j].m_iGrossProfit;
			strShow = strShow + os.str() + ", ";
			os.str("");
			os << ptrStock->m_vecQuart[i].m_rQuart[j].m_iNetAssertOne;
			strShow = strShow + os.str() + ", ";
			os.str("");
			os << ptrStock->m_vecQuart[i].m_rQuart[j].m_iNetAssertRate;
			strShow = strShow + os.str() + ", ";
			os.str("");
			os << ptrStock->m_vecQuart[i].m_rQuart[j].m_iNetProfit;
			strShow = strShow + os.str() + ", ";
			os.str("");
			os << ptrStock->m_vecQuart[i].m_rQuart[j].m_iNetProfitRose1;
			strShow = strShow + os.str() + ", ";
			os.str("");
			os << ptrStock->m_vecQuart[i].m_rQuart[j].m_iNetProfitRose2;
			strShow = strShow + os.str() + ", ";
			ptrStock->m_vecQuart[i].m_rQuart[j].m_iRevenue;
			strShow = strShow + os.str() + ", ";
			os.str("");
			os << ptrStock->m_vecQuart[i].m_rQuart[j].m_iRevenueRose1;
			strShow = strShow + os.str() + ", ";
			os.str("");
			os << ptrStock->m_vecQuart[i].m_rQuart[j].m_iRevenueRose2;
			strShow = strShow + os.str() + ", ";
			strShow = strShow + ptrStock->m_vecQuart[i].m_rQuart[j].m_strAllocationProfit + ", ";
			strShow = strShow + ptrStock->m_vecQuart[i].m_rQuart[j].m_strClosingDate + ", ";
			strShow = strShow + ptrStock->m_vecQuart[i].m_rQuart[j].m_strCode + ", ";
			strShow = strShow + ptrStock->m_vecQuart[i].m_rQuart[j].m_strDeclarationDate + ", ";
			strShow = strShow + ptrStock->m_vecQuart[i].m_rQuart[j].m_strName + ", ";
			strShow = strShow + ptrStock->m_vecQuart[i].m_rQuart[j].m_strUnknown + ", \n";
			DEBUG(strShow);
		}
	}

	return 0;
}

int CStockMap::GetSubIndustry(
	vector<CIndustryItem> *&ptrVecSubIndustry
	)
{
	ptrVecSubIndustry = &m_vecSubIndustry;

	return 0;
}

int CStockMap::InitRawData()
{
	vector<string> vecFile;
	vecFile.clear();
	//if (0 != TravelDir(cstrSHDir, vecFile))
	if (0 != ShowAllFiles(cstrSHDir.c_str(), vecFile))
	{
		ERROR("Call TravelDir(cstrSHDir, vecFile) failed. cstrSHDir: "<<cstrSHDir);
		return 1;
	}
	DEBUG("Call TravelDir(cstrSHDir, vecFile) success. vecFile.size: "<<vecFile.size());
	//if (0 != TravelDir(cstrSZDir, vecFile))
	if (0 != ShowAllFiles(cstrSZDir.c_str(), vecFile))
	{
		ERROR("Call TravelDir(cstrSZDir, vecFile) failed. cstrSHDir: "<<cstrSZDir);
		return 1;
	}
	DEBUG("Call TravelDir(cstrSHDir, vecFile) success. vecFile.size: "<<vecFile.size());

	for (int i=0; i<vecFile.size(); i++)
	{
		if (0 != LoadFile(vecFile[i]))
		{
			ERROR("Call LoadFile(vecFile[i]) error. vecFile[i]: "<<vecFile[i]);
			return 1;
		}
	}

	return 0;
}

int CStockMap::TravelDir(
	string strDir,
	vector<string> &vecFile
	)
{
/*    CFileFind ff;
	if (strDir[strDir.size()-1] != '\\')
        strDir += "\\";
    strDir += "*.day";
     
	string strName;
    string strPath;
    BOOL ret = ff.FindFile(strDir.c_str());
    while (ret)
	{
		ret = ff.FindNextFile();
        if (ff.IsDirectory() && !ff.IsDots())
        {
            string strPath = ff.GetFilePath();
            TravelDir(strPath.c_str(), vecFile);
        }
        else if (!ff.IsDirectory() && !ff.IsDots())
        {
            strName = ff.GetFileName();
            strPath = ff.GetFilePath();
            vecFile.push_back(strPath);
			OutputDebugString("strName: ");
			OutputDebugString(strName.c_str());
			OutputDebugString(", strPath:");
			OutputDebugString(strPath.c_str());
			OutputDebugString("\r\n");
        }
    }
*/
	return 0;
}

int CStockMap::LoadFile(
	string strFile
	)
{
	// 得到股票代码
	string strCode = "";
	CStrSplit css;
	css.SetString(strFile, "/");
	if (css.Count() <= 0)
	{
		ERROR("css.Count() <= 0. strFile: "<<strFile);
		return 1;
	}
	// sz000008.day
	strCode = css.Item(css.Count()-1);
	if (12 != strCode.length())
	{
		ERROR("12 != strCode.length(). strCode: "<<strCode);
		return 1;
	}
	strCode = strCode.substr(2,6);
	//DEBUG("strCode: "<<strCode);

	// 从文件中得到每天的数据
    vector<DayData> vecDayData;
	vecDayData.clear();
	DayData sDayData;
	FILE *fp; 
	if (!(fp=fopen(strFile.c_str(),"rb")))
	{
		ERROR("Open file failed. strFile: "<<strFile);
		return 1;
	}

	while (1 == fread(&sDayData, 32, 1, fp))
	{
		vecDayData.push_back(sDayData);
	}
	fclose(fp);

	StockData* ptrStockData = NULL;
	map<string,StockData*>::iterator iter;
	iter = m_mapStock.find(strCode);
	if (iter != m_mapStock.end())
	{
		ptrStockData = iter->second;
	}
	else
	{
		ptrStockData = new StockData;
		ptrStockData->m_strCode = strCode;
		m_mapStock[strCode] = ptrStockData;
	}
	if (NULL == ptrStockData)
	{
		ERROR("NULL == ptrStockData");
		return 1;
	}

    CDayData objDayData;
	ptrStockData->m_vecDayData.clear();
	for (int i=vecDayData.size(); i>0; i--)
	{
	    objDayData.date    = vecDayData[i-1].date;
		objDayData.close   = vecDayData[i-1].close;
		objDayData.high    = vecDayData[i-1].high;
		objDayData.low     = vecDayData[i-1].low;
		objDayData.open    = vecDayData[i-1].open;
		objDayData.remain  = vecDayData[i-1].remain;
		objDayData.amount  = vecDayData[i-1].amount;
		objDayData.vol     = vecDayData[i-1].vol;
		ptrStockData->m_vecDayData.push_back(objDayData); // 将数据从最近的日期向后排
	}
	ptrStockData = NULL;

	return 0;
}

int CStockMap::CheckDBData()
{
	string strCode = "";
	string strCloseDate = "";
	string strDeclDate = "";
	StockData *ptrStock = NULL;
	map<string, StockData*>::iterator iter;
	for (iter = m_mapStock.begin(); iter != m_mapStock.end(); iter++)
	{
		ptrStock = iter->second;
		if (NULL == ptrStock)
		{
			ERROR("ptrStock is NULL.");
			return 1;
		}

		//DEBUG("ptrStock: "<<ptrStock<<", ptrStock->m_vecQuart.size(): "<<ptrStock->m_vecQuart.size());
		for (int i=0; i+1<ptrStock->m_vecQuart.size(); i++)
		{
			for (int j=3; j>=0; j--)
			{
				strCode = ptrStock->m_vecQuart[i].m_rQuart[j].m_strCode;  // 四季度
				if (!strCode.empty())
				{
					strCloseDate = ptrStock->m_vecQuart[i].m_rQuart[j].m_strClosingDate;
					strDeclDate = ptrStock->m_vecQuart[i+1].m_rQuart[j].m_strDeclarationDate;
					if (strCloseDate < strDeclDate)
					{
						if (0 != CDBData::Instance()->UpdateDeclDate(strCode, strCloseDate, strDeclDate))
						{
							ERROR("UpdateDeclDate error. strCode: "<<strCode<<", strCloseDate: "<<strCloseDate<<", strDeclDate: "<<strDeclDate);
							return 1;
						}
					}
					ptrStock->m_vecQuart[i].m_rQuart[j].m_strDeclarationDate = strDeclDate;
				}
				//DEBUG("CheckDBData code: "<<strCode<<", i: "<<i<<", j: "<<j<<", size: "<<ptrStock->m_vecQuart.size()
				//	<<", date: "<<ptrStock->m_vecQuart[i].m_rQuart[j].m_strClosingDate);
			}
		}

		if (ptrStock->m_vecQuart.size() > 1) // 最后一个元素
		{
			int idx = ptrStock->m_vecQuart.size()-1;
			for (int j=3; j>=0; j--)
			{
				strCode = ptrStock->m_vecQuart[idx].m_rQuart[j].m_strCode;  // 四季度
				if (!strCode.empty())
				{
					strCloseDate = ptrStock->m_vecQuart[idx].m_rQuart[j].m_strClosingDate;
					strDeclDate = ptrStock->m_vecQuart[idx].m_rQuart[j].m_strDeclarationDate;
					//DEBUG("declend strCode: "<<strCode<<", strDeclDate: "<<strDeclDate<<", 2: "<<ptrStock->m_vecQuart[idx-1].m_rQuart[j].m_strDeclarationDate);
					if (strDeclDate == ptrStock->m_vecQuart[idx-1].m_rQuart[j].m_strDeclarationDate)
					{
						strDeclDate = strCloseDate;
						if (0 != CDBData::Instance()->UpdateDeclDate(strCode, strCloseDate, strDeclDate))
						{
							ERROR("UpdateDeclDate error. strCode: "<<strCode<<", strCloseDate: "<<strCloseDate<<", strDeclDate: "<<strDeclDate);
							return 1;
						}
						ptrStock->m_vecQuart[idx].m_rQuart[j].m_strDeclarationDate = strDeclDate;
					}
				}
			}
		}

		DEBUG("Update item finished. strCode: "<<ptrStock->m_strCode);
	}

	return 0;
}

int DateStr2Int(string strDate)
{
	if (strDate.length() != 10)
	{
		ERROR("strDate.length() != 10. length: "<<strDate.length()<<", strDate: "<<strDate);
		return 0;
	}
	string strTemp = "";
	int iDate = 0;
	strTemp = strDate.substr(0,4);
	strTemp += strDate.substr(5,2);
	strTemp += strDate.substr(8,2);
	iDate = atoi(strTemp.c_str());
	//DEBUG("DateStr2Int. strDate: ["<<strDate<<"], iDate: "<<iDate);

	return iDate;
}

int GetNextDate(int iDate)
{
	int iNextDate = 0;
	int iYear = iDate/10000;
	int iMMDD = iDate%10000;
	if (iMMDD < 331)
	{
		iNextDate = iYear*10000 + 331;
	}
	else if (iMMDD < 630)
	{
		iNextDate = iYear*10000 + 630;
	}
	else if (iMMDD < 930)
	{
		iNextDate = iYear*10000 + 930;
	}
	else if (iMMDD < 1231)
	{
		iNextDate = iYear*10000 + 1231;
	}
	else if (iMMDD == 1231)
	{
		iNextDate = (iYear+1)*10000 + 331;
	}
	else
	{
	    ERROR("GetNextDate error. iDate: "<<iDate);
	    return 0;
	}

	return iNextDate;
}

int GetPreDate(int iDate)
{
	int iPreDate = 0;
	int iYear = iDate/10000;
	int iMMDD = iDate%10000;
	if (iMMDD <= 331)
	{
		iPreDate = (iYear-1)*10000 + 1231;
	}
	else if (iMMDD <= 630)
	{
		iPreDate = iYear*10000 + 331;
	}
	else if (iMMDD <= 930)
	{
		iPreDate = iYear*10000 + 630;
	}
	else if (iMMDD <= 1231)
	{
		iPreDate = iYear*10000 + 930;
	}
	else
	{
		ERROR("CSingleLine::GetPreDate error. iDate: "<<iDate<<", iMMDD: "<<iMMDD);
	}

	return iPreDate;
}

string DateInt2Str(int iDate)
{
	string strDate = "";
	char temp[8];
	sprintf(temp, "%d", iDate/10000);
	strDate = strDate + temp;
	sprintf(temp, "%02d", iDate%10000/100);
	strDate = strDate + "-" + temp;
	sprintf(temp, "%02d", iDate%100);
	strDate = strDate + "-" + temp;

	return strDate;
}

string GetNextDate(string strDate)
{
	int iDate = DateStr2Int(strDate);
	iDate = GetNextDate(iDate);
	return DateInt2Str(iDate);
}

string GetPreDate(string strDate)
{
	int iDate = DateStr2Int(strDate);
	iDate = GetPreDate(iDate);
	return DateInt2Str(iDate);
}

COptionMap* COptionMap::m_pInstance = NULL;
COptionMap* COptionMap::Instance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new COptionMap;
	}
	return m_pInstance;
}

COptionMap::COptionMap()
{
}

int COptionMap::Init()
{
	vector<COptionInfo*> vecOptionInfo;
	if (0 != CDBData::Instance()->GetOptionInfo(vecOptionInfo))
	{
		ERROR("Call CDBData::Instance()->GetOptionInfo error.");
		return 1;
	}

	m_mapOption.clear();
	m_mapTabSets.clear();
	for (int i=0; i<vecOptionInfo.size(); i++)
	{
		m_mapOption[vecOptionInfo[i]->m_strCode] = vecOptionInfo[i];
		m_mapTabSets[vecOptionInfo[i]->m_strTab].insert(vecOptionInfo[i]->m_strCode);
	}

	return 0;
}

int COptionMap::GetAll(
	string strTab,
	vector<string> &vecCode
	)
{
	map<string, COptionInfo*>::iterator iter;

	vecCode.clear();
	for (iter=m_mapOption.begin(); iter!=m_mapOption.end(); iter++)
	{
		vecCode.push_back(iter->second->m_strCode);
	}

	return 0;
}

bool COptionMap::CheckStock(
	string strCode
	)
{
	map<string, COptionInfo*>::iterator iter;
	iter = m_mapOption.find(strCode);
	if (m_mapOption.end() != iter)
	{
		return true;
	}

	return false;
}

int COptionMap::GetStock(
	string strTab,
	string strCode,
	COptionInfo *&ptrOption
	)
{
	ptrOption = NULL;
	map<string, COptionInfo*>::iterator iter;
	iter = m_mapOption.find(strCode);
	if (m_mapOption.end() != iter)
	{
		ptrOption = iter->second;
	}

	return 0;
}

int COptionMap::ModifyStock(
	COptionInfo objOption
	)
{
	map<string, COptionInfo*>::iterator iter;
	iter = m_mapOption.find(objOption.m_strCode);
	if (m_mapOption.end() == iter)
	{
		ERROR("Don't find objOption. objOption.m_strCode: "<<objOption.m_strCode);
		return 1;
	}

	if (0 != CDBData::Instance()->UpdateOptionInfo(0, objOption))
	{
		ERROR("Call CDBData::Instance()->UpdateOptionInfo error. objOption.m_strCode: "<<objOption.m_strCode);
		return 1;
	}
	*(iter->second) = objOption;
	return 0;
}

int COptionMap::AddStock(
	COptionInfo objOption
	)
{
	map<string, COptionInfo*>::iterator iter;
	iter = m_mapOption.find(objOption.m_strCode);
	if (m_mapOption.end() != iter)
	{
		ERROR("The objOption has existed, add again error. objOption.m_strCode: "<<objOption.m_strCode);
		return 1;
	}

	if (0 != CDBData::Instance()->UpdateOptionInfo(1, objOption))
	{
		ERROR("Call CDBData::Instance()->UpdateOptionInfo(1, objOption) error. objOption.m_strCode: "<<objOption.m_strCode);
		return 1;
	}

	COptionInfo *ptrOption = new COptionInfo;
	*ptrOption = objOption;
	m_mapOption[ptrOption->m_strCode] = ptrOption;
	return 0;
}

int COptionMap::DelStock(
	string strTab,
	string strCode
	)
{
	map<string, COptionInfo*>::iterator iter;
	iter = m_mapOption.find(strCode);
	if (m_mapOption.end() == iter)
	{
		ERROR("Don't find objOption. strCode: "<<strCode);
		return 1;
	}
	COptionInfo *ptrOption = iter->second;

	if (0 != CDBData::Instance()->UpdateOptionInfo(2, *ptrOption))
	{
		ERROR("Call CDBData::Instance()->UpdateOptionInfo(2, *ptrOption) error. strCode: "<<ptrOption->m_strCode);
		return 1;
	}

	m_mapOption.erase(strCode);
	delete ptrOption;

	return 0;
}

//	map<string, COptionInfo*> m_mapOption;
//	static COptionMap* m_pInstance;
