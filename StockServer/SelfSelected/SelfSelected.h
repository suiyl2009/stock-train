#ifndef __SELF_SELECTED_H__
#define __SELF_SELECTED_H__

#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include <iostream>
#include <vector>
#include "Data.h"
#include "DBPool.h"

using namespace std;

class CSelfSelected{
public:
    int GetTab( // 有哪些自选类型
        json_t *jReq,
		json_t *&jRes,
		string &strRetCode
        );

    int GetItem( // 得到自选列表
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int AddItem( // 添加自选股
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

private:
    int GetUIDBySession(
        string strSession,
        unsigned long long &ullUID,
        string &strRetCode
        );

    int SearchTab(
        unsigned long long ullUserId, 
        vector<string> &vecTab, 
        string &strRetCode
        );

    int SearchItem(
        unsigned long long ullUserId, 
        const string &strTab, 
        vector<string> &vecCode, 
        string &strRetCode
        );
    
    int UpdateItem(
        unsigned long long ullUserId, 
        const string &strTab, 
        bool bAdd, 
        vector<string> &vecCode, 
        string &strRetCode
        );
        
    int ShowTab(
        const vector<string> &vecTab, 
        json_t *&jRes, 
        string &strRetCode
        );

    int ShowItem(
        const vector<string> &vecCode, 
        json_t *&jRes, 
        string &strRetCode
        );
    
    Connection *m_pConn;
};

#endif

