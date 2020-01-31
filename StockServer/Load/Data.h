#ifndef __DATA__H
#define __DATA__H

#include <iostream>
#include <vector>
#include <map>
#include <set>
using namespace std;

class CListIdx
{
public:
	int m_iQuartIdx1;
	int m_iQuartIdx2;
	int m_iDayIdx;

	CListIdx()
	{
		m_iQuartIdx1 = -1;
		m_iQuartIdx2 = -1;
		m_iDayIdx    = -1;
	}
};

class DivideInfo   // 除权除息信息
{
public:
	string m_strMarket;           // 市场类型
	string m_strCode;             // 股票代码
	string m_strName;             // 股票名称
	int m_iSzzbl;              // 送转总比例
	int m_iSgbl;               // 送股比例
	int m_iZgbl;               // 转股比例
	int m_iXjfh;               // 现金分红
	int m_iGxl;                // 股息率
	string m_strYaggr;            // 业绩披露日期
	int m_iYaggrhsrzf;
	int m_iGqdjrqsrzf;
	string m_strGqdjr;            // 股权登记日
	string m_strCqcxr;            // 除权除息日
	int m_iCqcxrhssrzf;
	int m_iYcqts;
	int m_iTotalEquity;        // 总股本
	int m_iEarningPerShare;    // 每股收益
	int m_iNetAssetsPerShare;  // 每股净资产
	int m_iMggjj;              // 每股公积金
	int m_iMgwfply;            // 每股未分配利润（元）
	int m_iJlytbzz;            // 净利润同比增长
	string m_strReportingPeriod;  // 报告期
	string m_strResultsbyDate;    // 业绩披露期
	string m_strProjectProgress;  // 分配进度
	string m_strAllocationPlan;

	DivideInfo()
	{
		m_strMarket          = ""; // 市场类型
		m_strCode            = ""; // 股票代码
		m_strName            = ""; // 股票名称
		m_iSzzbl             = 0;  // 送转总比例
		m_iSgbl              = 0;  // 送股比例
		m_iZgbl              = 0;  // 转股比例
		m_iXjfh              = 0;  // 现金分红
		m_iGxl               = 0;  // 股息率
		m_strYaggr           = ""; // 业绩披露日期
		m_iYaggrhsrzf        = 0;
		m_iGqdjrqsrzf        = 0;
		m_strGqdjr           = ""; // 股权登记日
		m_strCqcxr           = ""; // 除权除息日
		m_iCqcxrhssrzf;
		m_iYcqts             = 0;
		m_iTotalEquity       = 0;  // 总股本
		m_iEarningPerShare   = 0;  // 每股收益
		m_iNetAssetsPerShare = 0;  // 每股净资产
		m_iMggjj             = 0;  // 每股公积金
		m_iMgwfply           = 0;  // 每股未分配利润（元）
		m_iJlytbzz           = 0;  // 净利润同比增长
		m_strReportingPeriod = ""; // 报告期
		m_strResultsbyDate   = ""; // 业绩披露期
		m_strProjectProgress = "";  // 分配进度
		m_strAllocationPlan  = "";
	}
};

class ReportData  // 财报信息
{
public:
	string m_strCode;              // 股票代码
	string m_strName;              // 股票名称
	int m_iEarnOne1;            // 每股收益（元）
	int m_iEarnOne2;            // 每股收益(扣除)(元)
	int m_iRevenue;             // 营业收入(元)
	int m_iRevenueRose1;        // 营业收入同比增长(%)
	int m_iRevenueRose2;        // 营业收入季度环比增长(%)
	int m_iNetProfit;           // 净利润(元)
	int m_iNetProfitRose1;      // 净利润同比增长(%)
	int m_iNetProfitRose2;      // 净利润季度环比增长(%)
	int m_iNetAssertOne;        // 每股净资产(元)
	int m_iNetAssertRate;       // 净资产收益率(%)
	int m_iCashFlowOne;         // 每股经营现金流量(元)
	int m_iGrossProfit;         // 销售毛利率(%)
	string m_strAllocationProfit;  // 利润分配
	int m_iDividentRate;        // 股息率(%)
	string m_strDeclarationDate;   // 公告日期
	string m_strClosingDate;       // 截止日期
	int m_iEquity;                 // 总股本
	string m_strUnknown;           // 未知
	// 扩充字段
	DivideInfo* m_ptrDivide;       // 拆分信息
	int m_iOneEarnYear;         // 每股年化收益
	int m_iDate;                   // 索引使用，同m_strClosingDate值
	int m_iIdx;                    // 价格索引值
	int m_iPrice;                  // 每股价格
	int m_iYearProfit;             // 年化净利润
	int m_iROP;                    // 净利润增长率
	int m_iYearROP;                // 年化净利润增长率
	
