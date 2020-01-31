#include "TradePoint.h"
#include "Constant.h"
#include "Log.h"
#include "Calendar.h"
#include <algorithm>
#include "StrSplit.h"

CPointData::CPointData()
{
    m_ullUserId = 0;
    m_strCode = "";
    m_nBeginDate = 0;
    m_nEndData = 0;
    m_vecPoint.clear();
    m_vecCount.clear();
    m_pPoint = NULL;
    m_nSize = 0;
}

CPointData::~CPointData()
{
    /*if (NULL != m_pPoint)
    {
        free(m_pPoint);
        //delete m_pPoint;
        m_pPoint = NULL;
    }
    */
}

int CPointData::Point2Vec(int nDate, set<int> setTrade)
{
    SPointBi objBiPoint;
    int count = 1;
    m_vecPoint.clear();
    m_vecCount.clear();
    //DEBUG("Point2Vec m_nSize: "<<m_nSize);
    for (int i=0; i<m_nSize; i++)
    {
        memcpy(&objBiPoint, m_pPoint+i, sizeof(SPointBi));
        //DEBUG("m_pPoint+i: "<<(m_pPoint+i)<<", sizeof(SPointBi): "<<sizeof(SPointBi));
        if ((0 == nDate) || (objBiPoint.m_nDate <= nDate))
        {
            if ((0 == setTrade.size()) || (setTrade.count(objBiPoint.m_nTrade)))
            {
                DEBUG("Point2Vec trade: "<<objBiPoint.m_nTrade<<", date: "<<objBiPoint.m_nDate<<", code: "<<m_strCode);
                m_vecPoint.push_back(objBiPoint);
                m_vecCount.push_back(count);
            }
        }
    }
    
    return 0;
}

int CPointData::Vec2Point()
{
    if (NULL != m_pPoint)
    {
        m_nSize = 0;
        delete m_pPoint;
        m_pPoint = NULL;
    }
    m_nSize = m_vecPoint.size();
    m_pPoint = (SPointBi*)malloc(sizeof(SPointBi)*m_nSize);
    for (int i=0; i< m_nSize; i++)
    {
        memcpy((void*)(m_pPoint + i), &(m_vecPoint[i]), sizeof(SPointBi));
    }
    
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

int CTradePoint::Insert(                 // 插入一个买卖点
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int nRet       = 0;
    json_t *jItem  = NULL;
    unsigned long long ullUserId = 0;
    string strSession = "";
    string strCode = "";
    bool bExist = true;
    SPointBi objBiPoint;
    CPointData objPoint;
    vector<CPointData> vecPoint;
    set<int> setTrade;

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
        ullUserId = json_integer_value(jItem);
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
    if (6 != strCode.length())
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines json code value error. code: "<<strCode<<", strRetCode: "<<strRetCode);
        return 1;
    }
    
    jItem = json_object_get(jReq, "trade");
    if (!jItem || !json_is_integer(jItem)) 
	{
		strRetCode = CErrJsonType;
	    ERROR("GetLines json trade is not int. strRetCode: "<<strRetCode);
		return 1;
    }
    objBiPoint.m_nTrade = json_integer_value(json_object_get(jReq, "trade"));
    if (0 >= objBiPoint.m_nTrade)
    {
		strRetCode = CErrJsonType;
	    ERROR("GetLines json trade value error. m_nTrade: "<<objBiPoint.m_nTrade<<", strRetCode: "<<strRetCode);
		return 1;
    }

    jItem = json_object_get(jReq, "date");
    if (!jItem || !json_is_integer(jItem)) 
	{
		strRetCode = CErrJsonType;
	    ERROR("GetLines json date is not int. strRetCode: "<<strRetCode);
		return 1;
    }
    objBiPoint.m_nDate = json_integer_value(json_object_get(jReq, "date"));
    if (0 >= objBiPoint.m_nDate)
    {
		strRetCode = CErrJsonType;
	    ERROR("GetLines json number value error. m_nDate: "<<objBiPoint.m_nDate<<", strRetCode: "<<strRetCode);
		return 1;
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
        if (0 == ullUserId)
        {
        	nRet = GetUIDBySession(strSession, ullUserId, strRetCode);
    		if (0 != nRet)
    		{
    			ERROR("Call GetUIDBySession error. strSession: " << strSession << ", strRetCode: " << strRetCode);
    			break;
    		}
        }
        
        nRet = SearchDBPoint(ullUserId, strCode, objPoint, strRetCode);
        if (0 != nRet)
        {
            ERROR("Call SearchDBPoint error. llUID: "<<objPoint.m_ullUserId<<", code: "<<objPoint.m_strCode
                <<", strRetCode: "<<strRetCode<<", nRet: "<<nRet);
            break;
        }

        if (0 == objPoint.m_ullUserId)  // 数据库无记录
        {
            bExist = false;
            objPoint.m_ullUserId = ullUserId;
            objPoint.m_strCode = strCode;
            objPoint.m_nBeginDate = objBiPoint.m_nDate;
            objPoint.m_nEndData = objBiPoint.m_nDate;
            objPoint.m_pPoint = (SPointBi*)malloc(sizeof(objBiPoint));
            objPoint.m_nSize = 1;
            memcpy(objPoint.m_pPoint, &objBiPoint, sizeof(objBiPoint));
        }
        else
        {
            bExist = true;
            objPoint.m_pPoint = (SPointBi*)realloc(objPoint.m_pPoint, (objPoint.m_nSize+1)*(sizeof(objBiPoint)));
            objPoint.m_nSize++;
            memcpy((void*)(objPoint.m_pPoint + objPoint.m_nSize-1), &objBiPoint, sizeof(objBiPoint));
            if (objPoint.m_nEndData < objBiPoint.m_nDate)
            {
                objPoint.m_nEndData = objBiPoint.m_nDate;
            }
        }

        objPoint.Point2Vec(objPoint.m_nEndData, setTrade);
        nRet = UpdateDBPoint(objPoint, bExist, strRetCode);
        if (0 != nRet)
        {
            ERROR("Call UpdateDBPoint error. llUID: "<<objPoint.m_ullUserId<<", code: "<<objPoint.m_strCode
                <<", strRetCode: "<<strRetCode<<", nRet: "<<nRet);
            break;
        }

        vecPoint.clear();
        vecPoint.push_back(objPoint);
        nRet = ShowInfo(vecPoint, jRes, strRetCode);
        if (0 != nRet)
        {
            ERROR("Call ShowInfo error. strRetCode: "<<strRetCode);
            break;
        }
    }
    while (false);

    for (int i=0; i<vecPoint.size(); i++)
    {
        free(vecPoint[i].m_pPoint);
        vecPoint[i].m_pPoint = NULL;
    }

    CDBPool::Instance()->PushConnect(m_pConn);

    return nRet;
}

