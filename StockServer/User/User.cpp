#include "User.h"
#include "Constant.h"
#include "Log.h"
#include "Mail.h"
#include "StrSplit.h"

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
method	字符串user.register	
name	字符串	用户名
email	字符串	邮箱
password	字符串	密码

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
result	{“info”: {“name”: “xxx”, “email”: “xxx”, “password”: “xxx”}, 
         “error”: {“item”: “xxx”, “type”: xxx}}	响应字段值。
type=1:格式错误；
type=2:重复
*/
int CUser::SendVerifyCode(        // 发送验证码
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
	int nRet = 0;
	json_t *jItem = NULL;
	string strName = "";
	string strEmail = "";
	string strPassword = "";
	string strVerifyCode = "";
	string strErrItem = "";
	int nType = 0;

	/////////begin: 解析json，得到各字段item//////////////////////
	jItem = json_object_get(jReq, "name");
	if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("SendVerifyCode json name is not string. strRetCode: " << strRetCode);
		return 1;
	}
	strName = json_string_value(jItem);
    strName = trim(strName);

	jItem = json_object_get(jReq, "email");
	if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("SendVerifyCode json email is not string. strRetCode: " << strRetCode);
		return 1;
	}
	strEmail = json_string_value(jItem);
    strEmail = trim(strEmail);

	jItem = json_object_get(jReq, "password");
	if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("SendVerifyCode json password is not string. strRetCode: " << strRetCode);
		return 1;
	}
	strPassword = json_string_value(jItem);
    strPassword = trim(strPassword);

	if (strName.empty() || strEmail.empty() || strPassword.empty())
	{
		strRetCode = CErrJsonType;
		ERROR("Item empty. strName["<<strName<<"], strEmail["<<strEmail<<"], strPassword["<<strPassword<<"] strRetCode: " << strRetCode);
		return 1;
	}
	/////////end: 解析json，得到各字段item//////////////////////


	m_pConn = CDBPool::Instance()->GetConnect();
	if (NULL == m_pConn)
	{
		strRetCode = CErrDBConn;
		ERROR(" CUser::SendVerifyCode Call CDBPool::Instance()->GetConnect() error. strRetCode: " << strRetCode);
		return 1;
	}

	do
	{
		//////begin: 验证输入信息///////////
		nRet = CheckName(strName, nType, strRetCode);
		if (0 != nRet)
		{
			ERROR("Call CheckName error. strName: " << strName << ", nType: " << nType << ", strRetCode: " << strRetCode);
			break;
		}
		if (0 != nType)
		{
			strErrItem = "name";
			break;
		}

		nRet = CheckEmail(strEmail, nType, strRetCode);
		if (0 != nRet)
		{
			ERROR("Call CheckEmail error. strEmail: " << strEmail << ", nType: " << nType << ", strRetCode: " << strRetCode);
			break;
		}
		if (0 != nType)
		{
			strErrItem = "email";
			break;
		}

		nRet = CheckPassword(strPassword, nType, strRetCode);
		if (0 != nRet)
		{
			ERROR("Call CheckPassword error. strPassword: " << strPassword << ", nType: " << nType << ", strRetCode: " << strRetCode);
			break;
		}
		if (0 != nType)
		{
			strErrItem = "password";
			break;
		}
		//////end: 验证输入信息///////////

		nRet = GetVerifyCode(strVerifyCode, strRetCode);  // 得到验证码
		if (0 != nRet)
		{
			ERROR("Call GetVerifyCode error. strRetCode: " << strRetCode);
			break;
		}

		nRet = InsertRegister(strName, strEmail, strPassword, strVerifyCode, strRetCode);  // 信息保存到临时表
		if (0 != nRet)
		{
			ERROR("Call InsertRegister error. strName: "<<strName<<", strEmail: "<<strEmail<<", strPassword: "<<strPassword
				<<", strVerifyCode: "<<strVerifyCode<<", strRetCode: "<<strRetCode);
			break;
		}

		nRet = SendMail(strEmail, "用户注册验证码", "用户注册验证码为：" + strVerifyCode);  // 发送验证码
		if (0 != nRet)
		{
			strRetCode = CErrSendMail;
			ERROR("Call SendMail error. nRet: "<<nRet<<", strRetCode: "<<strRetCode);
			break;
		}
	} while (false);

	CDBPool::Instance()->PushConnect(m_pConn);

	if (0 == nRet) // 组装报文
	{
		//retcode 字符串   错误码，成功为0000
		//	result{ “info”: {“name”: “xxx”, “email” : “xxx”, “password” : “xxx”},
		//“error” : {“item”: “xxx”, “type” : xxx} } 响应字段值。
		json_t *jResult = json_object();
		json_t *jInfo = json_object();
		json_object_set_new(jInfo, "name", json_string(strName.c_str()));
		json_object_set_new(jInfo, "email", json_string(strEmail.c_str()));
		json_object_set_new(jInfo, "password", json_string(strPassword.c_str()));
		json_object_set_new(jResult, "info", jInfo);
		if (0 != nType)
		{
			json_t *jError = json_object();
			json_object_set_new(jError, "item", json_string(strErrItem.c_str()));
			json_object_set_new(jError, "type", json_integer(nType));
			json_object_set_new(jResult, "error", jError);
		}
		json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
		json_object_set_new(jRes, "result", jResult);
	}

	return nRet;
}

