#ifndef __COND_LIST_H__
#define __COND_LIST_H__

#include <iostream>
#include <vector>
#include <jansson.h>
#include "Data.h"

using namespace std;

class CCondValue
{
public:
	StockData *m_ptrStock;
    CListIdx m_objListIdx;
    bool m_bReport;

	CCondValue();

	int Init(
		StockData *ptrStock,
		CListIdx objListIdx
		);

	virtual int GetItem(
		int idx,
		int &iValue,
		int iType = 0
		);
};

class CROECond: public CCondValue
{
public:
	CROECond();
	
	int GetItem(
		int idx,
		int &iValue,
		int iType = 0
		);
};
CCondValue* GetROECond(){return new CROECond;}

class CPrCond : public CCondValue
{
public:
	CPrCond();
	
	int GetItem(
		int idx,
		int &iValue,
		int iType = 0
		);
};
CCondValue* GetPrCond(){return new CPrCond;}

class CClosingDateCond : public CCondValue
{
public:
    CClosingDateCond();
    
    int GetItem(
        int idx,
        int &iValue,
        int iType = 0
        );
};
CCondValue* GetClosingDateCond(){return new CClosingDateCond;}

class CDeclareDateCond : public CCondValue
{
public:
    CDeclareDateCond();
    
    int GetItem(
        int idx,
        int &iValue,
        int iType = 0
        );
};
CCondValue* GetDeclareDateCond(){return new CDeclareDateCond;}

class CListIdxStock
{
public:
    StockData *m_pStock;
    CListIdx m_objIdx;
};

class CCondResult
{
public:
	string m_strCode;
	string m_strName;
	int m_iValue;
	int m_iMatch;
	int m_iDate;
	int m_iRank;

	int compare(CCondResult objItem)
	{
		if (m_iValue > objItem.m_iValue)
			return 1;
		if (m_iValue < objItem.m_iValue)
			return -1;
		return 0;
	}
};

class CCondList
{
public:
	CCondList();
	~CCondList();
	
    int Filter(
        json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

    int Compare(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int SubLeader( // roe, profit top 10; 1/3roe,1/3profit,1/3revenue
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );


    static const int m_scnSubLeaderNum = 3;  // 每个细分行业最多取3个
    static const int m_scnSubLeaderTop = 10; // 每个细分行业参与排序的个数
private:    
    int Init(
        vector<string> vecCode,
        int iBeginDate,
        string &strRetCode
        );

    int FilterReport(
        int iTraverseCnt,
        int iMatchCnt,
        bool bGreater,
        int iValue,
        string &strRetCode
        );

    int FilterPrice(
        int iTraverseCnt,
        int iMatchCnt,
        bool bGreater,
        int iValue,
        string &strRetCode
        );

    int CompareReport(
		bool bGreater,
        int iValue,
        string &strRetCode
        );

    int ComparePrice(
		bool bGreater,
        int iValue,
        string &strRetCode
        );

    int SelectLeader(
        vector<CListIdxStock> &vecListIdxStock, 
        vector<string> &vecCode,
        string &strRetCode
        );
    
	int ShowFilterResult(
		json_t *&jRes,
		string &strRetCode
		);

	int ShowCompareResult(
		json_t *&jRes,
		string &strRetCode
		);

    int ShowCodeListResult(
        json_t *&jRes,
        const vector<string> &vecCode,
		string &strRetCode
        );
    
    int PushItem(
		StockData *ptrStock,
		int iDate,
		bool bReportDate,
		string &strRetCode
		);

    vector<StockData*> m_vecStock;
	vector<CListIdx> m_vecIdx;
    CCondValue *m_ptrCondValue;
	vector<CCondResult> m_vecResult;
};

#endif
