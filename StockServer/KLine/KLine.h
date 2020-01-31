#ifndef __K_LINE_H__
#define __K_LINE_H__

#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include <iostream>
#include <vector>
#include "Data.h"

using namespace std;

class CKData     // 每日原始价格信息
{
public:
	int date;      // 年月日
	int open;      // 开盘价
	int high;      // 最高价
	int low;       // 最低价
	int close;     // 收盘价
	double amount;  // 成交额（元）
	long long vol;       // 成交量（手）
	int dayCnt;

    CKData()
    {
        date    = 0;
        open    = 0;
        high    = 0;
        low     = 0;
        close   = 0;
        amount  = 0;
        vol     = 0;
        dayCnt  = 0;
    }
};

class CTotalCnt
{
public:
    int date;
    int count;

    CTotalCnt()
    {
        date   = 0;
        count  = 0;
    }
};

class CKLine
{
public:
	CKLine();
	~CKLine();
	
	int GetLines(    // 得到单条或多条曲线
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

	int GetDayLine(string &strRetCode);
    int GetWeekLine(string &strRetCode);
    int GetMonthLine(string &strRetCode);
    int GetYearLine(string &strRetCode);

private:
	int Init(
		StockData *ptrStock,
		string &strRetCode
		);

	int InfirmPivotIdx(
		int iDate,
		int &idxPivot,
		string &strRetCode
		);

	int GetDayData(string &strRetCode);

    int Recover(
        string &strRetCode
        );

    int GetWeekData(
		string &strRetCode
		);

	int GetMonthData(
		string &strRetCode
		);
	
	int GetYearData(
		string &strRetCode
		);
	
	int ShowResult(
		json_t *jRes,
		string &strRetCode
		);

	// 初值及参数值
	StockData *m_ptrStock;
    string m_strCycle;
    int m_iRecover;
    
	int m_iDate;
	int m_iBeforeCnt;
	int m_iAfterCnt;

	// 显示区段坐标
	int m_idxPivot;
	int m_idxDBegin;
	int m_idxDEnd;

    // 显示信息
    vector<CKData> m_vecKLine;
    vector<CTotalCnt> m_vecTotalCnt;
    vector<int> m_vecReportDate;
	int m_iKMaxVal;
	int m_iKMinVal;
	long long m_iVolMax;
	long long m_iVolMin;
	
};

#endif