int CTradePoint::Delete(                 // 删除一个买卖点
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int nRet       = 0;
    json_t *jItem  = NULL;
    unsigned long long ullUserId = 0;
    string strSession = "";
    string strCode = "";
    bool bExist = true;
    SPointBi objBiPoint;
    CPointData objPoint;
    vector<CPointData> vecPoint;
    set<int> setTrade;
    
    /////////begin: 解析json，得到各字段item//////////////////////
    /*jItem = json_object_get(jReq, "userid");
    if (!jItem || !json_is_integer(jItem)) {
        strRetCode = CErrJsonType;
        ERROR("GetLines json userid is not int. strRetCode: "<<strRetCode);
        return 1;
    }
    ullUserId = json_integer_value(jItem);
    if (0 >= ullUserId)
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines json userid value error. userid: "<<ullUserId<<", strRetCode: "<<strRetCode);
        return 1;
    }*/
    
    jItem = json_object_get(jReq, "userid");
    if (jItem && json_is_integer(jItem)) {
        ullUserId = json_integer_value(jItem);
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
    if (6 != strCode.length())
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines json code value error. code: "<<strCode<<", strRetCode: "<<strRetCode);
        return 1;
    }
    
    jItem = json_object_get(jReq, "trade");
    if (!jItem || !json_is_integer(jItem)) 
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines json trade is not int. strRetCode: "<<strRetCode);
        return 1;
    }
    objBiPoint.m_nTrade = json_integer_value(json_object_get(jReq, "trade"));
    if (0 >= objBiPoint.m_nTrade)
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines json trade value error. m_nTrade: "<<objBiPoint.m_nTrade<<", strRetCode: "<<strRetCode);
        return 1;
    }
    
    jItem = json_object_get(jReq, "date");
    if (!jItem || !json_is_integer(jItem)) 
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines json date is not int. strRetCode: "<<strRetCode);
        return 1;
    }
    objBiPoint.m_nDate = json_integer_value(json_object_get(jReq, "date"));
    if (0 >= objBiPoint.m_nDate)
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines json number value error. m_nDate: "<<objBiPoint.m_nDate<<", strRetCode: "<<strRetCode);
        return 1;
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
        if (0 == ullUserId)
        {
        	nRet = GetUIDBySession(strSession, ullUserId, strRetCode);
    		if (0 != nRet)
    		{
    			ERROR("Call GetUIDBySession error. strSession: " << strSession << ", strRetCode: " << strRetCode);
    			break;
    		}
        }
        
        nRet = SearchDBPoint(ullUserId, strCode, objPoint, strRetCode);
        if (0 != nRet)
        {
            ERROR("Call SearchDBPoint error. llUID: "<<objPoint.m_ullUserId<<", code: "<<objPoint.m_strCode
                <<", strRetCode: "<<strRetCode<<", nRet: "<<nRet);
            break;
        }

        vecPoint.clear();
        if (0 != objPoint.m_ullUserId)  // 数据库有记录
        {   
            bExist = true;

            setTrade.clear();
            objPoint.Point2Vec(0, setTrade);
            objPoint.m_nBeginDate = objPoint.m_nEndData = 0;
            DEBUG("objPoint.m_vecPoint.size(): "<<objPoint.m_vecPoint.size());
            for (int i=0; i<objPoint.m_vecPoint.size(); i++)
            {
                if ((objPoint.m_vecPoint[i].m_nDate == objBiPoint.m_nDate) 
                    && (objPoint.m_vecPoint[i].m_nTrade == objBiPoint.m_nTrade))
                {
                    objPoint.m_vecPoint.erase(objPoint.m_vecPoint.begin() + i);
                    DEBUG("Delete point date: "<<objPoint.m_vecPoint[i].m_nDate
                        <<", trade: "<<objPoint.m_vecPoint[i].m_nTrade);
                    i--;
                    continue;
                }
                if (0 == objPoint.m_nBeginDate)
                {
                    objPoint.m_nBeginDate = objPoint.m_nEndData = objPoint.m_vecPoint[i].m_nDate;
                }
                else
                {
                    objPoint.m_nBeginDate = (objPoint.m_nBeginDate < objPoint.m_vecPoint[i].m_nDate) ?
                        objPoint.m_nBeginDate : objPoint.m_vecPoint[i].m_nDate;
                    objPoint.m_nEndData = (objPoint.m_nEndData > objPoint.m_vecPoint[i].m_nDate) ?
                        objPoint.m_nEndData : objPoint.m_vecPoint[i].m_nDate;
                }
            }
            DEBUG("objPoint.m_vecPoint.size(): "<<objPoint.m_vecPoint.size());
            
            objPoint.Vec2Point();
            nRet = UpdateDBPoint(objPoint, bExist, strRetCode);
            if (0 != nRet)
            {
                ERROR("Call UpdateDBPoint error. llUID: "<<objPoint.m_ullUserId<<", code: "<<objPoint.m_strCode
                    <<", strRetCode: "<<strRetCode<<", nRet: "<<nRet);
                break;
            }
            vecPoint.push_back(objPoint);
        }

        nRet = ShowInfo(vecPoint, jRes, strRetCode);
        if (0 != nRet)
        {
            ERROR("Call ShowInfo error. strRetCode: "<<strRetCode);
            break;
        }
    }
    while (false);
    
    for (int i=0; i<vecPoint.size(); i++)
    {
        if (NULL != vecPoint[i].m_pPoint)
        {
            free(vecPoint[i].m_pPoint);
            vecPoint[i].m_pPoint = NULL;
        }
    }
    
    CDBPool::Instance()->PushConnect(m_pConn);
    
    return nRet;
}

