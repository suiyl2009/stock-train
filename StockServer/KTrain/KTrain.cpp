#include "KTrain.h"
#include "Constant.h"
#include "Log.h"
#include "StrSplit.h"

CTrainInfo::CTrainInfo()
{
    m_llUID          = 0;
    m_strCreateTime  = "";
    m_iEndDate       = 0;
    m_strOperate     = "";
    m_iBeginFund     = 0;
    m_iEndFund       = 0;
    m_iTaxRate       = 0;
    m_iBeginDate     = 0;
    m_strHold        = "";
    m_iStatus        = 0;
}

int CTrainInfo::CreateJson(
    const vector<CTrainTrade> &vecOperate,
    string &strRetCode
    )
{
    //json_t *jRoot    = NULL;
    json_t *jOperate = NULL;
    json_t *jItem    = NULL;
    char *reply_str  = NULL;
    size_t sJsonSize = 0;
    CTrainTrade objOperate;

    ////////////////////begin: operate 字段处理//////////////////////
    //jRoot = json_object();
    jOperate = json_array();
    for (int i=0; i<vecOperate.size(); i++)
	{
	    objOperate = vecOperate[i];
		jItem = json_object();
        json_object_set_new(jItem, "type",   json_string(objOperate.m_strType.c_str()));
        json_object_set_new(jItem, "code",   json_string(objOperate.m_strCode.c_str()));
		json_object_set_new(jItem, "num",    json_integer(objOperate.m_iNum));
        json_object_set_new(jItem, "price",  json_integer(objOperate.m_iPrice));
        json_object_set_new(jItem, "tax",    json_integer(objOperate.m_iTax));
        json_object_set_new(jItem, "date",   json_integer(objOperate.m_iDate));
		json_array_append_new(jOperate, jItem);
	}
    //json_object_set_new(jRoot, "operate", jOperate);

	//reply_str = json_dumps(jRoot, 0);
	reply_str = json_dumps(jOperate, 0);
	m_strOperate = reply_str;
	free(reply_str);
	//json_decref(jRoot);
	json_decref(jOperate);
    ////////////////////end: operate 字段处理////////////////////////

    return 0;
}

int CTrainInfo::CreateJson(
    const vector<CTrainHold> &vecHold,
    string &strRetCode
    )
{
    //json_t *jRoot    = NULL;
    json_t *jOperate = NULL;
    json_t *jItem    = NULL;
    char *reply_str  = NULL;
    size_t sJsonSize = 0;
    CTrainHold objHold;

    ////////////////////begin: hold 字段处理///////////////////////
    //jRoot = json_object();
    jOperate = json_array();
    for (int i=0; i<vecHold.size(); i++)
    {
        objHold = vecHold[i];
        jItem = json_object();
        json_object_set_new(jItem, "code",   json_string(objHold.m_strCode.c_str()));
        json_object_set_new(jItem, "num",    json_integer(objHold.m_iNum));
        json_object_set_new(jItem, "price",  json_integer(objHold.m_iPrice));
        json_array_append_new(jOperate, jItem);
    }
    //json_object_set_new(jRoot, "hold", jOperate);
    
    //reply_str = json_dumps(jRoot, 0);
    reply_str = json_dumps(jOperate, 0);
    m_strHold = reply_str;
    free(reply_str);
    //json_decref(jRoot);
    json_decref(jOperate);
    ////////////////////end: hold 字段处理/////////////////////////

    return 0;
}

int CTrainInfo::ParseJson(
    vector<CTrainTrade> &vecTrade,
    string &strRetCode
    )
{
    //json_t *jRoot    = NULL;
    json_t *jOperate = NULL;
    json_t *jItem    = NULL;
    char *reply_str  = NULL;
    size_t sJsonSize = 0;
    CTrainTrade objOperate;
    
    ////////////////////begin: operate 字段处理//////////////////////
    vecTrade.clear();
    if (!m_strOperate.empty())
    {
        //jRoot = json_loadb(m_strOperate.c_str(), m_strOperate.length(), 0, NULL);
        //jOperate = json_object_get(jRoot, "operate");
        jOperate = json_loadb(m_strOperate.c_str(), m_strOperate.length(), 0, NULL);
        if (!jOperate || !json_is_array(jOperate)) {
            strRetCode = CErrJsonType;
            ERROR("GetListAll Get json item error. strRetCode: "<<strRetCode);
            //json_decref(jRoot);
            json_decref(jOperate);
            return 1;
        }
        sJsonSize = json_array_size(jOperate);
        for (int i=0; i<sJsonSize; i++)
        {
            jItem = json_array_get(jOperate, i);
            objOperate.m_strType = json_string_value(json_object_get(jItem, "type"));
            objOperate.m_strCode = json_string_value(json_object_get(jItem, "code"));
            objOperate.m_iNum    = json_integer_value(json_object_get(jItem, "num"));
            objOperate.m_iPrice  = json_integer_value(json_object_get(jItem, "price"));
            objOperate.m_iTax    = json_integer_value(json_object_get(jItem, "tax"));
            objOperate.m_iDate   = json_integer_value(json_object_get(jItem, "date"));
            vecTrade.push_back(objOperate);
        }
        //json_decref(jRoot);
        json_decref(jOperate);
    }
    ////////////////////end: operate 字段处理//////////////////////

    return 0;
}

int CTrainInfo::ParseJson(
    vector<CTrainHold> &vecHold,
    string &strRetCode
    )
{
    //json_t *jRoot    = NULL;
    json_t *jOperate = NULL;
    json_t *jItem    = NULL;
    char *reply_str  = NULL;
    size_t sJsonSize = 0;
    CTrainHold objHold;

    ////////////////////begin: hold 字段处理///////////////////////
     vecHold.clear();
     if (!m_strHold.empty())
     {
         //jRoot = json_loadb(m_strHold.c_str(), m_strHold.length(), 0, NULL);
         //jOperate = json_object_get(jRoot, "hold");
         jOperate = json_loadb(m_strHold.c_str(), m_strHold.length(), 0, NULL);
         if (!jOperate || !json_is_array(jOperate)) {
             strRetCode = CErrJsonType;
             ERROR("GetListAll Get json item error. strRetCode: "<<strRetCode);
             //json_decref(jRoot);
             json_decref(jOperate);
             return 1;
         }
         sJsonSize = json_array_size(jOperate);
         for (int i=0; i<sJsonSize; i++)
         {
             jItem = json_array_get(jOperate, i);
             objHold.m_strCode = json_string_value(json_object_get(jItem, "code"));
             objHold.m_iNum    = json_integer_value(json_object_get(jItem, "num"));
             objHold.m_iPrice  = json_integer_value(json_object_get(jItem, "price"));
             vecHold.push_back(objHold);
         }
         //json_decref(jRoot);
         json_decref(jOperate);
     }
     ////////////////////end: hold 字段处理/////////////////////////

     return 0;
}