/*
请求字段列表
字段名称    值类型   备注
method  字符串user.checkitem 
name    字符串   用户名
email   字符串   邮箱
password    字符串   密码

响应字段列表
字段名称    值类型   备注
method  同上  同上
retcode 字符串   错误码，成功为0000
result  {“info”: {“name”: “xxx”, “email”: “xxx”, “password”: “xxx”}, 
         “error”: {“item”: “xxx”, “type”: xxx}} 响应字段值。
type=1:格式错误；
type=2:重复
*/
int CUser::CheckItem(        // 验证字段
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
	int nRet = 0;
	json_t *jItem = NULL;
	string strName = "";
	string strEmail = "";
	string strPassword = "";
	string strErrItem = "";
	int nType = 0;

	/////////begin: 解析json，得到各字段item//////////////////////
	jItem = json_object_get(jReq, "name");
	if (jItem && json_is_string(jItem)) {
		strName = json_string_value(jItem);
        strName = trim(strName);
	}

	jItem = json_object_get(jReq, "email");
	if (jItem && json_is_string(jItem)) {
		strEmail = json_string_value(jItem);
        strEmail = trim(strEmail);
	}

	jItem = json_object_get(jReq, "password");
	if (jItem && json_is_string(jItem)) {
		strPassword = json_string_value(jItem);
        strPassword = trim(strPassword);
	}
	/////////end: 解析json，得到各字段item//////////////////////


	m_pConn = CDBPool::Instance()->GetConnect();
	if (NULL == m_pConn)
	{
		strRetCode = CErrDBConn;
		ERROR(" CUser::CheckItem Call CDBPool::Instance()->GetConnect() error. strRetCode: " << strRetCode);
		return 1;
	}

	do
	{
		// 验证输入信息
		if (false == strName.empty())
		{
			nRet = CheckName(strName, nType, strRetCode);
			if (0 != nRet)
			{
				ERROR("Call CheckName error. strName: "<<strName<<", nType: "<<nType<<", strRetCode: "<<strRetCode);
				break;
			}
			if (0 != nType)
			{
				strErrItem = "name";
				break;
			}
		}

		if (false == strEmail.empty())
		{
			nRet = CheckEmail(strEmail, nType, strRetCode);
			if (0 != nRet)
			{
				ERROR("Call CheckEmail error. strEmail: " << strEmail << ", nType: " << nType << ", strRetCode: " << strRetCode);
				break;
			}
			if (0 != nType)
			{
				strErrItem = "email";
				break;
			}
		}

		if (false == strPassword.empty())
		{
			nRet = CheckPassword(strPassword, nType, strRetCode);
			if (0 != nRet)
			{
				ERROR("Call CheckPassword error. strPassword: " << strPassword << ", nType: " << nType << ", strRetCode: " << strRetCode);
				break;
			}
			if (0 != nType)
			{
				strErrItem = "password";
				break;
			}
		}

	} while (false);

	CDBPool::Instance()->PushConnect(m_pConn);

	if (0 == nRet) // 组装报文
	{
		//retcode 字符串   错误码，成功为0000
		//	result{ “info”: {“name”: “xxx”, “email” : “xxx”, “password” : “xxx”},
		//“error” : {“item”: “xxx”, “type” : xxx} } 响应字段值。
		json_t *jResult = json_object();
		json_t *jInfo   = json_object();
		json_object_set_new(jInfo, "name", json_string(strName.c_str()));
		json_object_set_new(jInfo, "email", json_string(strEmail.c_str()));
		json_object_set_new(jInfo, "password", json_string(strPassword.c_str()));
		json_object_set_new(jResult, "info", jInfo);
		if (0 != nType)
		{
			json_t *jError = json_object();
			json_object_set_new(jError, "item", json_string(strErrItem.c_str()));
			json_object_set_new(jError, "type", json_integer(nType));
			json_object_set_new(jResult, "error", jError);
		}
		json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
		json_object_set_new(jRes, "result", jResult);
	}

	return nRet;
}

