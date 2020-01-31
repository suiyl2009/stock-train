#ifndef __K_TRAIN_H__
#define __K_TRAIN_H__

#include <iostream>
#include <vector>
#include "DBPool.h"
#include <jansson.h>

using namespace std;

class CTrainTrade
{
public:
    string m_strType;
    string m_strCode;
    int m_iNum;
    int m_iPrice;
    int m_iTax;
    int m_iDate;

    CTrainTrade()
    {
        m_strType     = "";
        m_strCode     = "";
        m_iNum        = 0;
        m_iPrice      = 0;
        m_iTax        = 0;
        m_iDate       = 0;
    }
};

class CTrainHold
{
public:
    string m_strCode;
    int m_iNum;
    int m_iPrice;

    CTrainHold()
    {
        m_strCode = "";
        m_iNum    = 0;
        m_iPrice  = 0;
    }
};

class CTrainInfo
{
public:
    long long m_llUID;
    string m_strCreateTime;
    int m_iEndDate;
    string m_strOperate;
    int m_iBeginFund;
    int m_iEndFund;
    int m_iTaxRate;
    int m_iBeginDate;
    string m_strHold;
    int m_iStatus;

    CTrainInfo();

    int CreateJson(
        const vector<CTrainTrade> &vecTrade,
        string &strRetCode
        );

    int CreateJson(
        const vector<CTrainHold> &vecHold,
        string &strRetCode
        );
    
    int ParseJson(
        vector<CTrainTrade> &vecTrade,
        string &strRetCode
        );
    
    int ParseJson(
        vector<CTrainHold> &vecHold,
        string &strRetCode
        );
};

class CTrainList
{
public:
    long long m_llUID;
    string m_strCreateTime;
    int m_iBeginDate;
    int m_iEndDate;
    int m_iBeginFund;
    int m_iEndFund;
    int m_iTaxRate;
    int m_iStatus;

    CTrainList()
    {
        m_llUID = 0;
        m_strCreateTime = "";
        m_iBeginDate = 0;
        m_iEndDate = 0;
        m_iBeginFund = 0;
        m_iEndFund = 0;
        m_iTaxRate = 0;
        m_iStatus = 0;
    }
};

class CKTrain
{
public:
    int Begin(                 // 开始一个新的训练
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int Buy(                   // 执行一笔购买交易
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int Sale(                  // 执行一笔卖出交易
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int End(                   // 结束一个训练
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int List(                  // 训练列表
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int Search(                // 查询训练信息
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

private:
    
    int SearchDBCurr(
        long long llUID,
        bool &bExist,
        CTrainInfo &objTrainInfo,
        string &strRetCode
        );

    int BeginDBCurr(
        CTrainInfo &objTrainInfo, 
        bool bExist,
        string &strRetCode
        );

    int TradeBuy(
        string strCode, 
        int iNum, 
        int iPrice, 
        int iDate, 
        CTrainInfo &objTrainInfo, 
        string &strRetCode
        );

    int TradeSale(
        string strCode, 
        int iNum, 
        int iPrice, 
        int iDate, 
        CTrainInfo &objTrainInfo, 
        string &strRetCode
        );

    int UpdateDBCurr(
        CTrainInfo &objTrainInfo, 
        string &strRetCode
        );

    int SearchDBHistory(
        long long llUID, 
        string strCreateTime, 
        bool &bExist, 
        CTrainInfo &objTrainInfo, 
        string &strRetCode
        );

    int FinishDBCurr(
        CTrainInfo &objTrainInfo, 
        string &strRetCode
        );

    int SearchDBList(
        long long llUID, 
        string strCreateTime,
        int iNum,
        vector<CTrainList> &vecList, 
        string &strRetCode
        );

    int ShowTrainInfo(
        CTrainInfo &objTrainInfo, 
        json_t *&jRes, 
        string &strRetCode
        );

    int ShowTrainList(
        vector<CTrainList> vecList, 
        json_t *&jRes, 
        string &strRetCode
        );

    int GetUIDBySession(
        string strSession,
        long long &llUID,
        string &strRetCode
        );

    Connection *m_pConn;
    //CTrainInfo m_objInfo;
    //vector<CTrainList> m_vecList;
};

#endif

