#include "OutInfo.h"
#include "SingleLine.h"
#include "BatchLine.h"
#include "CondList.h"
#include "KLine.h"

COutInfo* COutInfo::m_ptrInstance = NULL;

COutInfo* COutInfo::Instance()
{
    if (NULL == m_ptrInstance)
	{
	    m_ptrInstance = new COutInfo;
		m_ptrInstance->Init();
	}
	
	return m_ptrInstance;
}

void COutInfo::Remove()
{
    if (NULL != m_ptrInstance)
	{
	    delete m_ptrInstance;
		m_ptrInstance = NULL;
	}
}


COutInfo::COutInfo()
{
}

int COutInfo::Init()
{
    m_mapItem.clear();
	m_mapSLine.clear();
	m_mapBLine.clear();
	m_mapCond.clear();
	m_mapKLine.clear();

    ///////////////begin: list条目列表////////////////////////
    // 与ReportData相关的List条目列表
	m_mapItem["Code"]              = &StockData::GetStrCode;
    m_mapItem["Name"]              = &StockData::GetstrName;
    m_mapItem["EarnOne1"]          = &StockData::GetstrEarnOne1;
    m_mapItem["EarnOne2"]          = &StockData::GetstrEarnOne2;
    m_mapItem["Revenue"]           = &StockData::GetstrRevenue;
    m_mapItem["RevenueRose1"]      = &StockData::GetstrRevenueRose1;
    m_mapItem["RevenueRose2"]      = &StockData::GetstrRevenueRose2;
    m_mapItem["NetProfit"]         = &StockData::GetStrNetProfit;
    m_mapItem["NetProfitRose1"]    = &StockData::GetstrNetProfitRose1;
    m_mapItem["NetProfitRose2"]    = &StockData::GetstrNetProfitRose2;
    m_mapItem["NetAssertOne"]      = &StockData::GetstrNetAssertOne;
    m_mapItem["NetAssertRate"]     = &StockData::GetstrNetAssertRate;
    m_mapItem["CashFlowOne"]       = &StockData::GetstrCashFlowOne;
    m_mapItem["GrossProfit"]       = &StockData::GetstrGrossProfit;
    m_mapItem["AllocationProfit"]  = &StockData::GetstrAllocationProfit;
    m_mapItem["DividentRate"]      = &StockData::GetstrDividentRate;
    m_mapItem["DeclarationDate"]   = &StockData::GetstrDeclarationDate;
    m_mapItem["ClosingDate"]       = &StockData::GetstrClosingDate;

    // 与DayPInfo相关的List条目列表
	m_mapItem["DayDate"]              = &StockData::GetStrDate;
	m_mapItem["DayClose"]             = &StockData::GetStrClose;
	m_mapItem["DayPE"]                = &StockData::GetStrPE;
	m_mapItem["DayPB"]                = &StockData::GetStrPB;
	m_mapItem["DayPR"]                = &StockData::GetStrPR;
	m_mapItem["DayPR2"]               = &StockData::GetStrPR2;
	m_mapItem["DayDivideRate"]        = &StockData::GetStrDivideRate;
	m_mapItem["DayTotalValue"]        = &StockData::GetStrTotalValue;

	// 与m_objInfo相关的List条目列表
	m_mapItem["SubIndustry"]          = &StockData::GetStrSubIndustry;
	///////////////end: list条目列表////////////////////////

    //////////////begin: single line曲线列表////////////////
    m_mapSLine["Revenue"]             = &CSingleLine::GetRevenueYLine;
	m_mapSLine["Profit"]              = &CSingleLine::GetProfitYLine;
	m_mapSLine["ROE"]                 = &CSingleLine::GetROEYLine;
	m_mapSLine["DivRate"]             = &CSingleLine::GetDivRateYLine;
	m_mapSLine["DayPR"]               = &CSingleLine::GetPRYLine;
	m_mapSLine["DayPR2"]              = &CSingleLine::GetPR2YLine;
	m_mapSLine["DayPrice"]            = &CSingleLine::GetPriceYLine;
	m_mapSLine["DayReinPrice"]        = &CSingleLine::GetReinPriceYLine;
	m_mapSLine["DayValue"]            = &CSingleLine::GetTotalValYLine;
	m_mapSLine["DayCovery"]           = &CSingleLine::GetCoveryYLine;
	m_mapSLine["ROP"]                 = &CSingleLine::GetROPYLine;
    m_mapSLine["DayPP"]               = &CSingleLine::GetPPYLine;
    //////////////end: single line曲线列表//////////////////

    //////////////begin: batch line曲线列表////////////////
    m_mapBLine["Revenue"]             = &CBatchLine::GetRevenueLine;	
	m_mapBLine["Profit"]              = &CBatchLine::GetProfitLine;
    m_mapBLine["DayPrice"]            = &CBatchLine::GetPriceLine;
    //////////////end: batch line曲线列表//////////////////

	/////////////begin: cond list 条件过滤/////////////////
	m_mapCond["ROE"]            = GetROECond;
	m_mapCond["PR"]             = GetPrCond;
    m_mapCond["DeclareDate"]    = GetDeclareDateCond;
    m_mapCond["CloseDate"]      = GetClosingDateCond;
    //m_mapCond["Leader"] = NULL;
	/////////////end: cond list 条件过滤///////////////////

	////////////begin: kline //////////////////////////////
	m_mapKLine["day"]    = &CKLine::GetDayLine;
	m_mapKLine["week"]   = &CKLine::GetWeekLine;
	m_mapKLine["month"]  = &CKLine::GetMonthLine;
	m_mapKLine["year"]   = &CKLine::GetYearLine;
	////////////end: kline ////////////////////////////////

	return 0;
}