/*
请求字段列表
字段名称	值类型	备注
method	字符串user.register
name	字符串	用户名
email	字符串	邮箱
verifycode	字符串	验证码
password	字符串	密码

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
result{ “name”: “xxx”, “email” : “xxx”, “type” : xxx }	
响应字段值。type = 0:注册成功；type = 1：验证成功；type = 2：name重复；type = 3：email重复；
type = 4：verifycode错误；type = 5：password错误

备注：未填入验证码时发送的请求为验证信息是否有重复；填入验证码时发送的请求为提交用户注册。
*/
int CUser::Register(        // 用户注册
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
	int nRet = 0;
	json_t *jItem = NULL;
	string strName = "";
	string strEmail = "";
	string strPassword = "";
	string strVerifyCode = "";
	string strErrItem = "";
	int nType = 0;

	/////////begin: 解析json，得到各字段item//////////////////////
	jItem = json_object_get(jReq, "name");
	if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("SendVerifyCode json name is not string. strRetCode: " << strRetCode);
		return 1;
	}
	strName = json_string_value(jItem);
    strName = trim(strName);

	jItem = json_object_get(jReq, "email");
	if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("SendVerifyCode json email is not string. strRetCode: " << strRetCode);
		return 1;
	}
	strEmail = json_string_value(jItem);
    strEmail = trim(strEmail);

	jItem = json_object_get(jReq, "verifycode");
	if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("SendVerifyCode json verifycode is not string. strRetCode: " << strRetCode);
		return 1;
	}
	strVerifyCode = json_string_value(jItem);
    strVerifyCode = trim(strVerifyCode);

	jItem = json_object_get(jReq, "password");
	if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("SendVerifyCode json password is not string. strRetCode: " << strRetCode);
		return 1;
	}
	strPassword = json_string_value(jItem);
    strPassword = trim(strPassword);

	if (strName.empty() || strEmail.empty() || strPassword.empty() || strVerifyCode.empty())
	{
		strRetCode = CErrJsonType;
		ERROR("Item empty. strName[" << strName << "], strEmail[" << strEmail << "], strPassword[" << strPassword 
			<< "] strRetCode: " << strRetCode);
		return 1;
	}
	/////////end: 解析json，得到各字段item//////////////////////


	m_pConn = CDBPool::Instance()->GetConnect();
	if (NULL == m_pConn)
	{
		strRetCode = CErrDBConn;
		ERROR(" CUser::SendVerifyCode Call CDBPool::Instance()->GetConnect() error. strRetCode: " << strRetCode);
		return 1;
	}

	do
	{
		//////begin: 验证输入信息///////////
		nRet = CheckName(strName, nType, strRetCode);
		if (0 != nRet)
		{
			ERROR("Call CheckName error. strName: " << strName << ", nType: " << nType << ", strRetCode: " << strRetCode);
			break;
		}
		if (0 != nType)
		{
			strErrItem = "name";
			break;
		}

		nRet = CheckEmail(strEmail, nType, strRetCode);
		if (0 != nRet)
		{
			ERROR("Call CheckEmail error. strEmail: " << strEmail << ", nType: " << nType << ", strRetCode: " << strRetCode);
			break;
		}
		if (0 != nType)
		{
			strErrItem = "email";
			break;
		}

		nRet = CheckPassword(strPassword, nType, strRetCode);
		if (0 != nRet)
		{
			ERROR("Call CheckPassword error. strPassword: " << strPassword << ", nType: " << nType << ", strRetCode: " << strRetCode);
			break;
		}
		if (0 != nType)
		{
			strErrItem = "password";
			break;
		}
		//////end: 验证输入信息///////////

		nRet = InsertUserInfo(strName, strEmail, strPassword, strVerifyCode, strRetCode);  // 新用户信息保存到用户表
		if (0 != nRet)
		{
			ERROR("Call InsertUserInfo error. strName: " << strName << ", strEmail: " << strEmail << ", strPassword: " << strPassword
				<< ", strVerifyCode: " << strVerifyCode << ", strRetCode: " << strRetCode);
			break;
		}

	} while (false);

	CDBPool::Instance()->PushConnect(m_pConn);

	if (0 == nRet) // 组装报文
	{
		//retcode 字符串   错误码，成功为0000
		//	result{ “info”: {“name”: “xxx”, “email” : “xxx”, “password” : “xxx”},
		//“error” : {“item”: “xxx”, “type” : xxx} } 响应字段值。
		json_t *jResult = json_object();
		json_t *jInfo = json_object();
		json_object_set_new(jInfo, "name", json_string(strName.c_str()));
		json_object_set_new(jInfo, "email", json_string(strEmail.c_str()));
		json_object_set_new(jInfo, "password", json_string(strPassword.c_str()));
		json_object_set_new(jResult, "info", jInfo);
		if (0 != nType)
		{
			json_t *jError = json_object();
			json_object_set_new(jError, "item", json_string(strErrItem.c_str()));
			json_object_set_new(jError, "type", json_integer(nType));
			json_object_set_new(jResult, "error", jError);
		}
		json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
		json_object_set_new(jRes, "result", jResult);
	}

	return nRet;
}