int CTradePoint::Search(                 // 查找某种类型的买卖点
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int nRet       = 0;
    json_t *jItem  = NULL;
    unsigned long long ullUserId = 0;
    string strSession = "";
    vector<string> vecCode;
    size_t sJsonSize   = 0;
    vector<CPointData> vecPoint;
    set<int> setTrade;
    int nDate = 0;
    SPointBi objBiPoint;
    CPointData objPoint;
    
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
        ullUserId = json_integer_value(jItem);
    }

    jItem = json_object_get(jReq, "session");
	if (jItem && json_is_string(jItem)) {
        strSession = json_string_value(jItem);
        strSession = trim(strSession);
	}

    jItem = json_object_get(jReq, "code");
    if (!jItem || !json_is_array(jItem)) {
        strRetCode = CErrJsonType;
		ERROR("GetListAll Get json code error. strRetCode: "<<strRetCode);
		return 1;
    }
	sJsonSize = json_array_size(jItem);
	if (0 == sJsonSize)
	{
        strRetCode = CErrJsonType;
		ERROR("GetListAll Get json code error. strRetCode: "<<strRetCode);
		return 1;
	}
	
	for (int i=0; i<sJsonSize; i++)
	{
	    vecCode.push_back(json_string_value(json_array_get(jItem, i)));
	}
    
    jItem = json_object_get(jReq, "date");
    if (!jItem || !json_is_integer(jItem)) 
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines json date is not int. strRetCode: "<<strRetCode);
        return 1;
    }
    nDate = json_integer_value(json_object_get(jReq, "date"));
    if (0 > nDate)
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines json number value error. nDate: "<<nDate<<", strRetCode: "<<strRetCode);
        return 1;
    }
    
    jItem = json_object_get(jReq, "trade");
    if (jItem && json_is_array(jItem)) 
    {
        sJsonSize = json_array_size(jItem);
        for (int i=0; i<sJsonSize; i++)
        {
            setTrade.insert(json_integer_value(json_array_get(jItem, i)));
            //vecCode.push_back(json_string_value(json_array_get(jItem, i)));
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
        if (0 == ullUserId)
        {
        	nRet = GetUIDBySession(strSession, ullUserId, strRetCode);
    		if (0 != nRet)
    		{
    			ERROR("Call GetUIDBySession error. strSession: " << strSession << ", strRetCode: " << strRetCode);
    			break;
    		}
        }
        
        nRet = SearchDBPoint(ullUserId, vecCode, vecPoint, strRetCode);
        if (0 != nRet)
        {
            ERROR("Call SearchDBPoint error. llUID: "<<ullUserId<<", code size: "<<vecCode.size()
                <<", strRetCode: "<<strRetCode<<", nRet: "<<nRet);
            break;
        }

        for (int i=0; i<vecPoint.size(); i++)
        {
            nRet = vecPoint[i].Point2Vec(nDate, setTrade);
            if (0 != nRet)
            {
                strRetCode = CErrOutOfRange;
                ERROR("Call vecPoint[i].Point2Vec error. nDate: "<<nDate<<", llUID: "<<ullUserId
                    <<", code: "<<vecPoint[i].m_strCode<<", strRetCode: "<<strRetCode);
                break;
            }
        }

        if (0 != nRet)
        {
            break;
        }

        nRet = ShowInfo(vecPoint, jRes, strRetCode);
        if (0 != nRet)
        {
            ERROR("Call ShowInfo error. strRetCode: "<<strRetCode);
            break;
        }
    }
    while (false);

    for (int i=0; i<vecPoint.size(); i++)
    {
        free(vecPoint[i].m_pPoint);
        vecPoint[i].m_pPoint = NULL;
    }
    
    CDBPool::Instance()->PushConnect(m_pConn);
    
    return nRet;
}