#define CATCH_DB catch (sql::SQLException ex) \
  { \
    strRetCode = CErrDBCatch; \
    ERROR( "sql::SQLException:" << ex.what()<<", strRetCode: "<<strRetCode); \
    return 1; \
  }   \
  catch (exception ex) \
  { \
    strRetCode = CErrDBCatch; \
    ERROR( "unknown Exception:" << ex.what()<<", strRetCode: "<<strRetCode); \
    return 1; \
  } 

int CKTrain::Begin(        // 开始一个新的训练
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode     // 返回码
    )
{
    long long llUID    = 0;       // 用户编号
    string strSession  = "";
    int iBeginDate     = 0;       // 开始日期
    int iBeginFund     = 0;       // 起始资金
    int iTaxRate       = 0;       // 交易税率
    json_t *jItem      = NULL;
    CTrainInfo objTrainInfo;

    int iRet = 0;
    bool bExist = true;

    /////////begin: 解析json，得到各字段item//////////////////////
    jItem = json_object_get(jReq, "userid");
    /*if (!jItem || !json_is_integer(jItem)) {
        strRetCode = CErrJsonType;
        ERROR("GetLines json userid is not int. strRetCode: "<<strRetCode);
        return 1;
    }
    ullUserId = json_integer_value(jItem);
    if (0 == ullUserId)
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines json userid value error. userid: "<<ullUserId<<", strRetCode: "<<strRetCode);
        return 1;
    }*/
    if (jItem && json_is_integer(jItem)) {
        llUID = json_integer_value(jItem);
    }

    jItem = json_object_get(jReq, "session");
	if (jItem && json_is_string(jItem)) {
        strSession = json_string_value(jItem);
        strSession = trim(strSession);
	}

	
    jItem = json_object_get(jReq, "begindate");
    if (!jItem || !json_is_integer(jItem)) 
	{
		strRetCode = CErrJsonType;
	    ERROR("GetLines json date is not int. strRetCode: "<<strRetCode);
		return 1;
    }
    iBeginDate = json_integer_value(json_object_get(jReq, "begindate"));
    if (((0 != iBeginDate) && (iBeginDate < 19800000)) || (iBeginDate > 99999999))
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", iBeginDate: "<<iBeginDate);
        return 1;
    }

    jItem = json_object_get(jReq, "beginfund");
    if (!jItem || !json_is_integer(jItem)) 
	{
		strRetCode = CErrJsonType;
	    ERROR("GetLines json beginfund is not int. strRetCode: "<<strRetCode);
		return 1;
    }
    iBeginFund = json_integer_value(json_object_get(jReq, "beginfund"));
    if (iBeginFund <= 0)
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines Get json beginfund value error. strRetCode: "<<strRetCode<<", iBeginFund: "<<iBeginFund);
        return 1;
    }

    jItem = json_object_get(jReq, "taxrate");
    if (jItem && json_is_integer(jItem)) 
	{
		iTaxRate = json_integer_value(json_object_get(jReq, "taxrate"));
		if (iTaxRate < 0)
		{
	        strRetCode = CErrJsonType;
			ERROR("GetLines Get json taxrate value error. strRetCode: "<<strRetCode<<", iTaxRate: "<<iTaxRate);
			return 1;
		}
    }
    /////////end: 解析json，得到各字段item//////////////////////

    m_pConn = CDBPool::Instance()->GetConnect();
    if (NULL == m_pConn)
    {
        strRetCode = CErrDBConn;
        ERROR("CKTrain::GetDBConnect Call CDBPool::Instance()->GetConnect() error. strRetCode: "<<strRetCode);
        return 1;
    }

    do
    {
        if (0 == llUID)
        {
        	iRet = GetUIDBySession(strSession, llUID, strRetCode);
    		if (0 != iRet)
    		{
    			ERROR("Call GetUIDBySession error. strSession: " << strSession << ", strRetCode: " << strRetCode);
    			break;
    		}
        }
            
        iRet = SearchDBCurr(llUID, bExist, objTrainInfo, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call SearchCurr error. llUID: "<<llUID<<", strRetCode: "<<strRetCode
                <<", iRet: "<<iRet);
            break;
        }
        
        if ((true == bExist) && (0 == objTrainInfo.m_iStatus)) // 在数据库中存在记录并且没有结束
        {
            DEBUG("Has current train not finished. llUID: "<<llUID);
            iRet = ShowTrainInfo(objTrainInfo, jRes, strRetCode);
            if (0 != iRet)
            {
                ERROR("Call ShowTrainInfo error. strRetCode: "<<strRetCode);
                break;
            }
            break;
        }

        objTrainInfo.m_llUID       = llUID;
        objTrainInfo.m_iBeginDate  = iBeginDate;
        objTrainInfo.m_iEndDate    = iBeginDate;
        objTrainInfo.m_iBeginFund  = iBeginFund;
        objTrainInfo.m_iEndFund    = iBeginFund;
        objTrainInfo.m_iTaxRate    = iTaxRate;
        objTrainInfo.m_iStatus     = 0;

        iRet = BeginDBCurr(objTrainInfo, bExist, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call MofifyCurr error. llUID: "<<llUID<<", iBeginDate: "<<iBeginDate<<", iBeginFund: "<<iBeginFund
                <<", iTaxRate: "<<iTaxRate);
            break;
        }

        iRet = ShowTrainInfo(objTrainInfo, jRes, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call ShowTrainInfo error. strRetCode: "<<strRetCode);
            break;
        }
    }
    while (false);

    CDBPool::Instance()->PushConnect(m_pConn);
    
    return iRet;
}