/*
请求字段列表
字段名称	值类型	备注
method	字符串user.login
identity	字符串	用户名或邮箱
password	字符串	密码

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
result{ “name”:”xxx”, “session” : ”xxx” }
*/
int CUser::Login(  // 用户登录
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
	int nRet = 0;
	json_t *jItem = NULL;
	string strIdentity = "";
	int nType = 0;
	string strPassword = "";
	unsigned long long ullUID = 0;
	string strName = "";
	string strSession = "";

	/////////begin: 解析json，得到各字段item//////////////////////
	jItem = json_object_get(jReq, "identity");
	if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("SendVerifyCode json identity is not string. strRetCode: " << strRetCode);
		return 1;
	}
	strIdentity = json_string_value(jItem);
    strIdentity = trim(strIdentity);

	jItem = json_object_get(jReq, "password");
	if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("SendVerifyCode json password is not string. strRetCode: " << strRetCode);
		return 1;
	}
	strPassword = json_string_value(jItem);
    strPassword = trim(strPassword);

	if (strIdentity.empty() || strPassword.empty())
	{
		strRetCode = CErrJsonType;
		ERROR("Item empty. strIdentity[" << strIdentity << "], strPassword[" << strPassword << "] strRetCode: " << strRetCode);
		return 1;
	}

	nType = 1; // 1：name；2：email
	for (int i = 0; i < strIdentity.length(); i++)
	{
		if ('@' == strIdentity[i])
		{
			nType = 2;
			break;
		}
	}
	/////////end: 解析json，得到各字段item//////////////////////


	m_pConn = CDBPool::Instance()->GetConnect();
	if (NULL == m_pConn)
	{
		strRetCode = CErrDBConn;
		ERROR(" CUser::SendVerifyCode Call CDBPool::Instance()->GetConnect() error. strRetCode: " << strRetCode);
		return 1;
	}

	do
	{
		nRet = VerifyUser(strIdentity, nType, strPassword, ullUID, strName, strRetCode);
		if (0 != nRet)
		{
			ERROR("Call VerifyUser error. strIdentity: " << strIdentity << ", nType: " << nType << ", strPassword: " 
				<< strPassword << ", strRetCode: " << strRetCode);
			break;
		}

		nRet = CreateSession(ullUID, strSession, strRetCode);
		if (0 != nRet)
		{
			ERROR("Call CreateSession error. ullUID: " << ullUID << ", strRetCode: " << strRetCode);
			break;
		}

		json_t *jResult = json_object();
		json_object_set_new(jResult, "name", json_string(strName.c_str()));
		json_object_set_new(jResult, "session", json_string(strSession.c_str()));
		json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
		json_object_set_new(jRes, "result", jResult);
	} while (false);

	CDBPool::Instance()->PushConnect(m_pConn);

	return nRet;
}