int CTradePoint::Statistics(             // 统计某种类型买卖点
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int nRet       = 0;
    json_t *jItem  = NULL;
    unsigned long long ullUserId = 0;
    string strSession = "";
    vector<string> vecCode;
    size_t sJsonSize   = 0;
    vector<CPointData> vecPoint;
    set<int> setTrade;
    vector<int> vecDate;
    map<unsigned long long, CPointStat> mapStat;
    int nDate = 0;
    string strCycle = "";
    SPointBi objBiPoint;
    CPointData objPoint;
    
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
        ullUserId = json_integer_value(jItem);
    }

    jItem = json_object_get(jReq, "session");
	if (jItem && json_is_string(jItem)) {
        strSession = json_string_value(jItem);
        strSession = trim(strSession);
	}
    
    jItem = json_object_get(jReq, "code");
    if (!jItem || !json_is_array(jItem)) {
        strRetCode = CErrJsonType;
        ERROR("GetListAll Get json code error. strRetCode: "<<strRetCode);
        return 1;
    }
    sJsonSize = json_array_size(jItem);
    if (0 == sJsonSize)
    {
        strRetCode = CErrJsonType;
        ERROR("GetListAll Get json code error. strRetCode: "<<strRetCode);
        return 1;
    }
    
    for (int i=0; i<sJsonSize; i++)
    {
        vecCode.push_back(json_string_value(json_array_get(jItem, i)));
    }

    jItem = json_object_get(jReq, "cycle");
    if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
	    ERROR("GetLines json cycle is not string. strRetCode: "<<strRetCode);
		return 1;
    }
	strCycle = json_string_value(jItem);
    
    jItem = json_object_get(jReq, "date");
    if (jItem && json_is_integer(jItem)) 
    {
        nDate = json_integer_value(json_object_get(jReq, "date"));
    }
    if (0 > nDate)
    {
        strRetCode = CErrJsonType;
        ERROR("GetLines json number value error. nDate: "<<nDate<<", strRetCode: "<<strRetCode);
        return 1;
    }

    jItem = json_object_get(jReq, "trade");
    if (jItem && json_is_array(jItem)) 
    {
        sJsonSize = json_array_size(jItem);
        for (int i=0; i<sJsonSize; i++)
        {
            setTrade.insert(json_integer_value(json_array_get(jItem, i)));
            //vecCode.push_back(json_string_value(json_array_get(jItem, i)));
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
        if (0 == ullUserId)
        {
        	nRet = GetUIDBySession(strSession, ullUserId, strRetCode);
    		if (0 != nRet)
    		{
    			ERROR("Call GetUIDBySession error. strSession: " << strSession << ", strRetCode: " << strRetCode);
    			break;
    		}
        }
            
        nRet = SearchDBPoint(ullUserId, vecCode, vecPoint, strRetCode);
        if (0 != nRet)
        {
            ERROR("Call SearchDBPoint error. llUID: "<<ullUserId<<", code size: "<<vecCode.size()
                <<", strRetCode: "<<strRetCode<<", nRet: "<<nRet);
            break;
        }
    
        for (int i=0; i<vecPoint.size(); i++)
        {
            nRet = vecPoint[i].Point2Vec(nDate, setTrade);
            if (0 != nRet)
            {
                strRetCode = CErrOutOfRange;
                ERROR("Call vecPoint[i].Point2Vec error. nDate: "<<nDate<<", llUID: "<<ullUserId
                    <<", code: "<<vecPoint[i].m_strCode<<", strRetCode: "<<strRetCode);
                break;
            }
        }
    
        if (0 != nRet)
        {
            break;
        }

        nRet = CreateStat(vecPoint, strCycle, mapStat, vecDate, strRetCode);
        if (0 != nRet)
        {
            ERROR("Call CreateStat error. llUID: "<<ullUserId<<", vecPoint size: "<<vecPoint.size()
                <<", strRetCode: "<<strRetCode<<", nRet: "<<nRet);
            break;
        }
    
        nRet = ShowInfo(vecPoint.size(), mapStat, vecDate, jRes, strRetCode);
        if (0 != nRet)
        {
            ERROR("Call ShowInfo error. strRetCode: "<<strRetCode);
            break;
        }
    }
    while (false);
    
    CDBPool::Instance()->PushConnect(m_pConn);
    
    return nRet;
}