	ReportData()
	{
		m_strCode = "";              // 股票代码
		m_strName = "";              // 股票名称
		m_iEarnOne1 = 0;            // 每股收益（元）
		m_iEarnOne2 = 0;            // 每股收益(扣除)(元)
		m_iRevenue = 0;             // 营业收入(元)
		m_iRevenueRose1 = 0;        // 营业收入同比增长(%)
		m_iRevenueRose2 = 0;        // 营业收入季度环比增长(%)
		m_iNetProfit = 0;           // 净利润(元)
		m_iNetProfitRose1 = 0;      // 净利润同比增长(%)
		m_iNetProfitRose2 = 0;      // 净利润季度环比增长(%)
		m_iNetAssertOne = 0;        // 每股净资产(元)
		m_iNetAssertRate = 0;       // 净资产收益率(%)
		m_iCashFlowOne = 0;         // 每股经营现金流量(元)
		m_iGrossProfit = 0;         // 销售毛利率(%)
		m_strAllocationProfit = "";  // 利润分配
		m_iDividentRate = 0;        // 股息率(%)
		m_strDeclarationDate = "";   // 公告日期
		m_strClosingDate = "";       // 截止日期
		m_strUnknown = "";            // 未知
		m_ptrDivide = NULL;           // 拆分信息
		m_iOneEarnYear = 0;           // 每股年化收益
		m_iDate = -1;
		m_iIdx  = -1;
		m_iPrice = 0;
		m_iEquity = 0;
		m_iROP = 0;
        m_iYearROP = 0;
	}
	
};

class QuartData  // 汇总四个季度一年的财报信息
{
public:
	string m_strYear;
	int m_iYear; // 索引使用，同m_strYear值
	ReportData m_rQuart[4];
	
	/*ReportData m_rQuart1;
	ReportData m_rQuart2;
	ReportData m_rQuart3;
	ReportData m_rQuart4;
	*/
	
	int GetiRate();  // 得到本年的股息率
};

struct BaseInfo   // 基本信息
{
	string m_strCode;
	string m_strName;
	int m_iPE;
	string m_strIndustry;
	string m_strSubIndustry;
	string m_strArea;
	int m_iFloatStock;
	int m_iFloatValue;
	int m_iTotalValue;
	int m_iTotalStock;
	string m_strLaunchDate;
	int m_iShareHolder;
	int m_iPerHolding;
	int m_iPerValue;
	string m_strGetDate;
};

struct DayData     // 每日原始价格信息
{
	int date;      // 年月日
	int open;      // 开盘价
	int high;      // 最高价
	int low;       // 最低价
	int close;     // 收盘价
	float amount;  // 成交额（元）
	int vol;       // 成交量（手）
	int remain;    // 上日收盘
};

class CDayData     // 每日原始价格信息
{
public:
	int date;      // 年月日
	int open;      // 开盘价
	int high;      // 最高价
	int low;       // 最低价
	int close;     // 收盘价
	double amount;  // 成交额（元）
	unsigned int vol;       // 成交量（手）
	int remain;    // 上日收盘
};

class CDayPInfo   // 每日价格信息
{
public:
	int date;
	int close;
	int pe;
	int pb;
	int pr;  // pe/roe, declaration date
	int pr2; // close date*/
	int divideRate;
	int totalValue;
	int covery;
    int pp;
    int rp; // 1/pr

