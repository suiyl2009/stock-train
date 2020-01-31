#ifndef __INDEX_H__
#define __INDEX_H__

#include <iostream>
#include <jansson.h>
#include "IndexData.h"

using namespace std;

class CIndex{
public:
    int list(
        json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);
    
    int single(
        json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);
    
    int batch(
        json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);
    
    int kline(
        json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);
    
private:
    int InfirmShowIdx(
        CIndexData* ptrIndex, 
        int nDate, 
        int nBeforeCnt, 
        int nAfterCnt, 
        int &nBeforeIdx, 
        int &nAfterIdx, 
        string &strRetCode
        );
};

#endif