/*
请求字段列表
字段名称	值类型	备注
method	字符串user.logout
session	字符串

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
*/
int CUser::Logout(  // 用户退出
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
	int nRet = 0;
	json_t *jItem = NULL;
	string strSession = "";

	/////////begin: 解析json，得到各字段item//////////////////////
	jItem = json_object_get(jReq, "session");
	if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("Logout json session is not string. strRetCode: " << strRetCode);
		return 1;
	}
	strSession = json_string_value(jItem);
    strSession = trim(strSession);

	if (strSession.empty())
	{
		strRetCode = CErrJsonType;
		ERROR("strSession empty. strSession[" << strSession << "] strRetCode: " << strRetCode);
		return 1;
	}
	/////////end: 解析json，得到各字段item//////////////////////


	m_pConn = CDBPool::Instance()->GetConnect();
	if (NULL == m_pConn)
	{
		strRetCode = CErrDBConn;
		ERROR(" CUser::SendVerifyCode Call CDBPool::Instance()->GetConnect() error. strRetCode: " << strRetCode);
		return 1;
	}

	do
	{
		nRet = DeleteSession(strSession, strRetCode);
		if (0 != nRet)
		{
			ERROR("Call DeleteSession error. strSession: " << strSession << ", strRetCode: " << strRetCode);
			break;
		}

		json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	} while (false);

	CDBPool::Instance()->PushConnect(m_pConn);

	return nRet;
}

/*
请求字段列表
字段名称	值类型	备注
method	字符串user.search
session	字符串

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
result	{“name”: ”xxx”, “email”: ”xxx”, “userlevel”: xxx,  “grantlevel”: xxx, “granttime”: xxx, 
“lastfund”: xxx, “fundtime”: "xxx"}
*/
int CUser::Search(  // 用户信息查询
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
	int nRet = 0;
	json_t *jItem = NULL;
	string strSession = "";
	unsigned long long ullUID = 0;
	string strName      = "";
	string strEmail     = "";
	int nUserLevel      = 0;
	int nGrantLevel     = 0;
	int nGrantTime      = 0;
	int nLastFund       = 0;
	string strFundTime  = "";
    int nCreateTime     = 0;

	/////////begin: 解析json，得到各字段item//////////////////////
	jItem = json_object_get(jReq, "session");
	if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("Logout json session is not string. strRetCode: " << strRetCode);
		return 1;
	}
	strSession = json_string_value(jItem);
    strSession = trim(strSession);

	if (strSession.empty())
	{
		strRetCode = CErrJsonType;
		ERROR("strSession empty. strSession[" << strSession << "] strRetCode: " << strRetCode);
		return 1;
	}
	/////////end: 解析json，得到各字段item//////////////////////


	m_pConn = CDBPool::Instance()->GetConnect();
	if (NULL == m_pConn)
	{
		strRetCode = CErrDBConn;
		ERROR(" CUser::SendVerifyCode Call CDBPool::Instance()->GetConnect() error. strRetCode: " << strRetCode);
		return 1;
	}

	do
	{
		nRet = GetUIDBySession(strSession, ullUID, strRetCode);
		if (0 != nRet)
		{
			ERROR("Call GetUIDBySession error. strSession: " << strSession << ", strRetCode: " << strRetCode);
			break;
		}

		nRet = GetUserInfo(ullUID, strName, strEmail, nUserLevel, nGrantLevel, nGrantTime, nLastFund, strFundTime, nCreateTime, strRetCode);
		if (0 != nRet)
		{
			ERROR("Call GetUserInfo error. strSession: " << strSession << ", ullUID: " << ullUID << ", strRetCode: " << strRetCode);
			break;
		}
		/*result{ “name”: ”xxx”, “email” : ”xxx”, “userlevel” : xxx, “grantlevel” : xxx, “granttime” : xxx,
		“lastfund” : xxx, “fundtime” : "xxx" }*/
		json_t *jResult = json_object();
		json_object_set_new(jResult, "name", json_string(strName.c_str()));
		json_object_set_new(jResult, "email", json_string(strEmail.c_str()));
		json_object_set_new(jResult, "userlevel", json_integer(nUserLevel));
		json_object_set_new(jResult, "grantlevel", json_integer(nGrantLevel));
		json_object_set_new(jResult, "granttime", json_integer(nGrantTime));
		json_object_set_new(jResult, "lastfund", json_integer(nLastFund));
		json_object_set_new(jResult, "fundtime", json_string(strFundTime.c_str()));
        json_object_set_new(jResult, "createtime", json_integer(nCreateTime));
		json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
		json_object_set_new(jRes, "result", jResult);
	} while (false);

	CDBPool::Instance()->PushConnect(m_pConn);

	return nRet;
}

