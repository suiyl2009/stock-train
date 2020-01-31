#include "IndexData.h"
#include "StrSplit.h"

// 指数数据
CIndexMap *CIndexMap::m_pInstance = NULL;
CIndexMap::CIndexMap(){}
CIndexMap* CIndexMap::Instance(){
    if (NULL == m_pInstance){
        m_pInstance = new CIndexMap;
    }
    return m_pInstance;
}

void CIndexMap::Remove(){
}

int CIndexMap::Init(){
    string strFile = "";
    m_bInited = false;
	if (0 != CDBData::Instance()->GetIndexData(m_mapIndex))
	{
		ERROR("CStockMap::Init call CDBData::Instance()->GetIndexData(m_mapIndex) failed.");
		return 1;
	}
	DEBUG("CDBData::Instance()->GetIndexData(m_mapIndex)");

    for(map<string, CIndexData*>::iterator iter=m_mapIndex.begin(); iter != m_mapIndex.end(); iter++){
        if ("sh" == iter->second->m_strExchange){
            strFile = cstrSHDir + "/sh" + iter->first + ".day";
        }
        else if ("sz" == iter->second->m_strExchange){
            strFile = cstrSZDir + "/sz" + iter->first + ".day";
        }
        else{
            ERROR("don't support this exchange: "<<iter->second->m_strExchange);
            return 1;
        }

        if (0 != LoadFile(strFile))
		{
			ERROR("Call LoadFile(vecFile[i]) error. strFile: "<<strFile);
			return 1;
		}
    }

    m_bInited = true;
    return 0;
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

int CIndexMap::LoadFile(
    string strFile
    )
{
    // 得到股票代码
    string strCode = "";
    CStrSplit css;
    css.SetString(strFile, "/");
    if (css.Count() <= 0)
    {
        ERROR("css.Count() <= 0. strFile: "<<strFile);
        return 1;
    }
    // sz000008.day
    strCode = css.Item(css.Count()-1);
    if (12 != strCode.length())
    {
        ERROR("12 != strCode.length(). strCode: "<<strCode);
        return 1;
    }
    strCode = strCode.substr(2,6);
    //DEBUG("strCode: "<<strCode);

    // 从文件中得到每天的数据
    vector<DayData> vecDayData;
    vecDayData.clear();
    DayData sDayData;
    FILE *fp; 
    if (!(fp=fopen(strFile.c_str(),"rb")))
    {
        ERROR("Open file failed. strFile: "<<strFile);
        return 1;
    }

    while (1 == fread(&sDayData, 32, 1, fp))
    {
        vecDayData.push_back(sDayData);
    }
    fclose(fp);

    CIndexData* ptrIndexData = NULL;
    map<string, CIndexData*>::iterator iter;
    iter = m_mapIndex.find(strCode);
    if (iter != m_mapIndex.end())
    {
        ptrIndexData = iter->second;
    }
    /*else
    {
        ptrStockData = new StockData;
        ptrStockData->m_strCode = strCode;
        m_mapStock[strCode] = ptrStockData;
    }*/
    if (NULL == ptrIndexData)
    {
        ERROR("NULL == ptrIndexData. strCode: "<<strCode);
        return 0;
    }

    CDayData objDayData;
    ptrIndexData->m_vecDayData.clear();
    for (int i=vecDayData.size(); i>0; i--)
    {
        objDayData.date    = vecDayData[i-1].date;
        objDayData.close   = vecDayData[i-1].close;
        objDayData.high    = vecDayData[i-1].high;
        objDayData.low     = vecDayData[i-1].low;
        objDayData.open    = vecDayData[i-1].open;
        objDayData.remain  = vecDayData[i-1].remain;
        objDayData.amount  = vecDayData[i-1].amount;
        objDayData.vol     = vecDayData[i-1].vol;
        ptrIndexData->m_vecDayData.push_back(objDayData); // 将数据从最近的日期向后排
    }
    ptrIndexData = NULL;

    return 0;
}

