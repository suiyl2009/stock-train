#ifndef __CURR_LIST_H__
#define __CURR_LIST_H__

#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include <iostream>
#include <vector>
#include "Data.h"

using namespace std;

class CCurrList
{
public:
	int GetAllCodes(
        int nDate,
        bool bReportDate,
        json_t *&jArrCodes,
        string &strRetCode
        );
    
	/* GetListAll
	请求字段列表
	字段名称	值类型	备注
	method	字符串list.all	
	item	数组[]	需要返回的字段列表
	date	整型或空	开始日期，例如20170121
	
	响应字段列表
	字段名称	值类型	备注
	method	同上	同上
	retcode 字符串	错误码，成功为0000
	result	数组[{},{},{}…] 响应字段值。数组元素为{“Code”: xxx, ”Name”: xxx, …}
	*/
	int GetListAll(
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

	int GetListSub(
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);
	
	int GetListSort(
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

	int GetListShow(
		json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);

private:

    int PushItem(
		StockData *ptrStock,
		int iDate,
		bool bReportDate,
		string &strRetCode
		);

	int ShowResult(
		vector<string> vecItems,
		json_t *&jRes,
		string &strRetCode
		);

	int SortList(
		string strSortItem,
		string strSortType,
		string &strRetCode
		);
	
	vector<StockData*> m_vecStock;
	vector<CListIdx> m_vecIdx;
};

#endif