int CKTrain::Buy(          // 执行一笔购买交易
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode     // 返回码
    )
{
    long long llUID    = 0;       // 用户编号
    string strSession  = "";
    string strCode     = "";      // 股票代码
    int iNum           = 0;       // 购买数量
    int iPrice         = 0;       // 股票单价
    int iDate          = 0;       // 交易日期
    json_t *jItem      = NULL;
    CTrainInfo objTrainInfo;

    int iRet = 0;
    bool bExist = true;

    /////////begin: parameter//////////////////////
    jItem = json_object_get(jReq, "userid");
    /*if (!jItem || !json_is_integer(jItem)) {
        strRetCode = CErrJsonType;
        ERROR("GetLines json userid is not int. strRetCode: "<<strRetCode);
        return 1;
    }
    ullUserId = json_integer_value(jItem);
    if (0 == ullUserId)
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines json userid value error. userid: "<<ullUserId<<", strRetCode: "<<strRetCode);
        return 1;
    }*/
    if (jItem && json_is_integer(jItem)) {
        llUID = json_integer_value(jItem);
    }

    jItem = json_object_get(jReq, "session");
	if (jItem && json_is_string(jItem)) {
        strSession = json_string_value(jItem);
        strSession = trim(strSession);
	}

	jItem = json_object_get(jReq, "code");
    if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
	    ERROR("GetLines json code is not string. strRetCode: "<<strRetCode);
		return 1;
    }
	strCode = json_string_value(jItem);

    jItem = json_object_get(jReq, "num");
    if (!jItem || !json_is_integer(jItem)) 
	{
		strRetCode = CErrJsonType;
	    ERROR("GetLines json num is not int. strRetCode: "<<strRetCode);
		return 1;
    }
	iNum = json_integer_value(json_object_get(jReq, "num"));
	if ((iNum <= 0) || (0 != iNum%100))
	{
        strRetCode = CErrJsonType;
		ERROR("GetLines Get json num value error. strRetCode: "<<strRetCode<<", iNum: "<<iNum);
		return 1;
	}
        
    jItem = json_object_get(jReq, "price");
    if (!jItem || !json_is_integer(jItem)) 
	{
		strRetCode = CErrJsonType;
	    ERROR("GetLines json price is not int. strRetCode: "<<strRetCode);
		return 1;
    }
    iPrice = json_integer_value(json_object_get(jReq, "price"));
    if (iPrice <= 0)
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines Get json price value error. strRetCode: "<<strRetCode<<", iPrice: "<<iPrice);
        return 1;
    }

    jItem = json_object_get(jReq, "date");
    if (!jItem || !json_is_integer(jItem)) 
	{
		strRetCode = CErrJsonType;
	    ERROR("GetLines json date is not int. strRetCode: "<<strRetCode);
		return 1;
    }
    iDate = json_integer_value(json_object_get(jReq, "date"));
	if (((0 != iDate) && (iDate < 19800000)) || (iDate > 99999999))
	{
        strRetCode = CErrJsonType;
		ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", iDate: "<<iDate);
		return 1;
	}
    /////////end: parameter//////////////////////

    m_pConn = CDBPool::Instance()->GetConnect();
    if (NULL == m_pConn)
    {
        strRetCode = CErrDBConn;
        ERROR("CKTrain::GetDBConnect Call CDBPool::Instance()->GetConnect() error. strRetCode: "<<strRetCode);
        return 1;
    }

    do
    {
        if (0 == llUID)
        {
        	iRet = GetUIDBySession(strSession, llUID, strRetCode);
    		if (0 != iRet)
    		{
    			ERROR("Call GetUIDBySession error. strSession: " << strSession << ", strRetCode: " << strRetCode);
    			break;
    		}
        }
        
        iRet = SearchDBCurr(llUID, bExist, objTrainInfo, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call SearchCurr error. llUID: "<<llUID<<", strRetCode: "<<strRetCode
                <<", iRet: "<<iRet);
            break;
        }
        
        if ((false == bExist) || (1 == objTrainInfo.m_iStatus)) // 在数据库中不存在记录或已经结束
        {
            iRet = 1;
            strRetCode = CErrDBNotExist;
            ERROR("Hasn't exist current train. llUID: "<<llUID<<", strRetCode: "<<strRetCode);
            break;
        }

        iRet = TradeBuy(strCode, iNum, iPrice, iDate, objTrainInfo, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call MofifyCurr error. llUID: "<<llUID<<", iDate: "<<iDate);
            break;
        }

        iRet = UpdateDBCurr(objTrainInfo, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call UpdateDBCurr error. llUID: "<<llUID<<", iDate: "<<iDate);
            break;
        }

        iRet = ShowTrainInfo(objTrainInfo, jRes, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call ShowTrainInfo error. strRetCode: "<<strRetCode);
            break;
        }
    }
    while (false);

    CDBPool::Instance()->PushConnect(m_pConn);
    
    return iRet;

}

int CKTrain::Sale(         // 执行一笔卖出交易
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode     // 返回码
    )
{
    long long llUID    = 0;       // 用户编号
    string strSession  = "";
    string strCode     = "";      // 股票代码
    int iNum           = 0;       // 购买数量
    int iPrice         = 0;       // 股票单价
    int iDate          = 0;       // 交易日期
    json_t *jItem      = NULL;
    CTrainInfo objTrainInfo;

    int iRet = 0;
    bool bExist = true;

    /////////begin: parameter//////////////////////
    jItem = json_object_get(jReq, "userid");
    /*if (!jItem || !json_is_integer(jItem)) {
        strRetCode = CErrJsonType;
        ERROR("GetLines json userid is not int. strRetCode: "<<strRetCode);
        return 1;
    }
    ullUserId = json_integer_value(jItem);
    if (0 == ullUserId)
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines json userid value error. userid: "<<ullUserId<<", strRetCode: "<<strRetCode);
        return 1;
    }*/
    if (jItem && json_is_integer(jItem)) {
        llUID = json_integer_value(jItem);
    }

    jItem = json_object_get(jReq, "session");
	if (jItem && json_is_string(jItem)) {
        strSession = json_string_value(jItem);
        strSession = trim(strSession);
	}

	jItem = json_object_get(jReq, "code");
    if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
	    ERROR("GetLines json code is not string. strRetCode: "<<strRetCode);
		return 1;
    }
	strCode = json_string_value(jItem);

    jItem = json_object_get(jReq, "num");
    if (!jItem || !json_is_integer(jItem)) 
	{
		strRetCode = CErrJsonType;
	    ERROR("GetLines json num is not int. strRetCode: "<<strRetCode);
		return 1;
    }
	iNum = json_integer_value(json_object_get(jReq, "num"));
	if ((iNum <= 0) || (0 != iNum%100))
	{
        strRetCode = CErrJsonType;
		ERROR("GetLines Get json num value error. strRetCode: "<<strRetCode<<", iNum: "<<iNum);
		return 1;
	}
        
    jItem = json_object_get(jReq, "price");
    if (!jItem || !json_is_integer(jItem)) 
	{
		strRetCode = CErrJsonType;
	    ERROR("GetLines json price is not int. strRetCode: "<<strRetCode);
		return 1;
    }
    iPrice = json_integer_value(json_object_get(jReq, "price"));
    if (iPrice <= 0)
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines Get json price value error. strRetCode: "<<strRetCode<<", iPrice: "<<iPrice);
        return 1;
    }

    jItem = json_object_get(jReq, "date");
    if (!jItem || !json_is_integer(jItem)) 
	{
		strRetCode = CErrJsonType;
	    ERROR("GetLines json date is not int. strRetCode: "<<strRetCode);
		return 1;
    }
    iDate = json_integer_value(json_object_get(jReq, "date"));
	if (((0 != iDate) && (iDate < 19800000)) || (iDate > 99999999))
	{
        strRetCode = CErrJsonType;
		ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", iDate: "<<iDate);
		return 1;
	}
    /////////end: parameter//////////////////////

    m_pConn = CDBPool::Instance()->GetConnect();
    if (NULL == m_pConn)
    {
        strRetCode = CErrDBConn;
        ERROR("CKTrain::GetDBConnect Call CDBPool::Instance()->GetConnect() error. strRetCode: "<<strRetCode);
        return 1;
    }

    do
    {
        if (0 == llUID)
        {
        	iRet = GetUIDBySession(strSession, llUID, strRetCode);
    		if (0 != iRet)
    		{
    			ERROR("Call GetUIDBySession error. strSession: " << strSession << ", strRetCode: " << strRetCode);
    			break;
    		}
        }
        
        iRet = SearchDBCurr(llUID, bExist, objTrainInfo, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call SearchCurr error. llUID: "<<llUID<<", strRetCode: "<<strRetCode
                <<", iRet: "<<iRet);
            break;
        }
        
        if ((false == bExist) || (1 == objTrainInfo.m_iStatus)) // 在数据库中不存在记录或已经结束
        {
            iRet = 1;
            strRetCode = CErrDBNotExist;
            ERROR("Hasn't exist current train. llUID: "<<llUID<<", strRetCode: "<<strRetCode);
            break;
        }

        iRet = TradeSale(strCode, iNum, iPrice, iDate, objTrainInfo, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call MofifyCurr error. llUID: "<<llUID<<", iDate: "<<iDate);
            break;
        }

        iRet = UpdateDBCurr(objTrainInfo, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call UpdateDBCurr error. llUID: "<<llUID<<", iDate: "<<iDate);
            break;
        }

        iRet = ShowTrainInfo(objTrainInfo, jRes, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call ShowTrainInfo error. strRetCode: "<<strRetCode);
            break;
        }
    }
    while (false);

    CDBPool::Instance()->PushConnect(m_pConn);
    
    return iRet;
}