	CDayPInfo()
	{
		date   = 0;
		close  = 0;
		pe     = 0;
		pb     = 0;
		pr     = 0;
		pr2    = 0;
		divideRate = 0;
		totalValue = 0;
		covery = 0;
        pp     = 0;
        rp     = 0;
	}
};

enum eItemType{eInt, eDouble, eStr, eEnd}; // 条目都以字符串方式返回，此字段标记其原始类型

class StockData
{
public:
	string m_strCode;
	string m_strName;
	BaseInfo m_objInfo;               // 基本信息
	bool m_bInfo;
	vector<QuartData> m_vecQuart;     // 财报信息
	vector<CDayData> m_vecDayData;    // 价格信息
	vector<DivideInfo> m_vecDivide;  // 分红送配信息
	CDayPInfo m_objDayPInfo;          // 实时计算的一个值，供列表项使用

	StockData()
	{
		m_strCode = "";
		m_strName = "";
		m_bInfo = false;
		m_vecQuart.clear();
		m_vecDayData.clear();
		m_vecDivide.clear();
	}

	int GetLastiRate(
		int idx
		);

	int DecorateReport(); // 修饰类，计算每股年收益，iYear, iDate

	int GetOneEarnYear(        // 每股年收益
		int idxYear,
		int idxQuart,
		int &iOneEarnYear
		);
	
	int GetYearProfit(        // 每股年收益
		int idxYear,
		int idxQuart,
		int &iYearProfit
		);

	int GetROP(        // 净利润增长率
		int idxYear,
		int idxQuart,
		int &iROP
		);
	
	int GetQuartIdx(    // 二分查找
		int iYear
		);

	int GetDayIdx(      // 二分查找
		int iDate
		);

	int GetPValue(
		int iDayIdx,
		CDayPInfo &objDayPInfo
		);


