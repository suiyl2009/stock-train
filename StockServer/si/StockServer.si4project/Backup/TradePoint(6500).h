#ifndef __TRADE_POINT_H__
#define __TRADE_POINT_H__

#include <iostream>
#include <vector>
#include "DBPool.h"
#include <jansson.h>
#include <set>

using namespace std;

struct SPointBi
{
    int m_nTrade;
    int m_nDate;
};

class CPointData
{
public:
    unsigned long long m_ullUserId;
    string m_strCode;
    int m_nBeginDate;
    int m_nEndData;
    vector<SPointBi> m_vecPoint;
    vector<int> m_vecCount;
    SPointBi* m_pPoint;
    int m_nSize;

    CPointData();
    ~CPointData();
    int Point2Vec(int nDate, set<int> setTrade);
    int Vec2Point();
};

class CPointStat
{
public:
    int m_nNum;
    int m_nDate;
    int m_nTrade;
    vector<string> m_vecCode;

    CPointStat()
    {
        m_nNum = 0;
        m_nDate = 0;
        m_nTrade = 0;
        m_vecCode.clear();
    }
};

class DataBuf : public std::streambuf
{
public:
	DataBuf(char* d, size_t s)
	{
		setg(d, d, d + s);
	}
};

class CTradePoint
{
public:
    int Insert(                 // 插入一个买卖点
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int Delete(                 // 删除一个买卖点
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int Search(                 // 查找某种类型的买卖点
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int Statistics(             // 统计某种类型买卖点
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

private:
    int SearchDBPoint(
        unsigned long long ullUID,
        string strCode,
        CPointData &objPointData,
        string &strRetCode
        );
        
    int SearchDBPoint(
        unsigned long long ullUID,
        vector<string> vecCode,
        vector<CPointData> &vecPointData,
        string &strRetCode
        );

    int UpdateDBPoint(
        CPointData objPoint,
        bool bExist,
        string &strRetCode
        );

    int CreateStat(
        const vector<CPointData> &vecPoint,
        string strCycle,
        map<unsigned long long, CPointStat> &mapStat,
        vector<int> &vecDate,
        string &strRetCode
        );

    int ShowInfo(
        const vector<CPointData> &vecPoint, 
        json_t *&jRes, 
        string &strRetCode
        );

    int ShowInfo(
        const map<unsigned long long, CPointStat> &mapStat,
        vector<int> &vecDate,
        json_t *&jRes,
        string &strRetCode
        );

    int GetUIDBySession(
        string strSession,
        unsigned long long &ullUID,
        string &strRetCode
        );
    
    Connection *m_pConn;
};

#endif
