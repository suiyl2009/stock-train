#ifndef __BATCH_LINE_H__
#define __BATCH_LINE_H__

#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include <iostream>
#include <vector>
#include "Data.h"

using namespace std;

class CBatchQuart
{
public:
	string m_strCode;
	string m_strName;
	vector<ReportData> m_vecReport;

	CBatchQuart()
	{
	    m_strCode = "";
		m_strName = "";
		m_vecReport.clear();
	}
};

class CBatchDayPInfo
{
public:
	string m_strCode;
	string m_strName;
	vector<CDayPInfo> m_vecDayPInfo;

	CBatchDayPInfo()
	{
	    m_strCode = "";
		m_strName = "";
		m_vecDayPInfo.clear();
	}
};

class CBatchYVal
{
public:
	string m_strCode;
	string m_strName;
	int m_iMax;
	int m_iMin;
	vector<int> m_vecItems;
	
	CBatchYVal()
	{
	    m_strCode = "";
		m_strName = "";
	    m_iMax = 0;
		m_iMin = 0;
		m_vecItems.clear();
	}
};

class CBatchLine;

class CBValue
{
public:
	int Init(CBatchLine *ptrLine, string &strRetCode);
	virtual int Item(int i, int j) = 0;
	CBatchLine *m_ptrLine;
};

class CBRevenue : public CBValue
{
public:
	int Item(int i, int j);
};

class CBProfit : public CBValue
{
public:
	int Item(int i, int j);
};

class CBPrice : public CBValue
{
public:
	int Item(int i, int j);
};

class CBReinPrice : public CBValue
{
public:
	int Item(int i, int j);
};

class CBatchLine
{
public:
	CBatchLine();
	~CBatchLine();
	
	int GetLines(    // 得到单条或多条某一类型的曲线
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

	int GetSub(   // 得到一个细分行业某一类型的曲线
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

	int GetRevenueLine(string &strRetCode);
	int GetProfitLine(string &strRetCode);
	int GetROELine(string &strRetCode);
	int GetDivRateLine(string &strRetCode);
	int GetPRLine(string &strRetCode);
	int GetPR2Line(string &strRetCode);
    int GetPriceLine(string &strRetCode);
    int GetReinPriceLine(string &strRetCode);

//private:
	int Init(  // 根据传入的代码列表初始化
		vector<string> vecCode,
		string &strRetCode
		);

	int Init( // 根据传入的细分行业初始化
		string strSub,
		string &strRetCode
		);
	
	int GetQuartReport(
		string &strRetCode
		);
	
	int InfirmQuartIdx( // 确定显示区域坐标
		string &strRetCode
		);
	
	int CreateQuartLine(
		CBValue *ptrValue,
		string &strRetCode
		);
		
	int GetDayInfo(
		string &strRetCode
		);
	
	int InfirmDayIdx( // 确定显示区域坐标
		string &strRetCode
		);
	
	int CreateDayLine(
		CBValue *ptrValue,
		string &strRetCode
		);

	int ShowResult(
		json_t *jRes,
		string &strRetCode
		);

	// 初值及参数值
	vector<StockData *> m_vecStock;
	//vector<vector<ReportData> > m_vvQuartReport;
	//vector<vector<CDayPInfo> > m_vvDayInfo;
	vector<CBatchQuart> m_vecBatchQuart;
	vector<CBatchDayPInfo> m_vecBatchDay;
	int m_iDate;
	int m_iBeforeCnt;
	int m_iAfterCnt;

	// 显示区段坐标
	int m_idxBegin;
	int m_idxEnd;

    // 显示信息
    string m_strItem;
	vector<CBatchYVal> m_vecBatchYVal;
	vector<int> m_vecXVal;
	int m_iMaxYVal;
	int m_iMinYVal;
	
};

#endif