int CKTrain::End(          // 结束一个训练
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode     // 返回码
    )
{
    long long llUID    = 0;       // 用户编号
    string strSession  = "";
    int iEndDate       = 0;
    json_t *jItem      = NULL;
    vector <CTrainHold> vecHold;
    CTrainInfo objTrainInfo;

    int iRet = 0;
    bool bExist = true;

        /////////begin: parameter//////////////////////
    jItem = json_object_get(jReq, "userid");
     /*if (!jItem || !json_is_integer(jItem)) {
         strRetCode = CErrJsonType;
         ERROR("GetLines json userid is not int. strRetCode: "<<strRetCode);
         return 1;
     }
     ullUserId = json_integer_value(jItem);
     if (0 == ullUserId)
     {
         strRetCode = CErrJsonType;
         ERROR("GetLines json userid value error. userid: "<<ullUserId<<", strRetCode: "<<strRetCode);
         return 1;
     }*/
     if (jItem && json_is_integer(jItem)) {
         llUID = json_integer_value(jItem);
     }
    
     jItem = json_object_get(jReq, "session");
     if (jItem && json_is_string(jItem)) {
         strSession = json_string_value(jItem);
         strSession = trim(strSession);
     }
	
    jItem = json_object_get(jReq, "enddate");
    if (!jItem || !json_is_integer(jItem)) 
	{
		strRetCode = CErrJsonType;
	    ERROR("GetLines json date is not int. strRetCode: "<<strRetCode);
		return 1;
    }
    iEndDate = json_integer_value(json_object_get(jReq, "enddate"));
    if (((0 != iEndDate) && (iEndDate < 19800000)) || (iEndDate > 99999999))
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines Get json date value error. strRetCode: "<<strRetCode<<", iEndDate: "<<iEndDate);
        return 1;
    }
    /////////end: parameter//////////////////////
    
    m_pConn = CDBPool::Instance()->GetConnect();
    if (NULL == m_pConn)
    {
        strRetCode = CErrDBConn;
        ERROR("CKTrain::GetDBConnect Call CDBPool::Instance()->GetConnect() error. strRetCode: "<<strRetCode);
        return 1;
    }

    do
    {
        if (0 == llUID)
        {
        	iRet = GetUIDBySession(strSession, llUID, strRetCode);
    		if (0 != iRet)
    		{
    			ERROR("Call GetUIDBySession error. strSession: " << strSession << ", strRetCode: " << strRetCode);
    			break;
    		}
        }
            
        iRet = SearchDBCurr(llUID, bExist, objTrainInfo, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call SearchCurr error. llUID: "<<llUID<<", strRetCode: "<<strRetCode
                <<", iRet: "<<iRet);
            break;
        }

        iRet = objTrainInfo.ParseJson(vecHold, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call objTrainInfo.ParseJson error. llUID: "<<llUID<<", strRetCode: "<<strRetCode);
            break; 
        }
        
        if ((false == bExist)) // || (vecHold.size() > 0)) // 在数据库中不存在记录或有未卖出交易
        {
            iRet = 1;
            strRetCode = CErrDBNotExist;
            ERROR("Has current train not finished. llUID: "<<llUID<<", strRetCode: "<<strRetCode);
            break;
        }
        
        objTrainInfo.m_iEndDate = iEndDate;
        iRet = FinishDBCurr(objTrainInfo, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call FinishCurr error. llUID: "<<llUID);
            break;
        }

        iRet = ShowTrainInfo(objTrainInfo, jRes, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call ShowTrainInfo error. strRetCode: "<<strRetCode);
            break;
        }
    }
    while (false);

    CDBPool::Instance()->PushConnect(m_pConn);
    
    return iRet;
}

int CKTrain::List(         // 训练列表
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode     // 返回码
    )
{
    long long llUID       = 0;       // 用户编号
    string strCreateTime  = "";      // 创建日期
    int iNum              = 0;       // 列表条数
    json_t *jItem         = NULL;
    vector<CTrainList> vecList;
    CTrainInfo objTrainInfo;
    int iRet = 0;
    bool bExist = true;

    /////////begin: parameter//////////////////////
	jItem = json_object_get(jReq, "userid");
    if (!jItem || !json_is_integer(jItem)) {
		strRetCode = CErrJsonType;
	    ERROR("GetLines json userid is not int. strRetCode: "<<strRetCode);
		return 1;
    }
	llUID = json_integer_value(jItem);

	jItem = json_object_get(jReq, "createtime");
    if (jItem && json_is_string(jItem)) {
        strCreateTime = json_string_value(jItem);
    }

    jItem = json_object_get(jReq, "num");
    if (!jItem || !json_is_integer(jItem)) 
	{
		strRetCode = CErrJsonType;
	    ERROR("GetLines json num is not int. strRetCode: "<<strRetCode);
		return 1;
    }
	iNum = json_integer_value(json_object_get(jReq, "num"));
	if ((iNum <= 0) || (0 != iNum%100))
	{
        strRetCode = CErrJsonType;
		ERROR("GetLines Get json num value error. strRetCode: "<<strRetCode<<", iNum: "<<iNum);
		return 1;
	}
    /////////end: parameter//////////////////////

    m_pConn = CDBPool::Instance()->GetConnect();
    if (NULL == m_pConn)
    {
        strRetCode = CErrDBConn;
        ERROR("CKTrain::GetDBConnect Call CDBPool::Instance()->GetConnect() error. strRetCode: "<<strRetCode);
        return 1;
    }

    do
    {
        vecList.clear();
        iRet = SearchDBCurr(llUID, bExist, objTrainInfo, strRetCode);  // 查找当前记录
        if (0 != iRet)
        {
            ERROR("Call SearchCurr error. llUID: "<<llUID<<", strRetCode: "<<strRetCode
                <<", iRet: "<<iRet);
            break;
        }
        
        if ((true == bExist) && (0 == objTrainInfo.m_iStatus))
        {
            CTrainList objList;
            objList.m_llUID = llUID;
            objList.m_strCreateTime = objTrainInfo.m_strCreateTime;
            objList.m_iBeginDate = objTrainInfo.m_iBeginDate;
            objList.m_iEndDate = objTrainInfo.m_iEndDate;
            objList.m_iBeginFund = objTrainInfo.m_iBeginFund;
            objList.m_iEndFund = objTrainInfo.m_iEndFund;
            objList.m_iTaxRate = objTrainInfo.m_iTaxRate;
            objList.m_iStatus = objTrainInfo.m_iStatus;
            vecList.push_back(objList);
        }

        iRet = SearchDBList(llUID, strCreateTime, iNum, vecList, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call SearchList error. llUID: "<<llUID<<", strRetCode: "<<strRetCode);
            break;
        }

        iRet = ShowTrainList(vecList, jRes, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call ShowTrainInfo error. strRetCode: "<<strRetCode);
            break;
        }

    }
    while (false);

    CDBPool::Instance()->PushConnect(m_pConn);
    
    return 0;
}