int CTradePoint::SearchDBPoint(
    unsigned long long ullUID,
    string strCode,
    CPointData &objPointData,
    string &strRetCode
    )
{
    int nRet = 0;
    string strSQL = "";
    PreparedStatement* stmt = NULL;
    ResultSet* resultSet = NULL;

    objPointData.m_ullUserId = 0;
    try
    {
        strSQL = "select userID, code, point, beginDate, endDate from tb_trade_point where userID=? and code=?";
    	stmt = m_pConn->prepareStatement(strSQL.c_str());
		stmt->setInt64(1, ullUID);
        stmt->setString(2, strCode.c_str());
        resultSet = stmt->executeQuery();
        if (!resultSet->next())
        {
            objPointData.m_ullUserId = 0;
            delete resultSet;
		    delete stmt;
            return nRet;
        }

        objPointData.m_ullUserId = resultSet->getInt64(1);
        objPointData.m_strCode = resultSet->getString(2).c_str();

        istream *pis = resultSet->getBlob(3);
        pis->seekg(0, ios::end);
        streampos sz = pis->tellg(); 
        if (NULL != objPointData.m_pPoint)
        {
            free(objPointData.m_pPoint);
            objPointData.m_pPoint = NULL;
        }
        if (sz > 0)
        {
            objPointData.m_pPoint = (SPointBi*)malloc(sz);
            pis->seekg(ios::beg);
            pis->read((char*)(objPointData.m_pPoint), sz);
            objPointData.m_nSize = sz/sizeof(SPointBi);
        }

        objPointData.m_nBeginDate = resultSet->getInt(4);
        objPointData.m_nBeginDate = resultSet->getInt(5);

        delete resultSet;
		delete stmt;
    }
    CATCH_DB;

    return nRet;
}
    
int CTradePoint::SearchDBPoint(
    unsigned long long ullUID,
    vector<string> vecCode,
    vector<CPointData> &vecPointData,
    string &strRetCode
    )
{
    int nRet = 0;
    //CPointData objPointData;
    
    for (int i=0; i<vecCode.size(); i++)
    {
        CPointData objPointData; // 每次都重新定义一次吧，否则函数里边判断指针非空会将还在使用的指针释放掉的
        nRet = SearchDBPoint(ullUID, vecCode[i], objPointData, strRetCode);
        if (0 != nRet)
        {
            ERROR("Call SearchDBPoint error. code: "<<vecCode[i]<<", strRetCode: "<<strRetCode);
            return nRet;
        }
        if (0 != objPointData.m_ullUserId)
        {
            vecPointData.push_back(objPointData);
        }
    }

    return nRet;
}