/*
请求字段列表
字段名称	值类型	备注
method	字符串user.charge
session	字符串	三个字段需要填写2个字段
name	字符串
email	字符串
fund	整数	充值金额

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
result	{“name”: ”xxx”, “fund”: xxx “oldtime”: xxx, “newtime”: xxx}
*/
int CUser::Charge(  // 用户充值，为了安全此接口以后可能会移走
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
	return 1;
}

int CUser::CheckName(
	string strName,
	int &nType,
	string &strRetCode
	)
{
	int nRet = 0;
	string strSQL = "";
	PreparedStatement* stmt = NULL;
	ResultSet* resultSet = NULL;

	nType = 0;
	if ((3 > strName.length()) || (32 < strName.length()))
	{
		nType = 1;
		return nRet;
	}
	for (int i = 0; i < strName.length(); i++)
	{
		if (!((strName[i] >= '0' && strName[i] <= '9')
			|| (strName[i] >= 'A' && strName[i] <= 'Z')
			|| (strName[i] >= 'a' && strName[i] <= 'z')))
		{
			nType = 1;
			return nRet;
		}
	}

	try
	{
		strSQL = "select name from tb_user_info where name=?";
		stmt = m_pConn->prepareStatement(strSQL.c_str());
		stmt->setString(1, strName.c_str());
		resultSet = stmt->executeQuery();
		if (resultSet->next())
		{
			nType = 2;
		}

		delete resultSet;
		delete stmt;
	}
	CATCH_DB;

	return nRet;
}

int CUser::CheckEmail(
	string strEmail,
	int &nType,
	string &strRetCode
	)
{
	int nRet = 0;
	string strSQL = "";
	PreparedStatement* stmt = NULL;
	ResultSet* resultSet = NULL;

	nType = 0;
	if ((3 > strEmail.length()) || (64 < strEmail.length()))
	{
		nType = 1;
		return nRet;
	}
	for (int i = 0; i < strEmail.length(); i++)
	{
		if (!((strEmail[i] >= '0' && strEmail[i] <= '9')
			|| (strEmail[i] >= 'A' && strEmail[i] <= 'Z')
			|| (strEmail[i] >= 'a' && strEmail[i] <= 'z')
			|| (strEmail[i] == '_')
			|| (strEmail[i] == '@')
			|| (strEmail[i] == '.')))
		{
			nType = 1;
			return nRet;
		}
	}

	try
	{
		strSQL = "select email from tb_user_info where email=?";
		stmt = m_pConn->prepareStatement(strSQL.c_str());
		stmt->setString(1, strEmail.c_str());
		resultSet = stmt->executeQuery();
		if (resultSet->next())
		{
			nType = 2;
		}

		delete resultSet;
		delete stmt;
	}
	CATCH_DB;

	return nRet;
}

int CUser::CheckPassword(
	string strPassword,
	int &nType,
	string &strRetCode
	)
{
	int nRet = 0;
	string strSQL = "";
	PreparedStatement* stmt = NULL;
	ResultSet* resultSet = NULL;

	nType = 0;
	if ((6 > strPassword.length()) || (16 < strPassword.length()))
	{
		nType = 1;
		return nRet;
	}
	for (int i = 0; i < strPassword.length(); i++)
	{
		if (!((strPassword[i] >= '0' && strPassword[i] <= '9')
			|| (strPassword[i] >= 'A' && strPassword[i] <= 'Z')
			|| (strPassword[i] >= 'a' && strPassword[i] <= 'z')
			|| (strPassword[i] == '_')))
		{
			nType = 1;
			return nRet;
		}
	}

	/*try
	{
		strSQL = "select password from tb_user_info where password=?";
		stmt = m_pConn->prepareStatement(strSQL.c_str());
		stmt->setString(1, strPassword.c_str());
		resultSet = stmt->executeQuery();
		if (resultSet->next())
		{
			nType = 2;
		}

		delete resultSet;
		delete stmt;
	}
	CATCH_DB;*/

	return nRet;
}

int CUser::GetVerifyCode(
	string &strVerifyCode,
	string &strRetCode
	)
{
	char temp[10] = { 0 };
	int nRandNum = 0;

	srand((int)time(0));
	nRandNum = rand() % 900000 + 100000;
	sprintf(temp, "%d", nRandNum);
	strVerifyCode = temp;

	return 0;
}