int CKTrain::Search(       // 查询训练信息
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{    
    long long llUID       = 0;       // 用户编号
    string strSession     = "";
    int iCurrent          = 0;       // 是否当前训练
    string strCreateTime  = "";      // 创建日期
    json_t *jItem         = NULL;
    CTrainInfo objTrainInfo;

    int iRet = 0;
    bool bExist = true;

    /////////begin: parameter//////////////////////
    jItem = json_object_get(jReq, "userid");
    /*if (!jItem || !json_is_integer(jItem)) {
        strRetCode = CErrJsonType;
        ERROR("GetLines json userid is not int. strRetCode: "<<strRetCode);
        return 1;
    }
    ullUserId = json_integer_value(jItem);
    if (0 == ullUserId)
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines json userid value error. userid: "<<ullUserId<<", strRetCode: "<<strRetCode);
        return 1;
    }*/
    if (jItem && json_is_integer(jItem)) {
        llUID = json_integer_value(jItem);
    }

    jItem = json_object_get(jReq, "session");
	if (jItem && json_is_string(jItem)) {
        strSession = json_string_value(jItem);
        strSession = trim(strSession);
	}


    jItem = json_object_get(jReq, "current");
    if (jItem && json_is_integer(jItem)) 
	{
		iCurrent= json_integer_value(json_object_get(jReq, "current"));
		if (iCurrent < 0)
		{
	        strRetCode = CErrJsonType;
			ERROR("GetLines Get json taxrate value error. strRetCode: "<<strRetCode<<", iCurrent: "<<iCurrent);
			return 1;
		}
    }

	jItem = json_object_get(jReq, "createtime");
    if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
	    ERROR("GetLines json createtime is not string. strRetCode: "<<strRetCode);
		return 1;
    }
	strCreateTime = json_string_value(jItem);
    /////////end: parameter//////////////////////

    m_pConn = CDBPool::Instance()->GetConnect();
    if (NULL == m_pConn)
    {
        strRetCode = CErrDBConn;
        ERROR("CKTrain::GetDBConnect Call CDBPool::Instance()->GetConnect() error. strRetCode: "<<strRetCode);
        return 1;
    }

    do
    {
        if (0 == llUID)
        {
        	iRet = GetUIDBySession(strSession, llUID, strRetCode);
    		if (0 != iRet)
    		{
    			ERROR("Call GetUIDBySession error. strSession: " << strSession << ", strRetCode: " << strRetCode);
    			break;
    		}
        }
            
        if (0 != iCurrent)  // 当前训练
        {
            iRet = SearchDBCurr(llUID, bExist, objTrainInfo, strRetCode);
            if (0 != iRet)
            {
                ERROR("Call SearchCurr error. llUID: "<<llUID<<", strRetCode: "<<strRetCode
                    <<", iRet: "<<iRet);
                break;
            }
            
            if (false == bExist) // 在数据库中不存在记录
            {
                iRet = 1;
                strRetCode = CErrDBNotExist;
                ERROR("Hasn't exist current train. llUID: "<<llUID<<", strRetCode: "<<strRetCode);
                break;
            }
        }
        else
        {
            iRet = SearchDBHistory(llUID, strCreateTime, bExist, objTrainInfo, strRetCode);
            if (0 != iRet)
            {
                ERROR("Call SearchCurr error. llUID: "<<llUID<<", strCreateTime: "<<strCreateTime
                    <<", strRetCode: "<<strRetCode);
                break;
            }
            
            if (false == bExist) // 在数据库中不存在记录
            {
                iRet = 1;
                strRetCode = CErrDBNotExist;
                ERROR("Hasn't exist current train. llUID: "<<llUID<<", strCreateTime: "<<strCreateTime
                    <<", strRetCode: "<<strRetCode);
                break;
            }
        }

        iRet = ShowTrainInfo(objTrainInfo, jRes, strRetCode);
        if (0 != iRet)
        {
            ERROR("Call ShowTrainInfo error. strRetCode: "<<strRetCode);
            break;
        }
    }
    while (false);

    CDBPool::Instance()->PushConnect(m_pConn);
    
    return iRet;
}

int CKTrain::SearchDBCurr(
    long long llUID,
    bool &bExist,
    CTrainInfo &objTrainInfo,
    string &strRetCode
    )
{
    int iRet = 0;
    string strSQL = "";
    PreparedStatement* stmt = NULL;
    ResultSet* resultSet = NULL;

    bExist = true;
    try
    {
        strSQL = "select userID, date_format(createTime, '%Y%m%d%H%i%s'), endDate, operate, beginFund, endFund, ";
        strSQL += "beginDate, hold, status, taxRate from tb_train_current where userID=? and status=0";
    	stmt = m_pConn->prepareStatement(strSQL.c_str());
		stmt->setInt64(1, llUID);
        resultSet = stmt->executeQuery();
        if (!resultSet->next())
        {
            bExist = false;
            delete resultSet;
		    delete stmt;
            return 0;
        }

        objTrainInfo.m_llUID = resultSet->getInt64(1);
        objTrainInfo.m_strCreateTime = resultSet->getString(2).c_str();
        objTrainInfo.m_iEndDate = atoi(resultSet->getString(3).c_str());
        objTrainInfo.m_strOperate = resultSet->getString(4).c_str();
        objTrainInfo.m_iBeginFund = resultSet->getInt(5);
        objTrainInfo.m_iEndFund = resultSet->getInt(6);
        objTrainInfo.m_iBeginDate = atoi(resultSet->getString(7).c_str());
        objTrainInfo.m_strHold = resultSet->getString(8).c_str();
        objTrainInfo.m_iStatus = resultSet->getInt(9);
        objTrainInfo.m_iTaxRate = resultSet->getInt(10);

        delete resultSet;
		delete stmt;
    }
    CATCH_DB;

    return iRet;
}

