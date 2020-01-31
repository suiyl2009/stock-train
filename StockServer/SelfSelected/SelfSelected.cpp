#include "SelfSelected.h"
#include "Data.h"
#include "Log.h"
#include "Constant.h"
#include <algorithm>
#include "StrSplit.h"
#include <set>

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

/*
请求字段列表
字段名称	值类型	备注
method	字符串selected.gettab	
session	字符串	

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
result	数组[]	响应字段。数组元素tab字符串
*/
int CSelfSelected::GetTab( // 有哪些自选类型
    json_t *jReq,
	json_t *&jRes,
	string &strRetCode
    ){
    int nRet       = 0;
    json_t *jItem  = NULL;
    unsigned long long ullUserId = 0;
    string strSession = "";
    string strTab = "";
    vector<string> vecTab;
    size_t sJsonSize   = 0;

    /////////begin: 解析json，得到各字段item//////////////////////
    jItem = json_object_get(jReq, "session");
    if (!jItem || !json_is_string(jItem)) {
        strRetCode = CErrJsonType;
        ERROR("GetItem json session is not string. strRetCode: "<<strRetCode);
        return 1;
    }
    strSession = json_string_value(jItem);
    strSession = trim(strSession);
    /////////end: 解析json，得到各字段item//////////////////////

    m_pConn = CDBPool::Instance()->GetConnect();
    if (NULL == m_pConn){
        strRetCode = CErrDBConn;
        ERROR("CSelfSelected::GetItem Call CDBPool::Instance()->GetConnect() error. strRetCode: "<<strRetCode);
        return 1;
    }

    do{
        nRet = GetUIDBySession(strSession, ullUserId, strRetCode);
        if (0 != nRet) {
            ERROR("Call GetUIDBySession error. strSession: " << strSession << ", strRetCode: " << strRetCode);
            break;
        }
      
        nRet = SearchTab(ullUserId, vecTab, strRetCode);
        if (0 != nRet) {
            ERROR("Call SearchDBPoint error. llUID: "<<ullUserId<<", vecTab size: "<<vecTab.size()
                <<", strRetCode: "<<strRetCode<<", nRet: "<<nRet);
            break;
        }

        nRet = ShowTab(vecTab, jRes, strRetCode);
        if (0 != nRet) {
            ERROR("Call ShowItem error. strRetCode: "<<strRetCode);
            break;
        }
    } while (false);

    CDBPool::Instance()->PushConnect(m_pConn);

    return nRet;
}

/*
请求字段列表
字段名称	值类型	备注
method	字符串selected.item	
session	字符串	
tab	字符串	缺省返回所有item

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
result	数组[]	响应字段。数组元素code
*/
int CSelfSelected::GetItem( // 得到自选列表
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    ){
    int nRet       = 0;
    json_t *jItem  = NULL;
    unsigned long long ullUserId = 0;
    string strSession = "";
    string strTab = "";
    vector<string> vecCode;
    size_t sJsonSize   = 0;
    
    /////////begin: 解析json，得到各字段item//////////////////////
    jItem = json_object_get(jReq, "session");
    if (!jItem || !json_is_string(jItem)) {
        strRetCode = CErrJsonType;
        ERROR("GetItem json session is not string. strRetCode: "<<strRetCode);
        return 1;
    }
    strSession = json_string_value(jItem);
    strSession = trim(strSession);

    jItem = json_object_get(jReq, "tab");
	if (jItem && json_is_string(jItem)) {
        strTab = json_string_value(jItem);
        strTab = trim(strTab);
	}
    /////////end: 解析json，得到各字段item//////////////////////
    
    m_pConn = CDBPool::Instance()->GetConnect();
    if (NULL == m_pConn){
        strRetCode = CErrDBConn;
        ERROR("CSelfSelected::GetItem Call CDBPool::Instance()->GetConnect() error. strRetCode: "<<strRetCode);
        return 1;
    }
    
    do{
        nRet = GetUIDBySession(strSession, ullUserId, strRetCode);
        if (0 != nRet) {
            ERROR("Call GetUIDBySession error. strSession: " << strSession << ", strRetCode: " << strRetCode);
            break;
        }
        
        nRet = SearchItem(ullUserId, strTab, vecCode, strRetCode);
        if (0 != nRet) {
            ERROR("Call SearchDBPoint error. llUID: "<<ullUserId<<", code size: "<<vecCode.size()
                <<", strRetCode: "<<strRetCode<<", nRet: "<<nRet);
            break;
        }
    
        nRet = ShowItem(vecCode, jRes, strRetCode);
        if (0 != nRet) {
            ERROR("Call ShowItem error. strRetCode: "<<strRetCode);
            break;
        }
    }
    while (false);

    CDBPool::Instance()->PushConnect(m_pConn);
    
    return nRet;
}