int CUser::InsertRegister(
	string strName,
	string strEmail,
	string strPassword,
	string strVerifyCode,
	string &strRetCode
	)
{
	string strSQL = "";
	PreparedStatement* stmt = NULL;
	ResultSet* resultSet = NULL;
	bool bExist = false;

	try
	{
		strSQL = "select email from tb_user_register where email=?";
		stmt = m_pConn->prepareStatement(strSQL.c_str());
		stmt->setString(1, strEmail.c_str());
		resultSet = stmt->executeQuery();
		bExist = false;
		if (resultSet->next())
		{
			bExist = true;
		}
		delete resultSet;
		delete stmt;

		if (true == bExist)
		{
			strSQL = "update tb_user_register set name=?, email=?, password=?, verifycode=?, time=NOW() where email=?";
			stmt = m_pConn->prepareStatement(strSQL.c_str());
			stmt->setString(1, strName.c_str());
			stmt->setString(2, strEmail.c_str());
			stmt->setString(3, strPassword.c_str());
			stmt->setString(4, strVerifyCode.c_str());
			stmt->setString(5, strEmail.c_str());
			stmt->executeUpdate();
		}
		else
		{
			strSQL = "insert into tb_user_register(name, email, password, verifycode, time) values(?,?,?,?,NOW())";
			stmt = m_pConn->prepareStatement(strSQL.c_str());
			stmt->setString(1, strName.c_str());
			stmt->setString(2, strEmail.c_str());
			stmt->setString(3, strPassword.c_str());
			stmt->setString(4, strVerifyCode.c_str());
			stmt->executeUpdate();
		}

		delete stmt;
		stmt = NULL;
	}
	CATCH_DB;

	return 0;
}

int CUser::InsertUserInfo(
	string strName,
	string strEmail,
	string strPassword,
	string strVerifyCode,
	string &strRetCode
	)
{
	string strSQL = "";
	string strDBVerifyCode = "";
	PreparedStatement* stmt = NULL;
	ResultSet* resultSet = NULL;

	try
	{
		strSQL = "select verifycode from tb_user_register where email=?";
		stmt = m_pConn->prepareStatement(strSQL.c_str());
		stmt->setString(1, strEmail.c_str());
		resultSet = stmt->executeQuery();
		if (!resultSet->next())
		{
			delete resultSet;
			delete stmt;
			strRetCode = CErrDBNotExist;
			ERROR("InsertUserInfo register info not exist. strEmail: "<<strEmail<<", strRetCode: "<<strRetCode);
			return 1;
		}
		strDBVerifyCode = resultSet->getString(1).c_str();
		delete resultSet;
		delete stmt;

		if (0 != strDBVerifyCode.compare(strVerifyCode))
		{
			strRetCode = CErrDBNotExist;
			ERROR("InsertUserInfo register info not exist. strEmail: " << strEmail <<", strVerifyCode: "<<strVerifyCode
				<<" != strDBVerifyCode: "<<strDBVerifyCode<< ", strRetCode: " << strRetCode);
			return 1;
		}

		strSQL = "delete from tb_user_register where email=?";
		stmt = m_pConn->prepareStatement(strSQL.c_str());
		stmt->setString(1, strEmail.c_str());
		stmt->executeUpdate();
		delete stmt;
		stmt = NULL;

		strSQL = "insert into tb_user_info(name, email, password, loginTime, createTime, userLevel, \
				 grantLevel, grantTime, totalFund, lastFund, fundtime) \
				 values(?,?,?,NOW(),NOW(),1,1,date_add(curdate(), interval 10 day), 0, 0, NOW())";
		stmt = m_pConn->prepareStatement(strSQL.c_str());
		stmt->setString(1, strName.c_str());
		stmt->setString(2, strEmail.c_str());
		stmt->setString(3, strPassword.c_str());
		stmt->executeUpdate();
		delete stmt;
		stmt = NULL;
	}
	CATCH_DB;

	return 0;
}