int CKTrain::BeginDBCurr(
    CTrainInfo &objTrainInfo, 
    bool bExist,
    string &strRetCode
    )
{
    int iRet = 0;
    char temp[128] = {0};
    string strSQL = "";
    PreparedStatement* stmt = NULL;
    int iCnt = 0;

    try
    {
        strSQL = "update tb_train_current set createtime = NOW(), enddate=?, operate='', beginfund=?, endfund=?, ";
        strSQL += "begindate=?, hold='', taxrate=?, status=0 where userid=?";
        stmt = m_pConn->prepareStatement(strSQL.c_str());
        sprintf(temp, "%d", objTrainInfo.m_iEndDate);
		stmt->setString(1, temp);
		stmt->setInt(2, objTrainInfo.m_iBeginFund);
		stmt->setInt(3, objTrainInfo.m_iEndFund);
        sprintf(temp, "%d", objTrainInfo.m_iBeginDate);
		stmt->setString(4, temp);
		stmt->setInt(5, objTrainInfo.m_iTaxRate);
		stmt->setInt64(6, objTrainInfo.m_llUID);
		iCnt = stmt->executeUpdate();
		delete stmt;
        stmt = NULL;
    }
    CATCH_DB;

    if (0 == iCnt)
    {
        try
        {
            strSQL = "insert into tb_train_current(userid, createtime, enddate, operate, beginfund, endfund, ";
            strSQL += "begindate, hold, taxrate, status) values(?, NOW(), ?, '', ?, ?, ?, '', ?, 0) ";
            stmt = m_pConn->prepareStatement(strSQL.c_str());
            stmt->setInt64(1, objTrainInfo.m_llUID);
            sprintf(temp, "%d", objTrainInfo.m_iEndDate);
            stmt->setString(2, temp);
            stmt->setInt(3, objTrainInfo.m_iBeginFund);
            stmt->setInt(4, objTrainInfo.m_iEndFund);
            sprintf(temp, "%d", objTrainInfo.m_iBeginDate);
            stmt->setString(5, temp);
            stmt->setInt(6, objTrainInfo.m_iTaxRate);
            
            stmt->executeUpdate();
            delete stmt;
            stmt = NULL;
        }
        CATCH_DB;
    }
    
    return iRet;
}

int CKTrain::TradeBuy(
    string strCode, 
    int iNum, 
    int iPrice, 
    int iDate, 
    CTrainInfo &objTrainInfo, 
    string &strRetCode
    )
{
    int iRet         = 0;
    CTrainTrade objOperate;
    CTrainHold objHold;
    vector<CTrainTrade> vecOperate;
    vector<CTrainHold> vecHold;

    iRet = objTrainInfo.ParseJson(vecOperate, strRetCode);  // 解析operate字段
    if (0 != iRet)
    {
        ERROR("Call objTrainInfo.ParseJson error. strRetCode: "<<strRetCode);
        return iRet;
    }

    iRet = objTrainInfo.ParseJson(vecHold, strRetCode);     // 解析hold字段
    if (0 != iRet)
    {
        ERROR("Call objTrainInfo.ParseJson error. strRetCode: "<<strRetCode);
        return iRet;
    }

    ////////////////////begin: buy////////////////////////////////
    // operate
    objOperate.m_strType = "buy";
    objOperate.m_strCode = strCode;
    objOperate.m_iNum    = iNum;
    objOperate.m_iPrice  = iPrice;
    objOperate.m_iTax    = iNum*iPrice*objTrainInfo.m_iTaxRate/10000;
    objOperate.m_iDate   = iDate;
    vecOperate.push_back(objOperate);

    // fund
    if (objTrainInfo.m_iEndFund < iNum*iPrice + objOperate.m_iTax)
    {
        strRetCode = CErrOutOfRange;
        ERROR("objTrainInfo.m_iEndFund["<<objTrainInfo.m_iEndFund<<"] < iNum*iPrice + objOperate.m_iTax["
            <<iNum*iPrice + objOperate.m_iTax<<"]. strRetCode: "<<strRetCode);
        return 1;
    }
    objTrainInfo.m_iEndFund = objTrainInfo.m_iEndFund - iNum*iPrice - objOperate.m_iTax;

    // hold
    objHold.m_strCode  = objOperate.m_strCode;
    objHold.m_iNum     = objOperate.m_iNum;
    objHold.m_iPrice   = objOperate.m_iPrice;
    int i=0;
    for (i=0; i<vecHold.size(); i++)
    {
        if (vecHold[i].m_strCode == objHold.m_strCode)
        {
            break;
        }
    }
    
    if (i >= vecHold.size())
    {
        vecHold.push_back(objHold);
    }
    else
    {
        vecHold[i].m_iNum    += objHold.m_iNum;
        vecHold[i].m_iPrice  = (vecHold[i].m_iPrice*vecHold[i].m_iNum + objHold.m_iPrice*objHold.m_iNum) 
                               / (vecHold[i].m_iNum + objHold.m_iNum);
    }
    ////////////////////end: buy////////////////////////////////

    iRet = objTrainInfo.CreateJson(vecOperate, strRetCode); // 生成operate的json字符串
    if (0 != iRet)
    {
        ERROR("Call objTrainInfo.CreateJson error. strRetCode: "<<strRetCode);
        return iRet;
    }

    iRet = objTrainInfo.CreateJson(vecHold, strRetCode);    // 生成hold的json字符串
    if (0 != iRet)
    {
        ERROR("Call objTrainInfo.CreateJson error. strRetCode: "<<strRetCode);
        return iRet;
    }
    objTrainInfo.m_iEndDate = iDate;
    
    return iRet;
}

