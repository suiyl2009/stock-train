#include "IndexData.h"

// 指数数据
CStockMap *CIndexMap::m_pInstance = NULL;
CIndexMap::CStockMap(){}
CIndexMap* CIndexMap::Instance(){
    if (NULL == m_pInstance){
        m_pInstance = new CIndexMap;
    }
    return m_pInstance;
}

void CIndexMap::Remove(){
}

int CIndexMap::Init(){
}

int CIndexMap::GetIndex(
    string strCode, 
    CIndexData *&ptrIndex
    ){
	ptrIndex = NULL;

	map<string, CIndexData*>::iterator iter;
	iter = m_mapIndex.find(strCode);
	if (m_mapIndex.end() != iter)
	{
		DEBUG("GetIndex success. strCode: "<<strCode);
		ptrIndex = iter->second;
	}
	else
	{
		ERROR("GetIndex failed. strCode: "<<strCode);
		return 1;
	}

	return 0;
}

