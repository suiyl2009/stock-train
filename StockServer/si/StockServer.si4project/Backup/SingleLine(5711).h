#ifndef __SINGLE_LINE_H__
#define __SINGLE_LINE_H__

#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include <iostream>
#include <vector>
#include "Data.h"

using namespace std;

class CYVal
{
public:
	int m_iMax;
	int m_iMin;
	vector<int> m_vecItems;
	
	CYVal()
	{
	    m_iMax = 0;
		m_iMin = 0;
		m_vecItems.clear();
	}
};

class CSingleLine
{
public:
	CSingleLine();
	~CSingleLine();

	friend class CBatchLine;
	
	int GetLines(    // 得到单条或多条曲线
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

	int GetRevenueYLine(int &iMax, int &iMin, string &strRetCode);
	int GetProfitYLine(int &iMax, int &iMin, string &strRetCode);
	int GetROEYLine(int &iMax, int &iMin, string &strRetCode);
	int GetDivRateYLine(int &iMax, int &iMin, string &strRetCode);
	int GetPRYLine(int &iMax, int &iMin, string &strRetCode);
	int GetPR2YLine(int &iMax, int &iMin, string &strRetCode);
	int GetPriceYLine(int &iMax, int &iMin, string &strRetCode);
	int GetReinPriceYLine(int &iMax, int &iMin, string &strRetCode);
	int GetTotalValYLine(int &iMax, int &iMin, string &strRetCode);
	int GetCoveryYLine(int &iMax, int &iMin, string &strRetCode);
	int GetROPYLine(int &iMax, int &iMin, string &strRetCode);
    int GetPPYLine(int &iMax, int &iMin, string &strRetCode);

private:
	int Init(
		StockData *ptrStock
		);
	
	int GetQuartReport();
	int GetDayInfo();
	int GetMapIndex(); // 补齐report记录
	int InfirmShowIdx( // 确定显示区域坐标
		int iDate,
		int iBeforeCnt,
		int iAfterCnt,
		string &strRetCode
		);

	int CreateLine(
		const vector<string> &vecItems, 
		string &strRetCode
		);
	
    int AdjustYVal(string &strRetCode);
	int CreateXLine(string &strRetCode);

	int ShowResult(
		json_t *jRes,
		string &strRetCode
		);

	// 初值及参数值
	StockData *m_ptrStock;
	vector<ReportData> m_vecQuartReport;
	vector<CDayPInfo> m_vecDayInfo;
	int m_iDate;
	int m_iBeforeCnt;
	int m_iAfterCnt;

	// 显示区段坐标
	int m_idxQBegin;
	int m_idxQEnd;
	int m_idxDBegin;
	int m_idxDEnd;

    // 显示信息
	map<string, CYVal*> m_mapQYVal;
	map<string, CYVal*> m_mapDYVal;
	vector<int> m_vecQXVal;
	vector<int> m_vecIdx;
	vector<int> m_vecDXVal;
	int m_iMaxYVal;
	int m_iMinYVal;
	
};

#endif