int CKTrain::TradeSale(
    string strCode, 
    int iNum, 
    int iPrice, 
    int iDate, 
    CTrainInfo &objTrainInfo, 
    string &strRetCode
    )
{
    int iRet         = 0;
    CTrainTrade objOperate;
    vector<CTrainTrade> vecOperate;
    vector<CTrainHold> vecHold;
    
    iRet = objTrainInfo.ParseJson(vecOperate, strRetCode);  // 解析operate字段
    if (0 != iRet)
    {
        ERROR("Call objTrainInfo.ParseJson error. strRetCode: "<<strRetCode);
        return iRet;
    }
    
    iRet = objTrainInfo.ParseJson(vecHold, strRetCode);     // 解析hold字段
    if (0 != iRet)
    {
        ERROR("Call objTrainInfo.ParseJson error. strRetCode: "<<strRetCode);
        return iRet;
    }
    
    ////////////////////begin: sale////////////////////////////////
    // operate
    objOperate.m_strType = "sale";
    objOperate.m_strCode = strCode;
    objOperate.m_iNum    = iNum;
    objOperate.m_iPrice  = iPrice;
    objOperate.m_iTax    = 0; //iNum*iPrice*objTrainInfo.m_iTaxRate/10000;
    objOperate.m_iDate   = iDate;
    vecOperate.push_back(objOperate);

    // hold
    int i=0;
    for (i=0; i<vecHold.size(); i++)
    {
        if (vecHold[i].m_strCode == objOperate.m_strCode)
        {
            break;
        }
    }
    
    if (i >= vecHold.size())
    {
        strRetCode = CErrOutOfRange;
        ERROR("No strCode: "<<objOperate.m_strCode<<" in vecHold. strRetCode: "<<strRetCode);
        return 1;
    }

    if (vecHold[i].m_iNum < objOperate.m_iNum)
    {
        strRetCode = CErrOutOfRange;
        ERROR("vecHold[i].m_iNum["<<vecHold[i].m_iNum<<"] < objOperate.m_iNum["<<objOperate.m_iNum
            <<"]. strRetCode: "<<strRetCode);
        return 1;
    }
    
    if (objOperate.m_iNum == vecHold[i].m_iNum)
    {
        vector<CTrainHold>::iterator iter = vecHold.begin() + i;
        vecHold.erase(iter);
    }
    else
    {
        vecHold[i].m_iPrice  = (vecHold[i].m_iPrice*vecHold[i].m_iNum - objOperate.m_iPrice*objOperate.m_iNum) 
                           / (vecHold[i].m_iNum - objOperate.m_iNum);
        vecHold[i].m_iNum -= objOperate.m_iNum;
    }
    
    // fund
    objTrainInfo.m_iEndFund = objTrainInfo.m_iEndFund + objOperate.m_iPrice*objOperate.m_iNum;
    ////////////////////end: sale////////////////////////////////
    
    iRet = objTrainInfo.CreateJson(vecOperate, strRetCode); // 生成operate的json字符串
    if (0 != iRet)
    {
        ERROR("Call objTrainInfo.CreateJson error. strRetCode: "<<strRetCode);
        return iRet;
    }
    
    iRet = objTrainInfo.CreateJson(vecHold, strRetCode);    // 生成hold的json字符串
    if (0 != iRet)
    {
        ERROR("Call objTrainInfo.CreateJson error. strRetCode: "<<strRetCode);
        return iRet;
    }
    objTrainInfo.m_iEndDate = iDate;
    
    return iRet;
}

int CKTrain::UpdateDBCurr(
    CTrainInfo &objTrainInfo, 
    string &strRetCode
    )
{
    int iRet = 0;
    char temp[128] = {0};
    string strSQL = "";
    PreparedStatement* stmt = NULL;

    try
    {
        strSQL = "update tb_train_current set enddate=?, operate=?, endfund=?, hold=? where userid=?";
        stmt = m_pConn->prepareStatement(strSQL.c_str());
        sprintf(temp, "%d", objTrainInfo.m_iEndDate);
		stmt->setString(1, temp);
        stmt->setString(2, objTrainInfo.m_strOperate.c_str());
		stmt->setInt(3, objTrainInfo.m_iEndFund);
		stmt->setString(4, objTrainInfo.m_strHold.c_str());
		stmt->setInt64(5, objTrainInfo.m_llUID);
		stmt->executeUpdate();
		delete stmt;
        stmt = NULL;
    }
    CATCH_DB;
    
    return iRet;

}

int CKTrain::SearchDBHistory(
    long long llUID, 
    string strCreateTime, 
    bool &bExist, 
    CTrainInfo &objTrainInfo, 
    string &strRetCode
    )
{
    int iRet = 0;
    string strSQL = "";
    PreparedStatement* stmt = NULL;
    ResultSet* resultSet = NULL;

    bExist = true;
    try
    {
        strSQL = "select userID, date_format(createTime, '%Y%m%d%H%i%s'), endDate, operate, beginFund, endFund, ";
        strSQL += "beginDate, hold, taxRate from tb_train_history where userID=? and ";
        strSQL += "date_format(createTime, '%Y%m%d%H%i%s')=?";
    	stmt = m_pConn->prepareStatement(strSQL.c_str());
		stmt->setInt64(1, llUID);
        stmt->setString(2, strCreateTime.c_str());
        resultSet = stmt->executeQuery();
        if (!resultSet->next())
        {
            bExist = false;
            delete resultSet;
		    delete stmt;
            return 0;
        }

        bExist = true;
        objTrainInfo.m_llUID = resultSet->getInt64(1);
        objTrainInfo.m_strCreateTime = resultSet->getString(2).c_str();
        objTrainInfo.m_iEndDate = atoi(resultSet->getString(3).c_str());
        objTrainInfo.m_strOperate = resultSet->getString(4).c_str();
        objTrainInfo.m_iBeginFund = resultSet->getInt(5);
        objTrainInfo.m_iEndFund = resultSet->getInt(6);
        objTrainInfo.m_iBeginDate = atoi(resultSet->getString(7).c_str());
        objTrainInfo.m_strHold = resultSet->getString(8).c_str();
        objTrainInfo.m_iTaxRate = resultSet->getInt(9);
        objTrainInfo.m_iStatus = 1;

        delete resultSet;
		delete stmt;
    }
    CATCH_DB;

    return iRet;
}

int CKTrain::FinishDBCurr(
    CTrainInfo &objTrainInfo, 
    string &strRetCode
    )
{
    int iRet = 0;
    char temp[128] = {0};
    string strSQL = "";
    PreparedStatement* stmt = NULL;

    try
    {
        strSQL = "update tb_train_current set enddate=?, status=1 where userid=?";
        stmt = m_pConn->prepareStatement(strSQL.c_str());
        sprintf(temp, "%d", objTrainInfo.m_iEndDate);
		stmt->setString(1, temp);
		stmt->setInt64(2, objTrainInfo.m_llUID);
		stmt->executeUpdate();
		delete stmt;
        stmt = NULL;

        strSQL = "insert into tb_train_history(userid, createtime, enddate, operate, beginfund, endfund, ";
        strSQL += "begindate, hold, taxrate) values(?, str_to_date(?, '%Y%m%d%H%i%s'), ?, ?, ?, ?, ?, ?, ?)";
        stmt = m_pConn->prepareStatement(strSQL.c_str());
        stmt->setInt64(1, objTrainInfo.m_llUID);
        stmt->setString(2, objTrainInfo.m_strCreateTime.c_str());
        sprintf(temp, "%d", objTrainInfo.m_iEndDate);
        stmt->setString(3, temp);
        stmt->setString(4, objTrainInfo.m_strOperate.c_str());
        stmt->setInt(5, objTrainInfo.m_iBeginFund);
        stmt->setInt(6, objTrainInfo.m_iEndFund);
        sprintf(temp, "%d", objTrainInfo.m_iBeginDate);
        stmt->setString(7, temp);
        stmt->setString(8, objTrainInfo.m_strHold.c_str());
        stmt->setInt(9, objTrainInfo.m_iTaxRate);
        stmt->executeUpdate();
        delete stmt;
        stmt = NULL;        
    }
    CATCH_DB;
    
    return iRet;
}

