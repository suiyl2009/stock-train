#ifndef __OUT_INFO_H__
#define __OUT_INFO_H__

#include <iostream>
#include <map>
#include "Data.h"

using namespace std;

class CSingleLine;
class CBatchLine;
class CCondValue;
class CKLine;

typedef string (StockData::*itemFunc)(CListIdx objListIdx, eItemType &eType, string &strRetCode);

typedef int (CSingleLine::*singleLineFunc)(int &iMax, int &iMin, string &strRetCode);

typedef int (CBatchLine::*batchLineFunc)(string &strRetCode);

typedef CCondValue* (*getCondFunc)();

typedef int (CKLine::*klineFunc)(string &strRetCode);

class COutInfo
{
public:
	static COutInfo* Instance();
	static void Remove();

    map<string, itemFunc> m_mapItem;
	map<string, singleLineFunc> m_mapSLine;
	map<string, batchLineFunc> m_mapBLine;
	map<string, getCondFunc> m_mapCond;
	map<string, klineFunc> m_mapKLine;
	
private:
	COutInfo();
	int Init();
	static COutInfo *m_ptrInstance;
};

#endif