int CTradePoint::UpdateDBPoint(
    CPointData objPoint,
    bool bExist,
    string &strRetCode
    )
{
    int iRet = 0;
    string strSQL = "";
    PreparedStatement* stmt = NULL;

    try
    {
        if (true == bExist)
        {
            if ((0 == objPoint.m_nSize) || (NULL == objPoint.m_pPoint))
            {
                strSQL = "delete from tb_trade_point where userid=? and code=?";
                stmt = m_pConn->prepareStatement(strSQL.c_str());
                stmt->setInt64(1, objPoint.m_ullUserId);
                stmt->setString(2, objPoint.m_strCode.c_str());
                stmt->executeUpdate();
            }
            else
            {
                strSQL = "update tb_trade_point set beginDate=?, endDate=?, point=? where userid=? and code=?";
                DEBUG("strSQL: "<<strSQL<<", m_nBeginDate: "<<objPoint.m_nBeginDate<<", m_nEndData: "
                    <<objPoint.m_nEndData<<", m_nSize: "<<objPoint.m_nSize<<", userid: "<<objPoint.m_ullUserId
                    <<", m_strCode: "<<objPoint.m_strCode<<", m_pPoint: "<<objPoint.m_pPoint);
                stmt = m_pConn->prepareStatement(strSQL.c_str());
                stmt->setInt(1, objPoint.m_nBeginDate);
                stmt->setInt(2, objPoint.m_nEndData);
                DataBuf buffer((char*)(objPoint.m_pPoint), objPoint.m_nSize*sizeof(SPointBi));
                istream s(&buffer);
                stmt->setBlob(3, &s);
                stmt->setInt64(4, objPoint.m_ullUserId);
                stmt->setString(5, objPoint.m_strCode.c_str());
                stmt->executeUpdate();
                /*DEBUG("strSQL: "<<strSQL<<", m_nBeginDate: "<<objPoint.m_nBeginDate<<", m_nEndData: "
                    <<objPoint.m_nEndData<<", m_nSize: "<<objPoint.m_nSize<<", userid: "<<objPoint.m_ullUserId
                    <<", m_strCode: "<<objPoint.m_strCode<<", m_pPoint: "<<objPoint.m_pPoint);
                strSQL = "update tb_trade_point set beginDate=?, endDate=? where userid=? and code=?";
                DEBUG("strSQL: "<<strSQL<<", m_nBeginDate: "<<objPoint.m_nBeginDate<<", m_nEndData: "
                    <<objPoint.m_nEndData<<", m_nSize: "<<objPoint.m_nSize<<", userid: "<<objPoint.m_ullUserId
                    <<", m_strCode: "<<objPoint.m_strCode<<", m_pPoint: "<<objPoint.m_pPoint);
                stmt = m_pConn->prepareStatement(strSQL.c_str());
                stmt->setInt(1, objPoint.m_nBeginDate);
                stmt->setInt(2, objPoint.m_nEndData);
                stmt->setInt64(3, objPoint.m_ullUserId);
                stmt->setString(4, objPoint.m_strCode.c_str());
                stmt->executeUpdate();*/
            }
        }
        else
        {
            strSQL = "insert into tb_trade_point(userid, code, point, begindate, enddate) values(?,?,?,?,?)";
            stmt = m_pConn->prepareStatement(strSQL.c_str());
            stmt->setInt64(1, objPoint.m_ullUserId);
            stmt->setString(2, objPoint.m_strCode.c_str());
            DataBuf buffer((char*)(objPoint.m_pPoint), objPoint.m_nSize*sizeof(SPointBi));
            istream s(&buffer);
            stmt->setBlob(3, &s);
            stmt->setInt(4, objPoint.m_nBeginDate);
            stmt->setInt(5, objPoint.m_nEndData);
            stmt->executeUpdate();
        }
        
        delete stmt;
        stmt = NULL;
    }
    CATCH_DB;
    
    return iRet;
}