int CKTrain::SearchDBList(
    long long llUID, 
    string strCreateTime,
    int iNum,
    vector<CTrainList> &vecList, 
    string &strRetCode
    )
{
    int iRet = 0;
    string strSQL = "";
    PreparedStatement* stmt = NULL;
    ResultSet* resultSet = NULL;
    CTrainList objList;

    if ((0 >= iNum) || (100 < iNum))
    {
        iNum = 100;
    }

    try
    {
        if (!strCreateTime.empty())
        {
            strSQL = "select userID, date_format(createTime, '%Y%m%d%H%i%s'), endDate, beginFund, endFund, ";
            strSQL += "beginDate, taxRate from tb_train_history where userID=? and ";
            strSQL += "date_format(createTime, '%Y%m%d%H%i%s') < ? order by createtime desc limit ?";
            stmt = m_pConn->prepareStatement(strSQL.c_str());
            stmt->setInt64(1, llUID);
            stmt->setString(2, strCreateTime.c_str());
            stmt->setInt(3, iNum);
        }
        else
        {
            strSQL = "select userID, date_format(createTime, '%Y%m%d%H%i%s'), endDate, beginFund, endFund, ";
            strSQL += "beginDate, taxRate from tb_train_history where userID=? ";
            strSQL += "order by createtime desc limit ?";
            stmt = m_pConn->prepareStatement(strSQL.c_str());
            stmt->setInt64(1, llUID);
            stmt->setInt(2, iNum);
        }
        resultSet = stmt->executeQuery();
        while (resultSet->next())
        {
            objList.m_llUID = resultSet->getInt64(1);
            objList.m_strCreateTime = resultSet->getString(2).c_str();
            objList.m_iEndDate = atoi(resultSet->getString(3).c_str());
            objList.m_iBeginFund = resultSet->getInt(4);
            objList.m_iEndFund = resultSet->getInt(5);
            objList.m_iBeginDate = atoi(resultSet->getString(6).c_str());
            objList.m_iTaxRate = resultSet->getInt(7);
            objList.m_iStatus = 1;
            vecList.push_back(objList);
        }
    
        delete resultSet;
        delete stmt;
    }
    CATCH_DB;

    return iRet;
}

int CKTrain::ShowTrainInfo(
    CTrainInfo &objTrainInfo, 
    json_t *&jRes, 
    string &strRetCode
    )
{
    json_t *jOperate = NULL;
    json_t *jResult = json_object();
    json_object_set_new(jResult, "begindate",   json_integer(objTrainInfo.m_iBeginDate));
    json_object_set_new(jResult, "enddate",        json_integer(objTrainInfo.m_iEndDate));
    json_object_set_new(jResult, "beginfund",   json_integer(objTrainInfo.m_iBeginFund));
    json_object_set_new(jResult, "endfund",        json_integer(objTrainInfo.m_iEndFund));
    json_object_set_new(jResult, "taxrate",        json_integer(objTrainInfo.m_iTaxRate));

    if (!objTrainInfo.m_strOperate.empty())  // operate
    {
        jOperate = json_loadb(objTrainInfo.m_strOperate.c_str(), objTrainInfo.m_strOperate.length(), 0, NULL);
        if (!jOperate || !json_is_array(jOperate)) {
            strRetCode = CErrJsonType;
            ERROR("GetListAll Get json item error. strRetCode: "<<strRetCode);
            //json_decref(jRoot);
            json_decref(jOperate);
            json_decref(jResult);
            return 1;
        }
    }
    else
    {
        jOperate = json_array();
    }
    json_object_set_new(jResult, "operate", jOperate);

    if (!objTrainInfo.m_strHold.empty())  // hold
    {
        jOperate = json_loadb(objTrainInfo.m_strHold.c_str(), objTrainInfo.m_strHold.length(), 0, NULL);
        if (!jOperate || !json_is_array(jOperate)) {
            strRetCode = CErrJsonType;
            ERROR("GetListAll Get json item error. strRetCode: "<<strRetCode);
            //json_decref(jRoot);
            json_decref(jOperate);
            json_decref(jResult);
            return 1;
        }
    }
    else
    {
        jOperate = json_array();
    }
    json_object_set_new(jResult, "hold", jOperate);

    json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
    json_object_set_new(jRes, "result", jResult);
    
    return 0;
}

int CKTrain::ShowTrainList(
    vector<CTrainList> vecList, 
    json_t *&jRes, 
    string &strRetCode
    )
{
    json_t *jResult = json_array();
    for (int i=0; i<vecList.size(); i++)
    {
        json_t *jItem = json_object();
        json_object_set_new(jItem, "createtime",  json_string(vecList[i].m_strCreateTime.c_str()));	
    	json_object_set_new(jItem, "begindate",   json_integer(vecList[i].m_iBeginDate));
    	json_object_set_new(jItem, "enddate",        json_integer(vecList[i].m_iEndDate));
    	json_object_set_new(jItem, "beginfund",   json_integer(vecList[i].m_iBeginFund));
    	json_object_set_new(jItem, "endfund",        json_integer(vecList[i].m_iEndFund));
    	json_object_set_new(jItem, "taxrate",     json_integer(vecList[i].m_iTaxRate));
    	json_object_set_new(jItem, "status",      json_integer(vecList[i].m_iStatus));
        json_array_append_new(jResult, jItem);
    }
    json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
    json_object_set_new(jRes, "result", jResult);
    
    return 0;
}

int CKTrain::GetUIDBySession(
    string strSession,
    long long &llUID,
    string &strRetCode
    )
{
    string strSQL = "";
    PreparedStatement* stmt = NULL;
    ResultSet* resultSet = NULL;
    unsigned long long ullOldTime = 0;
    unsigned long long ullNowTime = 0;

    try
    {
        strSQL = "select userid, unix_timestamp(time), UNIX_TIMESTAMP() from tb_user_session where sessionid=?"; // 查询session
        stmt = m_pConn->prepareStatement(strSQL.c_str());
        stmt->setString(1, strSession.c_str());
        resultSet = stmt->executeQuery();
        if (!resultSet->next())
        {
            delete resultSet;
            delete stmt;
            strRetCode = CErrDBNotExist;
            ERROR("GetUIDBySession not exist. strSession: " << strSession << ", strRetCode: " << strRetCode);
            return 1;
        }
        llUID = resultSet->getInt64(1);
        ullOldTime = resultSet->getInt64(2);
        ullNowTime = resultSet->getInt64(3);
        delete resultSet;
        delete stmt;

        if (ullNowTime - ullOldTime > 1800)  // 判断是否超时
        {
            strSQL = "delete from tb_user_session where sessionid=?";
            stmt = m_pConn->prepareStatement(strSQL.c_str());
            stmt->setString(1, strSession.c_str());
            stmt->executeUpdate();
            delete stmt;
            strRetCode = CErrDBNotExist;
            ERROR("GetUIDBySession out of time. strSession: " << strSession <<", ullOldTime: "<<ullOldTime<<", ullNowTime: "
                <<ullNowTime << ", strRetCode: " << strRetCode);
            return 1;
        }

        strSQL = "update tb_user_session set time=NOW() where sessionid=?";  // 更新时间
        stmt = m_pConn->prepareStatement(strSQL.c_str());
        stmt->setString(1, strSession.c_str());
        stmt->executeUpdate();
        delete stmt;
    }
    CATCH_DB;

    return 0;
}