    ///////begin: 返回条目列表函数/////////////////////////////
    int CheckReportIdx(CListIdx &objListIdx);
    string GetStrCode(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrName(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrEarnOne1(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrEarnOne2(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrRevenue(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrRevenueRose1(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrRevenueRose2(CListIdx objListIdx, eItemType &eType, string &strRetCode);
    string GetStrNetProfit(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrNetProfitRose1(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrNetProfitRose2(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrNetAssertOne(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrNetAssertRate(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrCashFlowOne(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrGrossProfit(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrAllocationProfit(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrDividentRate(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrDeclarationDate(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetstrClosingDate(CListIdx objListIdx, eItemType &eType, string &strRetCode);

    int CheckDayIdx(CListIdx &objListIdx);
	string GetStrDate(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetStrClose(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetStrPE(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetStrPB(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetStrPR(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetStrPR2(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetStrDivideRate(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	string GetStrTotalValue(CListIdx objListIdx, eItemType &eType, string &strRetCode);

	string GetStrSubIndustry(CListIdx objListIdx, eItemType &eType, string &strRetCode);
	///////end: 返回条目列表函数//////////////////////////////////
	
};

class CIndustryItem
{
public:
	string m_strIndustry;
	int m_nCount;
	CIndustryItem()
	{
		m_strIndustry = "";
		m_nCount = 0;
	}
};

class CStockMap
{
public:
	static CStockMap* Instance();
	static void Remove();

	int Init();

	int GetStock(
		string strCode,
		StockData *&ptrStock
		);

	int GetReport(
		string strCode, 
		string strDate,
		ReportData &objReport
		);

	int GetSort(
		string strDate,
		string strItem,
		vector<string> vecSort
		);

	int ShowStockData(
		StockData* ptrStock
		);

	int GetSubIndustry(
		vector<CIndustryItem> *&ptrVecSubIndustry
		);

	map<string, StockData*> m_mapStock;
	vector<CIndustryItem> m_vecSubIndustry;

	bool m_bInited;
	string m_strEndDate;
	bool m_bReportDate;

private:
	CStockMap();
	static CStockMap *m_pInstance;

	int InitRawData();

	int TravelDir(
		string strDir,
		vector<string> &vecFile
		);

	int LoadFile(
		string strFile
		);

	int CheckDBData();
};

class SortDoubleItem
{
public:
	int m_idx;
	double m_dKey;
	int compare(SortDoubleItem objItem)
	{
		if (m_dKey > objItem.m_dKey)
			return 1;
		if (m_dKey < objItem.m_dKey)
			return -1;
		return 0;
	}
};

class SortIntItem
{
public:
	int m_idx;
	int m_iKey;
	int compare(SortIntItem objItem)
	{
		if (m_iKey > objItem.m_iKey)
			return 1;
		if (m_iKey < objItem.m_iKey)
			return -1;
		return 0;
	}
};

class SortStrItem
{
public:
	int m_idx;
	string m_strKey;
	int compare(SortStrItem objItem)
	{
		return m_strKey.compare(objItem.m_strKey);
	}
};

//////////////////begin: quick sort//////////////////////////////////////
template <class T>
int Partition(vector<T> &vecData, int iLow, int iHigh)
{
	T pivot = vecData[iLow];
	while (iLow < iHigh)
	{
		while (iLow<iHigh && pivot.compare(vecData[iHigh])<=0) --iHigh;
		vecData[iLow] = vecData[iHigh];
		while (iLow<iHigh && pivot.compare(vecData[iLow])>=0) ++iLow;
		vecData[iHigh] = vecData[iLow];
	}
	vecData[iLow] = pivot;

	return iLow;
}

template <class T>
void QSort(vector<T> &vecData, int iLow, int iHigh)
{
	if (iLow < iHigh)
	{
		int iPivot = Partition(vecData, iLow, iHigh);
		QSort(vecData, iLow, iPivot-1);
		QSort(vecData, iPivot+1, iHigh);
	}

}
//////////////////end: quick sort//////////////////////////////////////

//////////////////begin: leap sort/////////////////////////////////////
//构造最大堆
template <class T>
void MaxHeapFixDown(vector<T> &vecData, int nLow, int nHigh){
    int j = 2*nLow+1;
    T top = vecData[nLow];
    while(j<nHigh){
        if(j+1<nHigh&&vecData[j].compare(vecData[j+1])<0)
            ++j;
        if(top.compare(vecData[j])>0)
            break;
        else{
            vecData[nLow]=vecData[j];
            nLow=j;
            j=2*nLow+1;
        }
    }
    vecData[nLow]=top;
}

//堆排序,升序排列
template <class T>
void HeapSortAsc(vector<T> &vecData, int nTopCnt){
    for(int i= vecData.size()/2-1;i>=0;i--)
        MaxHeapFixDown(vecData, i, vecData.size());
    for(int i=vecData.size()-1;(i>=1)&&(vecData.size()-i<=nTopCnt);i--){
        swap(vecData[i],vecData[0]);
        MaxHeapFixDown(vecData,0,i);
    }
}
//////////////////end: leap sort/////////////////////////////////////

int DateStr2Int(string strDate);
string DateInt2Str(int iDate);
int GetNextDate(int iDate);
int GetPreDate(int iDate);
string GetNextDate(string strDate);
string GetPreDate(string strDate);

class COptionInfo
{
public:
	string m_strTab;
	string m_strCode;
	string m_strNote;
	int m_iCreateDate;

	COptionInfo()
	{
		m_strTab  = "";
		m_strCode = "";
		m_strNote = "";
		int m_iCreateDate = 0;
	}
};

class COptionMap
{
public:
	static COptionMap* Instance();

	int Init();
	

	int GetAll(
		string strTab,
		vector<string> &vecCode
		);

	bool CheckStock(
		string strCode
		);

	int GetStock(
		string strTab,
		string strCode,
		COptionInfo *&ptrOption
		);

	int ModifyStock(
		COptionInfo objOption
		);

	int AddStock(
		COptionInfo objOption
		);

	int DelStock(
		string strTab,
		string strCode
		);

	map<string, COptionInfo*> m_mapOption;
	map<string, set<string> > m_mapTabSets;

private:
	COptionMap();
	static COptionMap* m_pInstance;
};

// 价格复权用到的除权出息信息
class CCqcx   
{
public:
	int iDate;
	int iSzzbl;
};

#endif