int CTradePoint::CreateStat(
    const vector<CPointData> &vecPoint,
    string strCycle,
    map<unsigned long long, CPointStat> &mapStat,
    vector<int> &vecDate,
    string &strRetCode
    )
{
    int nBeginDate = 0;
    int nEndDate = 0;
    int nInterval = 0;
    SPointBi const *pBiPoint = NULL;
    CPointStat objStat;
    CCalendar objCal;
    map<unsigned long long, CPointStat>::iterator iter;
    map<unsigned long long, CPointStat>::reverse_iterator riter; // = mymap.rbegin();
    map<unsigned long long, CPointStat> mapTempStat;
    unsigned long long ullKey;

    if (0 == vecPoint.size())
    {
        DEBUG("No data. 0 == vecPoint.size()");
        return 0;
    }

    // ��һ��ͳ�ƣ�����map��
    for (int i=0; i<vecPoint.size(); i++)
    {
        for (int j=0; j< vecPoint[i].m_vecPoint.size(); j++)
        {
            pBiPoint = &(vecPoint[i].m_vecPoint[j]);
            if (0 == mapStat.count(pBiPoint->m_nDate))
            {
                objStat.m_nDate = pBiPoint->m_nDate;
                objStat.m_nTrade = pBiPoint->m_nTrade;
                objStat.m_nNum = 0;
                objStat.m_vecCode.clear();
                mapStat[pBiPoint->m_nDate] = objStat;
                DEBUG("pBiPoint->m_nDate: "<<pBiPoint->m_nDate<<", pBiPoint->m_nTrade: "<<pBiPoint->m_nTrade
                    <<", vecPoint[i].m_strCode: "<<vecPoint[i].m_strCode);
            }
            mapStat[pBiPoint->m_nDate].m_nNum++;
            mapStat[pBiPoint->m_nDate].m_vecCode.push_back(vecPoint[i].m_strCode);
        }
    }

    // �õ����ڷ�Χ
    iter = mapStat.begin();
    riter = mapStat.rbegin();
    nBeginDate = iter->second.m_nDate;
    nEndDate = riter->second.m_nDate;
    DEBUG("mapStat.size(): "<<mapStat.size()<<", iter->first: "<<iter->first<<", iter->second.m_nDate: "<<iter->second.m_nDate);

    if ("day" == strCycle)
    {
        nInterval = objCal.IntervalDays(nBeginDate);
        if (0 >= nInterval)
        {
            if (0 == objCal.AddDay(nInterval-2*nInterval))
            {
                strRetCode = CErrOutOfRange;
                ERROR("Call objCal.AddDay error. nInterval: "<<nInterval<<", nBeginDate: "
                    <<nBeginDate<<", strRetCode: "<<strRetCode);
                return 1;
            }
        }
        else
        {
            if (0 == objCal.SubDay(nInterval))
            {
                strRetCode = CErrOutOfRange;
                ERROR("Call objCal.AddDay error. nInterval: "<<nInterval<<", nBeginDate: "
                    <<nBeginDate<<", strRetCode: "<<strRetCode);
                return 1;
            }
        }
        
        vecDate.clear();
        vecDate.push_back(objCal.GetDate());
        while (objCal.GetDate() < nEndDate)
        {
            if (0 == objCal.AddDay(1))
            {
                strRetCode = CErrOutOfRange;
                ERROR("Call objCal.AddDay error. objCal.date: "<<objCal.GetDate()
                    <<", internal: 1, strRetCode: "<<strRetCode);
                return 1;
            }
            if ((0 == objCal.m_iWeekDay) || (6 == objCal.m_iWeekDay))
            {
                continue;
            }
            vecDate.push_back(objCal.GetDate());
        }
        
        return 0;  // ���߲���Ҫ�ٹ���һ��map
    }
    else if ("week" == strCycle)
    {
        nInterval = objCal.IntervalDays(nBeginDate);
        if (0 >= nInterval)
        {
            if (0 == objCal.AddDay(nInterval-2*nInterval))
            {
                strRetCode = CErrOutOfRange;
                ERROR("Call objCal.AddDay error. nInterval: "<<nInterval<<", strRetCode: "<<strRetCode);
                return 1;
            }
        }
        else
        {
            if (0 == objCal.SubDay(nInterval))
            {
                strRetCode = CErrOutOfRange;
                ERROR("Call objCal.AddDay error. nInterval: "<<nInterval<<", strRetCode: "<<strRetCode);
                return 1;
            }
        }
        if (0 == objCal.SubDay(7+objCal.m_iWeekDay-5))  // 下一个周五
        {
            strRetCode = CErrOutOfRange;
            ERROR("Call objCal.SubDay error. objCal.date: "<<objCal.GetDate()<<", Interval: "
                <<7+objCal.m_iWeekDay-5<<", strRetCode: "<<strRetCode);
            return 1;
        }
        
        vecDate.clear();
        while (objCal.GetDate() < nEndDate)
        {
            vecDate.push_back(objCal.GetDate());
            if (0 == objCal.AddDay(7))
            {
                strRetCode = CErrOutOfRange;
                ERROR("Call objCal.AddDay error. objCal.date: "<<objCal.GetDate()
                    <<", internal: 7, strRetCode: "<<strRetCode);
                return 1;
            }
        }
        vecDate.push_back(objCal.GetDate());
    }
    else if ("month" == strCycle)
    {
        int nYear = nBeginDate/10000;
        int nMonth = nBeginDate/100;
        nMonth = nMonth%100;
        int nTemp = nYear*10000 + nMonth*100 + 1;
        
        vecDate.clear();
        while (nTemp < nEndDate)
        {
            vecDate.push_back(nTemp);
            nMonth++;
            if (nMonth > 12)
            {
                nYear++;
                nMonth -= 12;
            }
            nTemp = nYear*10000 + nMonth*100 + 1;
        }

    }
    else
    {
        strRetCode = CErrOutOfRange;
        ERROR("strCycle value error. strCycle: "<<strCycle<<", strRetCode: "<<strRetCode);
        return 1;
    }


    // �ٴα���map���õ��������ڵ�����
    int index = vecDate.size();
    index--;
    for (riter = mapStat.rbegin(); riter != mapStat.rend(); riter++)
    {
        while (0 <= index)
        {
            if (riter->second.m_nDate >= vecDate[index])
            {
                ullKey = vecDate[index]*1000 + riter->second.m_nTrade;
                if (0 == mapTempStat.count(ullKey))
                {
                    mapTempStat[ullKey] = riter->second;
                    mapTempStat[ullKey].m_nDate = vecDate[index];
                }
                else
                {
                    mapTempStat[ullKey].m_nNum += riter->second.m_nNum;
                    mapTempStat[ullKey].m_vecCode.insert(mapTempStat[ullKey].m_vecCode.end(),
                        riter->second.m_vecCode.begin(), riter->second.m_vecCode.end());
                }
                break;
            }
            else
            {
                index--;
            }
        }
    }

    mapStat = mapTempStat;
    //reverse(vecDate.begin(), vecDate.end());

    return 0;
}