int CUser::VerifyUser(
	string strIdentity,
	int nType,
	string strPassword,
	unsigned long long &ullUID,
	string &strName,
	string &strRetCode
	)
{
	string strSQL = "";
	string strDBPassword = "";
	PreparedStatement* stmt = NULL;
	ResultSet* resultSet = NULL;

	try
	{
		if (1 == nType)
		{
			strSQL = "select userid, name, password from tb_user_info where name=?";
		}
		else
		{
			strSQL = "select userid, name, password from tb_user_info where email=?";
		}
		stmt = m_pConn->prepareStatement(strSQL.c_str());
		stmt->setString(1, strIdentity.c_str());
		resultSet = stmt->executeQuery();
		if (!resultSet->next())
		{
			delete resultSet;
			delete stmt;
			strRetCode = CErrDBNotExist;
			ERROR("VerifyUser info not exist. strIdentity: " << strIdentity << ", nType: " << nType << ", strRetCode: " << strRetCode);
			return 1;
		}
		ullUID         = resultSet->getInt64(1);
		strName        = resultSet->getString(2).c_str();
		strDBPassword  = resultSet->getString(3).c_str();

		delete resultSet;
		delete stmt;
	}
	CATCH_DB;

	if (0 != strDBPassword.compare(strPassword))
	{
		strRetCode = CErrDBNotExist;
		ERROR("VerifyUser password error. strIdentity: " << strIdentity << ", nType: " << nType <<", strPassword: "<<strPassword
			<<", strDBPassword: "<<strDBPassword<< ", strRetCode: " << strRetCode);
		return 1;
	}

	return 0;
}

int CUser::CreateSession(
	unsigned long long ullUID,
	string &strSession,
	string &strRetCode
	)
{
    string strSQL = "";
	char temp[64];
	PreparedStatement* stmt = NULL;
	time_t t;
	time(&t);
	sprintf(temp, "%llu%d", ullUID, (int)t);
	strSession = temp;

	try
	{
		strSQL = "delete from tb_user_session where sessionid=? or userid=?";
		stmt = m_pConn->prepareStatement(strSQL.c_str());
		stmt->setString(1, strSession.c_str());
        stmt->setInt64(2, ullUID);
		stmt->executeUpdate();
		delete stmt;

		strSQL = "insert into tb_user_session(sessionid, userid, time) values(?,?,now())";
		stmt = m_pConn->prepareStatement(strSQL.c_str());
		stmt->setString(1, strSession.c_str());
		stmt->setInt64(2, ullUID);
		stmt->executeUpdate();
		delete stmt;
	}
	CATCH_DB;

	return 0;
}

int CUser::DeleteSession(
	string strSession,
	string &strRetCode
	)
{
    string strSQL = "";
	PreparedStatement* stmt = NULL;
	try
	{
		strSQL = "delete from tb_user_session where sessionid=?";
		stmt = m_pConn->prepareStatement(strSQL.c_str());
		stmt->setString(1, strSession.c_str());
		stmt->executeUpdate();
		delete stmt;
	}
	CATCH_DB;

	return 0;
}

int CUser::GetUIDBySession(
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

int CUser::GetUserInfo(
	unsigned long long ullUID,
	string &strName,
	string &strEmail,
	int &nUserLevel,
	int &nGrantLevel,
	int &nGrantTime,
	int &nLastFund,
	string &strFundTime,
	int &nCreateTime,
	string &strRetCode
	)
{
	string strSQL = "";
	PreparedStatement* stmt = NULL;
	ResultSet* resultSet = NULL;

	try
	{
		strSQL = "select name, email, userlevel, grantlevel, DATE_FORMAT(granttime,'%Y%m%d'), \
				 lastfund, fundtime, DATE_FORMAT(createtime,'%Y%m%d') from tb_user_info where userid=?";
		stmt = m_pConn->prepareStatement(strSQL.c_str());
		stmt->setInt64(1, ullUID);
		resultSet = stmt->executeQuery();
		if (!resultSet->next())
		{
			delete resultSet;
			delete stmt;
			strRetCode = CErrDBNotExist;
			ERROR("GetUserInfo info not exist. ullUID: " << ullUID << ", strRetCode: " << strRetCode);
			return 1;
		}
		strName      = resultSet->getString(1).c_str();
		strEmail     = resultSet->getString(2).c_str();
		nUserLevel   = resultSet->getInt(3);
		nGrantLevel  = resultSet->getInt(4);
		nGrantTime   = atoi(resultSet->getString(5).c_str());
		nLastFund    = resultSet->getInt(6);
		strFundTime  = resultSet->getString(7).c_str();
        nCreateTime   = atoi(resultSet->getString(8).c_str());
		delete resultSet;
		delete stmt;
	}
	CATCH_DB;

	return 0;
}