/*
请求字段列表
字段名称	值类型	备注
method	字符串selected.item	
session	字符串	
tab	字符串	缺省返回所有item
item	数组[]	数组元素code

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
*/
int CSelfSelected::AddItem( // 添加自选股
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    ){
    int nRet       = 0;
    json_t *jItem  = NULL;
    unsigned long long ullUserId = 0;
    string strSession = "";
    string strTab = "";
    vector<string> vecCode;
    size_t sJsonSize   = 0;
    
    /////////begin: 解析json，得到各字段item//////////////////////
    jItem = json_object_get(jReq, "session");
    if (!jItem || !json_is_string(jItem)) {
        strRetCode = CErrJsonType;
        ERROR("AddItem json session is not string. strRetCode: "<<strRetCode);
        return 1;
    }
    strSession = json_string_value(jItem);
    strSession = trim(strSession);
    
    jItem = json_object_get(jReq, "tab");
    if (!jItem || !json_is_string(jItem)) {
        strRetCode = CErrJsonType;
        ERROR("AddItem json tab is not string. strRetCode: "<<strRetCode);
        return 1;
    }
    strTab = json_string_value(jItem);
    strTab = trim(strTab);

    jItem = json_object_get(jReq, "item");
    if (!jItem || !json_is_array(jItem)) {
        strRetCode = CErrJsonType;
        ERROR("AddItem Get json item error. strRetCode: "<<strRetCode);
        return 1;
    }
    sJsonSize = json_array_size(jItem);
    if (0 == sJsonSize){
        strRetCode = CErrJsonType;
        ERROR("AddItem Get json item error. strRetCode: "<<strRetCode);
        return 1;
    }
    
    for (int i=0; i<sJsonSize; i++) {
        vecCode.push_back(json_string_value(json_array_get(jItem, i)));
    }
    /////////end: 解析json，得到各字段item//////////////////////
    
    m_pConn = CDBPool::Instance()->GetConnect();
    if (NULL == m_pConn) {
        strRetCode = CErrDBConn;
        ERROR("CSelfSelected::GetItem Call CDBPool::Instance()->GetConnect() error. strRetCode: "<<strRetCode);
        return 1;
    }
    
    do{
        nRet = GetUIDBySession(strSession, ullUserId, strRetCode);
        if (0 != nRet) {
            ERROR("Call GetUIDBySession error. strSession: " << strSession << ", strRetCode: " << strRetCode);
            break;
        }
        
        nRet = UpdateItem(ullUserId, strTab, true, vecCode, strRetCode);
        if (0 != nRet) {
            ERROR("Call SearchDBPoint error. llUID: "<<ullUserId<<", code size: "<<vecCode.size()
                <<", strRetCode: "<<strRetCode<<", nRet: "<<nRet);
            break;
        }
    
        nRet = ShowItem(vecCode, jRes, strRetCode);
        if (0 != nRet) {
            ERROR("Call ShowItem error. strRetCode: "<<strRetCode);
            break;
        }
    }
    while (false);
    
    CDBPool::Instance()->PushConnect(m_pConn);
    
    return nRet;

}