int CTradePoint::ShowInfo(
    const vector<CPointData> &vecPoint,
    json_t *&jRes, 
    string &strRetCode
    )
{
    int nBeginDate = 0;
    int nEndDate = 0;

    if (vecPoint.size() > 0)
    {
        nBeginDate  = vecPoint[0].m_nBeginDate;
        nEndDate    = vecPoint[0].m_nEndData;
    }
    
    json_t *jResult = json_object();
    json_t *jTrade = json_array();  // 季度曲线Y轴坐标
    for (int i=0; i<vecPoint.size(); i++)
    {
        json_t *jTradeItem = json_object();
        json_object_set_new(jTradeItem, "code", json_string(vecPoint[i].m_strCode.c_str()));
        json_object_set_new(jTradeItem, "begindate", json_integer(vecPoint[i].m_nBeginDate));
        json_object_set_new(jTradeItem, "enddate", json_integer(vecPoint[i].m_nEndData));
        json_t *jPoint = json_array();
        for (int j=0; j<vecPoint[i].m_vecPoint.size(); j++)
        {
            json_t *jPointItem = json_object();
            json_object_set_new(jPointItem, "trade", json_integer(vecPoint[i].m_vecPoint[j].m_nTrade));
            json_object_set_new(jPointItem, "date", json_integer(vecPoint[i].m_vecPoint[j].m_nDate));
            json_object_set_new(jPointItem, "count", json_integer(vecPoint[i].m_vecCount[j]));
            json_array_append_new(jPoint, jPointItem);
        }
        json_object_set_new(jTradeItem, "point", jPoint);
        json_array_append_new(jTrade, jTradeItem);

        if (nBeginDate > vecPoint[i].m_nBeginDate)
        {
            nBeginDate = vecPoint[i].m_nBeginDate;
        }
        if (nEndDate < vecPoint[i].m_nEndData)
        {
            nEndDate = vecPoint[i].m_nEndData;
        }
    }
    json_object_set_new(jResult, "trade", jTrade);
    json_object_set_new(jResult, "begindate", json_integer(nBeginDate));
    json_object_set_new(jResult, "enddate", json_integer(nEndDate));
    
    json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
    json_object_set_new(jRes, "result", jResult);
    
    return 0;
}

int CTradePoint::ShowInfo(
    int nCodeCnt,
    const map<unsigned long long, CPointStat> &mapStat,
    vector<int> &vecDate,
    json_t *&jRes,
    string &strRetCode
    )
{
    int nBeginDate = 0;
    int nEndDate = 0;
    int nPointCnt = 0;
    map<unsigned long long, CPointStat>::const_reverse_iterator riter;

    if ((0 == vecDate.size()) || (0 == mapStat.size()))
    {
        return 0;
    }
    
    json_t *jResult = json_object();
    json_object_set_new(jResult, "codecnt", json_integer(nCodeCnt));
    json_t *jPoint = json_array();  // 季度曲线Y轴坐标
    for (riter = mapStat.rbegin(); riter != mapStat.rend(); riter++)
    {
        nPointCnt += riter->second.m_nNum;
        json_t *jPointItem = json_object();
        json_object_set_new(jPointItem, "trade", json_integer(riter->second.m_nTrade));
        json_object_set_new(jPointItem, "count", json_integer(riter->second.m_nNum));
        json_object_set_new(jPointItem, "date", json_integer(riter->second.m_nDate));
        json_t *jCode = json_array();
        for (int j=0; j<riter->second.m_vecCode.size(); j++)
        {
            json_array_append_new(jCode, json_string(riter->second.m_vecCode[j].c_str()));
        }
        json_object_set_new(jPointItem, "code", jCode);
        json_array_append_new(jPoint, jPointItem);
    
        nEndDate = riter->first;
    }
    json_object_set_new(jResult, "pointcnt", json_integer(nPointCnt));
    json_object_set_new(jResult, "point", jPoint);

    json_t *jDate = json_array();
    for (int i=vecDate.size(); i>0; i--)
    {
        json_array_append_new(jDate, json_integer(vecDate[i-1]));
    }
    json_object_set_new(jResult, "date", jDate);
    
    json_object_set_new(jResult, "begindate", json_integer(vecDate[0]));
    json_object_set_new(jResult, "enddate", json_integer(vecDate[vecDate.size()-1]));
    
    json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
    json_object_set_new(jRes, "result", jResult);

    return 0;
}

int CTradePoint::GetUIDBySession(
    string strSession,
    unsigned long long &ullUID,
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
        ullUID = resultSet->getInt64(1);
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

