#ifndef __INDEX_DATA__H
#define __INDEX_DATA__H

#include "Data.h"

// 指数数据
class CIndexData{
public:
    string m_strCode;
    string m_strName;
    string m_strExchange;
    vector<CDayData> m_vecDayData;    // 价格信息

    int GetDayIdx(int nDate);
};

class CIndexMap{
public:
    static CIndexMap* Instance();
	static void Remove();
    int Init();

    int GetIndex(
        string strCode, 
        CIndexData *&ptrIndex
        );

    map<string, CIndexData*> m_mapIndex;

private:
	CIndexMap();
    
    int LoadFile(
    	string strFile
    	);
    
	static CIndexMap *m_pInstance;
    bool m_bInited;
};
#endif
