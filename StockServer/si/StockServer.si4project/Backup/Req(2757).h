#ifndef __REQ_H__
#define __REQ_H__

#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>

#include <iostream>
#include <map>

using namespace std;

class CInitPack
{
public:
    static CInitPack* Instance();
	static void Remove();

    int Init();

    int InitInfo(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );
	
private:
	CInitPack();
    string m_strInitInfo;
	
	static CInitPack *m_ptrInstance;
};

class CReq;

typedef int(CReq::*cmdfunc)(json_t *jReq, json_t *&jRes, string &strCode);

class CReq
{
public:
	int Init();           // 初始化接口列表
	
	int Command(           // 入口函数
		string strReq,
		string &strRes
		);

    int InitInfo(
        json_t *jReq,
		json_t *&jRes,
		string &strRetCode
        );

	int OutInfo(
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

	int SubIndustry(
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);
	
	int ListAll(
		json_t *jReq, 
		json_t *&jRes,
		string &strRetCode
		);

	int ListSub(
		json_t *jReq, 
		json_t *&jRes,
		string &strRetCode
		);

	int ListSort(
		json_t *jReq, 
		json_t *&jRes,
		string &strRetCode
		);
		
	int ListShow(
		json_t *jReq, 
		json_t *&jRes,
		string &strRetCode
		);

	int SingleLine(
		json_t *jReq, 
		json_t *&jRes,
		string &strRetCode
		);

	int SingleZoom(
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

	int SingleShow(
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

	int BatchLine(
		json_t *jReq, 
		json_t *&jRes,
		string &strRetCode
		);

	int CondFilter(
		json_t *jReq, 
		json_t *&jRes,
		string &strRetCode
		);

	int CondCompare(
		json_t *jReq, 
		json_t *&jRes,
		string &strRetCode
		);

    int CondSubLeader(
        json_t *jReq, 
        json_t *&jRes,
        string &strRetCode
        );

	int OptTab(
		json_t *jReq, 
		json_t *&jRes,
		string &strRetCode
	    );

	int OptItem(
		json_t *jReq, 
		json_t *&jRes,
		string &strRetCode
		);

	int OptCheck(
		json_t *jReq, 
		json_t *&jRes,
		string &strRetCode
		);

	int OptModify(
		json_t *jReq, 
		json_t *&jRes,
		string &strRetCode
		);

	int KLine(
		json_t *jReq, 
		json_t *&jRes,
		string &strRetCode
		);

    int KTrainBegin(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int KTrainBuy(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int KTrainSale(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );
    
    int KTrainEnd(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );
    
    int KTrainList(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int KTrainSearch(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int PointInsert(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );
    
    int PointDelete(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int PointSearch(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int PointStatistics(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int UserCheckitem(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int UserSendverifycode(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int UserRegister(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int UserLogin(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );
    
    int UserLogout(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );
    
    int UserSearch(
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

    int SelectedGetTab(
		json_t *jReq, 
		json_t *&jRes,
		string &strRetCode
	    );

	int SelectedGetItem(
		json_t *jReq, 
		json_t *&jRes,
		string &strRetCode
		);

    int SelectedAddItem(
        json_t *jReq, 
        json_t *&jRes,
        string &strRetCode
        );

	map<string, cmdfunc> m_mapMethod;
};

#endif
