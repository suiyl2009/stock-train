#ifndef __USER_H__
#define __USER_H__

#include <iostream>
#include <jansson.h>
#include "DBPool.h"

using namespace std;

class CUser
{
public:
    int SendVerifyCode(        // 发送验证码
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

    int CheckItem(        // 验证字段
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);
        
	int Register(        // 用户注册
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

	int Login(    // 用户登录
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

	int Logout(     // 用户退出
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

	int Search(    // 用户信息查询
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

	int Charge(     // 用户充值，为了安全此接口以后可能会移走
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

private:
	int CheckName(
		string strName,
		int &nType,
		string &strRetCode
		);

	int CheckEmail(
		string strEmail,
		int &nType,
		string &strRetCode
		);

	int CheckPassword(
		string strPassword,
		int &nType,
		string &strRetCode
		);

	int GetVerifyCode(
		string &strVerifyCode,
		string &strRetCode
		);

	int InsertRegister(
		string strName,
		string strEmail,
		string strPassword,
		string strVerifyCode,
		string &strRetCode
		);

	int InsertUserInfo(
		string strName,
		string strEmail,
		string strPassword,
		string strVerifyCode,
		string &strRetCode
		);

	int VerifyUser(
		string strIdentity,
		int nType,
		string strPassword,
		unsigned long long &ullUID,
		string &strName,
		string &strRetCode
		);

	int CreateSession(
		unsigned long long ullUID,
		string &strSession,
		string &strRetCode
		);

	int DeleteSession(
		string strSession,
		string &strRetCode
		);

	int GetUIDBySession(
		string strSession,
		unsigned long long &ullUID,
		string &strRetCode
		);

	int GetUserInfo(
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
		);

	Connection *m_pConn;
};

#endif