int CSelfSelected::GetUIDBySession(
    string strSession,
    unsigned long long &ullUID,
    string &strRetCode
    ){
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

int CSelfSelected::SearchTab(
    unsigned long long ullUserId, 
    vector<string> &vecTab, 
    string &strRetCode
    ){
    int nRet = 0;
    string strSQL = "";
    PreparedStatement* stmt = NULL;
    ResultSet* resultSet = NULL;

    try {
        strSQL = "select tab from tb_selected where userid=?";
        stmt = m_pConn->prepareStatement(strSQL.c_str());
        stmt->setInt64(1, ullUserId);
        resultSet = stmt->executeQuery();
        vecTab.clear();
        while (resultSet->next()) {
            vecTab.push_back(resultSet->getString(1).c_str());
        }
    
        delete resultSet;
        delete stmt;
    }
    CATCH_DB;
    
    return nRet;
}

int CSelfSelected::SearchItem(
    unsigned long long ullUserId, 
    const string &strTab, 
    vector<string> &vecCode, 
    string &strRetCode
    ) {
    int nRet = 0;
    string strSQL = "";
    string strCode = "";
    PreparedStatement* stmt = NULL;
    ResultSet* resultSet = NULL;

    vecCode.clear();
    try {
        strSQL = "select code from tb_selected where userid=? and tab=?";
        stmt = m_pConn->prepareStatement(strSQL.c_str());
        stmt->setInt64(1, ullUserId);
        stmt->setString(2, strTab.c_str());
        resultSet = stmt->executeQuery();
        if (!resultSet->next()) {
            delete resultSet;
            delete stmt;
            return 0;
        }
        strCode = resultSet->getString(1).c_str();
        if (strCode.empty()){
            return 0;
        }
    	CStrSplit css;
		css.SetString(strCode, ","); // 切割日期字符串
		for (int i=0; i<css.Count(); i++) {
            vecCode.push_back(css.Item(i));
        }
        delete resultSet;
        delete stmt;
    }
    CATCH_DB;
    
    return nRet;
}

int CSelfSelected::UpdateItem(
    unsigned long long ullUserId, 
    const string &strTab, 
    bool bAdd, 
    vector<string> &vecCode, 
    string &strRetCode
    ){
    int nRet = 0;
    string strSQL = "";
    string strCode = "";
    PreparedStatement* stmt = NULL;
    ResultSet* resultSet = NULL;
    set<string> setCode;

    try {
        strSQL = "select code from tb_selected where userid=? and tab=?";
        stmt = m_pConn->prepareStatement(strSQL.c_str());
        stmt->setInt64(1, ullUserId);
        stmt->setString(2, strTab.c_str());
        resultSet = stmt->executeQuery();
        if (!resultSet->next()) {
            delete resultSet;
            delete stmt;
            return 0;
        }
        strCode = resultSet->getString(1).c_str();
    	CStrSplit css;
		css.SetString(strCode, ","); // 切割日期字符串
		for (int i=0; i<css.Count(); i++) {
            setCode.insert(css.Item(i));
        }
        delete resultSet;
        delete stmt;
    }
    CATCH_DB;

    for (int i=0; i<vecCode.size(); i++) {
        if (true == bAdd) {
            setCode.insert(vecCode[i]);
        }
        else {
            setCode.erase(vecCode[i]);
        }
    }
    strCode = "";
    for (set<string>::iterator iter=setCode.begin(); iter != setCode.end(); iter++) {
        if (iter != setCode.begin()){
            strCode.append(",");
        }
        strCode.append(*iter);
    }

    try {
        strSQL = "update tb_selected set code = ? where userid=? and tab=?";
        stmt = m_pConn->prepareStatement(strSQL.c_str());
        stmt->setString(1, strCode.c_str());
        stmt->setInt64(2, ullUserId);
        stmt->setString(3, strTab.c_str());
        stmt->executeUpdate();
    }
    CATCH_DB;
    return nRet;    
}
    
int CSelfSelected::ShowTab(
    const vector<string> &vecTab, 
    json_t *&jRes, 
    string &strRetCode
    ) {
    json_t *jResult = json_array();
    for (int i=0; i<vecTab.size(); i++)
    {
        json_array_append_new(jResult, json_string(vecTab[i].c_str()));
    }
    
    json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
    json_object_set_new(jRes, "result", jResult);
    
    return 0;
}

int CSelfSelected::ShowItem(
    const vector<string> &vecCode, 
    json_t *&jRes, 
    string &strRetCode
    ) {
    json_t *jResult = json_array();
    for (int i=0; i<vecCode.size(); i++)
    {
        json_array_append_new(jResult, json_string(vecCode[i].c_str()));
    }
    
    json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
    json_object_set_new(jRes, "result", jResult);
    
    return 0;
}

