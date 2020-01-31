#ifndef __OPTION_LIST_H__
#define __OPTION_LIST_H__

#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include <iostream>
#include <vector>
#include "Data.h"

using namespace std;

class COptionList
{
public:
    int GetTabs( // 有哪些类型
        json_t *jReq,
		json_t *&jRes,
		string &strRetCode
        );

    int GetItems( // 得到自选列表
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );
	
    int CheckItems( // 传入列表，判断哪些符合
        json_t *jReq,
        json_t *&jRes,
        string &strRetCode
        );

	int ModifyItems(  // 修改列表
	    json_t *jReq,
	    json_t *&jRes,
	    string &strRetCode
		);

private:
	int InsertItem(
		COptionInfo* ptrItem, 
		int iDate, 
		string &strRetCode
		);

	int ShowResult(
		vector<string> vecItems,
		json_t *&jRes,
		string &strRetCode
		);
	
	vector<COptionInfo* > m_vecOptInfo;
};

#endif