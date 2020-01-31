#include "Constant.h"
#include "Req.h"
#include "Log.h"
#include "OutInfo.h"
#include "CurrList.h"
#include "SingleLine.h"
#include "BatchLine.h"
#include "StrSplit.h"
#include "convert.h"
#include "CondList.h"
#include "OptionList.h"
#include "KLine.h"
#include "KTrain.h"
#include "TradePoint.h"
#include "User.h"
#include "SelfSelected.h"

CInitPack *CInitPack::m_ptrInstance = NULL;

CInitPack::CInitPack()
{
    m_strInitInfo = "";
}

CInitPack* CInitPack::Instance()
{
    if (NULL == m_ptrInstance)
    {
        m_ptrInstance = new CInitPack;
    }
    return m_ptrInstance;
}

void CInitPack::Remove()
{
    if (NULL != m_ptrInstance)
    {
        delete m_ptrInstance;
        m_ptrInstance = NULL;
    }
}

int CInitPack::Init()
{
    vector<CIndustryItem> *ptrVecSubIndustry = NULL;
    if (0 != CStockMap::Instance()->GetSubIndustry(ptrVecSubIndustry))
	{
		ERROR("Call CStockMap::Instance()->GetSubIndustry(ptrVecSubIndustry) error.");
		return 1;
	}

    json_t *jResult = json_object();

    ///////////begin: subindustry///////////////
	json_t *jSubIndustry = json_array();
	for (int i=0; i<(*ptrVecSubIndustry).size(); i++)
	{
	    json_t *jItem = json_object();
		json_object_set_new(jItem, "name", json_string((*ptrVecSubIndustry)[i].m_strIndustry.c_str()));
		json_object_set_new(jItem, "count", json_integer((*ptrVecSubIndustry)[i].m_nCount));
		json_array_append_new(jSubIndustry, jItem);
	}
    json_object_set_new(jResult, "subindustry", jSubIndustry);
    ///////////end: subindustry///////////////

    ////////////begin: outinfo/////////////////////
    // list item
	json_t *jItems = json_array();
	map<string, itemFunc>::iterator iter1;
	for (iter1 = COutInfo::Instance()->m_mapItem.begin(); iter1 != COutInfo::Instance()->m_mapItem.end(); iter1++)
	{
		json_array_append_new(jItems, json_string(iter1->first.c_str()));
	}
	json_object_set_new(jResult, "item", jItems);

    // single line
    json_t *jSLine = json_array();
	map<string, singleLineFunc>::iterator iter2;
	for (iter2 = COutInfo::Instance()->m_mapSLine.begin(); iter2 != COutInfo::Instance()->m_mapSLine.end(); iter2++)
	{
	    json_array_append_new(jSLine, json_string(iter2->first.c_str()));
	}
	json_object_set_new(jResult, "single", jSLine);

	// batch line
    json_t *jBLine = json_array();
	map<string, batchLineFunc>::iterator iter3;
	for (iter3 = COutInfo::Instance()->m_mapBLine.begin(); iter3 != COutInfo::Instance()->m_mapBLine.end(); iter3++)
	{
	    json_array_append_new(jBLine, json_string(iter3->first.c_str()));
	}
	json_object_set_new(jResult, "batch", jBLine);

    // cond
    json_t *jCond = json_array();
    map<string, getCondFunc>::iterator iter4;
    for (iter4 = COutInfo::Instance()->m_mapCond.begin(); iter4 != COutInfo::Instance()->m_mapCond.end(); iter4++)
    {
        json_array_append_new(jCond, json_string(iter4->first.c_str()));
    }
    json_object_set_new(jResult, "cond", jCond);
    ////////////end: outinfo/////////////////////

	char *reply_str = json_dumps(jResult, 0);
	m_strInitInfo = reply_str;
	free(reply_str);
	json_decref(jResult);

    return 0;
}

int CInitPack::InitInfo(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int nRet           = 0;
	int nDate          = 0;
	string strTemp     = "";
	bool bReportDate   = false;
    
    json_t *jDate = json_object_get(jReq, "date");
    if (jDate && json_is_integer(jDate)) 
	{
		nDate = json_integer_value(json_object_get(jReq, "date"));
		if (((0 != nDate) && (nDate < 19800000)) || (nDate > 99999999))
		{
	        strRetCode = CErrJsonType;
			ERROR("GetListAll Get json date value error. strRetCode: "<<strRetCode<<", nDate: "<<nDate);
			return 1;
		}
    }

	json_t *jDateType = json_object_get(jReq, "datetype");
    if (jDateType && json_is_string(jDateType)) {
		strTemp = json_string_value(jDateType);
		if (strTemp == "report")
		{
		    bReportDate = true;
		}
    }

    if (m_strInitInfo.empty()){
        strRetCode = CErrJsonLoad;
        ERROR("CInitPack::InitInfo json_loadb error. m_strInitInfo: "<<m_strInitInfo<<", strRetCode: "<<strRetCode);
        return 1;
    }
    
    json_t *jResult = json_loadb(m_strInitInfo.c_str(), m_strInitInfo.length(), 0, NULL);
    if (jResult == NULL) {
        strRetCode = CErrJsonLoad;
        ERROR("CInitPack::InitInfo json_loadb error. m_strInitInfo: "<<m_strInitInfo<<", strRetCode: "<<strRetCode);
        return 1;
    }

    json_t *jArrCodes = json_array();
    CCurrList *ptrCurrList = new CCurrList;
	nRet = ptrCurrList->GetAllCodes(nDate, bReportDate, jArrCodes, strRetCode);
	if (0 != nRet)
	{
	    ERROR("Call ptrCurrList->GetListAll. strRetCode: "<<strRetCode);
		delete ptrCurrList;
		ptrCurrList = NULL;
        json_decref(jResult);
        json_decref(jArrCodes);
		return nRet;
	}
    json_object_set_new(jResult, "code", jArrCodes);
	delete ptrCurrList;
	ptrCurrList = NULL;

    json_object_set_new(jRes, "method", json_string("init.info"));
	json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	json_object_set_new(jRes, "result", jResult);

    return 0;
}


int CReq::Init()          // 初始化接口列表
{
    m_mapMethod["init.info"]         = &CReq::InitInfo;
    m_mapMethod["outinfo.all"]       = &CReq::OutInfo;
	m_mapMethod["industry.sub"]      = &CReq::SubIndustry;

	
    m_mapMethod["list.all"]          = &CReq::ListAll;
	m_mapMethod["list.sub"]          = &CReq::ListSub;
	m_mapMethod["list.sort"]         = &CReq::ListSort;
	m_mapMethod["list.show"]         = &CReq::ListShow;
	
	m_mapMethod["single.line"]       = &CReq::SingleLine;
	m_mapMethod["single.zoom"]       = &CReq::SingleZoom;
	m_mapMethod["single.show"]       = &CReq::SingleShow;
		
	m_mapMethod["batch.line"]        = &CReq::BatchLine;
	
	m_mapMethod["cond.filter"]       = &CReq::CondFilter;
	m_mapMethod["cond.compare"]      = &CReq::CondCompare;
	m_mapMethod["cond.leader"]       = &CReq::CondSubLeader;

	m_mapMethod["opt.tab"]           = &CReq::OptTab;
	m_mapMethod["opt.item"]          = &CReq::OptItem;
	m_mapMethod["opt.check"]         = &CReq::OptCheck;
	m_mapMethod["opt.modify"]        = &CReq::OptModify;

	m_mapMethod["kline.line"]        = &CReq::KLine;

    m_mapMethod["ktrain.begin"]      = &CReq::KTrainBegin;
    m_mapMethod["ktrain.buy"]        = &CReq::KTrainBuy;
    m_mapMethod["ktrain.sale"]       = &CReq::KTrainSale;
    m_mapMethod["ktrain.end"]        = &CReq::KTrainEnd;
    m_mapMethod["ktrain.list"]       = &CReq::KTrainList;
    m_mapMethod["ktrain.search"]     = &CReq::KTrainSearch;

    m_mapMethod["point.insert"]      = &CReq::PointInsert;
    m_mapMethod["point.delete"]      = &CReq::PointDelete;
    m_mapMethod["point.search"]      = &CReq::PointSearch;
    m_mapMethod["point.statistics"]  = &CReq::PointStatistics;
    m_mapMethod["point.enddate"]     = &CReq::PointEndDate;
    m_mapMethod["point.prcond"]      = &CReq::PointPrCond;

    m_mapMethod["user.checkitem"]    = &CReq::UserCheckitem;
    m_mapMethod["user.verifycode"]   = &CReq::UserSendverifycode;
    m_mapMethod["user.register"]     = &CReq::UserRegister;
    m_mapMethod["user.login"]        = &CReq::UserLogin;
    m_mapMethod["user.logout"]       = &CReq::UserLogout;
    m_mapMethod["user.search"]       = &CReq::UserSearch;

    m_mapMethod["selected.gettab"]   = &CReq::SelectedGetTab;
    m_mapMethod["selected.getitem"]  = &CReq::SelectedGetItem;
    m_mapMethod["selected.additem"]  = &CReq::SelectedAddItem;
  
    return 0;
}

int CReq::Command(         // 入口函数
	string strReq,
	string &strRes
	)
{
    int iRet           = 0;
    json_t *jReq       = NULL;
	json_t *jRes       = NULL;
	string strRetCode  = CErrSuccess;
	string strCmd      = "";

    /*for (int i=(int)strReq.length()-1; i>0; i--) // 比较奇怪的是末尾有乱码，消除乱码
	{
	    if (strReq[i] == '}')
    	{
    	    break;
    	}
		strReq[i] = 0;
	}
	*/
	strReq = trim(strReq);
	//string strTemp = strReq;
    INFO("strReq: "<<strReq);
	cout<<"strReq: "<<strReq<<endl;
	
    do
	{
	    json_t *jReq = json_loadb(strReq.c_str(), strReq.length(), 0, NULL);
	    if (jReq == NULL) {
	        iRet = 1;
			strRetCode = CErrJsonLoad;
		    ERROR("CReq::Command json_loadb error. strReq: "<<strReq<<", strRetCode: "<<strRetCode);
			break;
	    }

	    json_t *method = json_object_get(jReq, "method");
	    if (!method || !json_is_string(method)) {
	        iRet = 1;
			strRetCode = CErrJsonType;
		    ERROR("CReq::Command json method is not string. strReq: "<<strReq<<", strRetCode: "<<strRetCode);
			break;
	    }
		
		strCmd = json_string_value(method);

		if (1 != m_mapMethod.count(strCmd))
		{
		    iRet = 1;
			strRetCode = CErrMethodNone;
			ERROR("CReq::Command method is not supported. strReq: "<<strReq<<", strCmd: "<<strCmd
				<<", strRetCode: "<<strRetCode);
		    break;
		}

		jRes = json_object();
	    iRet = (this->*m_mapMethod[strCmd])(jReq, jRes, strRetCode);
		if (0 != iRet)
		{
		    iRet = 1;
			ERROR("Call m_mapMethod failed. strRetCode: "<<strRetCode<<", strReq: "<<strReq);
			break;
		}

	}while (false);

	if (0 != iRet)
	{
	    if (NULL == jRes)
	    {
	        jRes = json_object();
	    }
		json_object_set_new(jRes, "method", json_string(strCmd.c_str()));
	    json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	}

    if (NULL != jRes)
    {
		char *reply_str = json_dumps(jRes, 0);
		strRes = reply_str;
		free(reply_str);
		json_decref(jRes);
    }
	if (NULL != jReq)
	{
	    json_decref(jReq);
	}
	INFO(strRes);
	
    return iRet;
}

int CReq::InitInfo(
    json_t *jReq,
	json_t *&jRes,
	string &strRetCode
    )
{
    int iRet = 0;

    iRet = CInitPack::Instance()->InitInfo(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call CInitPack::Instance()->InitInfo. strRetCode: "<<strRetCode);
        return iRet;
    }
    
    return iRet;
}   

int CReq::OutInfo(
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
	json_t *jResult = json_object();

    // 列表条目
	json_t *jItems = json_array();
	map<string, itemFunc>::iterator iter1;
	for (iter1 = COutInfo::Instance()->m_mapItem.begin(); iter1 != COutInfo::Instance()->m_mapItem.end(); iter1++)
	{
		json_array_append_new(jItems, json_string(iter1->first.c_str()));
	}
	json_object_set_new(jResult, "item", jItems);

    // single line
    json_t *jSLine = json_array();
	map<string, singleLineFunc>::iterator iter2;
	for (iter2 = COutInfo::Instance()->m_mapSLine.begin(); iter2 != COutInfo::Instance()->m_mapSLine.end(); iter2++)
	{
	    json_array_append_new(jSLine, json_string(iter2->first.c_str()));
	}
	json_object_set_new(jResult, "single", jSLine);

	// batch line
    json_t *jBLine = json_array();
	map<string, batchLineFunc>::iterator iter3;
	for (iter3 = COutInfo::Instance()->m_mapBLine.begin(); iter3 != COutInfo::Instance()->m_mapBLine.end(); iter3++)
	{
	    json_array_append_new(jBLine, json_string(iter3->first.c_str()));
	}
	json_object_set_new(jResult, "batch", jBLine);

	// cond
	json_t *jCond = json_array();
	map<string, getCondFunc>::iterator iter4;
	for (iter4 = COutInfo::Instance()->m_mapCond.begin(); iter4 != COutInfo::Instance()->m_mapCond.end(); iter4++)
	{
	    json_array_append_new(jCond, json_string(iter4->first.c_str()));
	}
	json_object_set_new(jResult, "cond", jCond);

	json_object_set_new(jRes, "method", json_string("outinfo.all"));
	json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	json_object_set_new(jRes, "result", jResult);

    return 0;
}

int CReq::SubIndustry(
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
    vector<CIndustryItem> *ptrVecSubIndustry = NULL;
    if (0 != CStockMap::Instance()->GetSubIndustry(ptrVecSubIndustry))
	{
	    strRetCode = CErrItemNone;
		ERROR("Call CStockMap::Instance()->GetSubIndustry(ptrVecSubIndustry) error. strRetCode: "<<strRetCode);
		return 1;
	}

    // 细分行业条目
	json_t *jResult = json_array();
	for (int i=0; i<(*ptrVecSubIndustry).size(); i++)
	{
	    json_t *jItem = json_object();
		json_object_set_new(jItem, "name", json_string((*ptrVecSubIndustry)[i].m_strIndustry.c_str()));
		json_object_set_new(jItem, "count", json_integer((*ptrVecSubIndustry)[i].m_nCount));
		json_array_append_new(jResult, jItem);
	}

	json_object_set_new(jRes, "method", json_string("industry.sub"));
	json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	json_object_set_new(jRes, "result", jResult);

    return 0;
}

int CReq::ListAll(
    json_t *jReq, 
    json_t *&jRes, 
    string &strRetCode
    )
{
    int iRet = 0;
	
    CCurrList *ptrCurrList = new CCurrList;
	iRet = ptrCurrList->GetListAll(jReq, jRes, strRetCode);
	if (0 != iRet)
	{
	    ERROR("Call ptrCurrList->GetListAll. strRetCode: "<<strRetCode);
		delete ptrCurrList;
		ptrCurrList = NULL;
		return iRet;
	}
	delete ptrCurrList;
	ptrCurrList = NULL;
	
    return iRet;
}

int CReq::ListSub(
	json_t *jReq, 
	json_t *&jRes,
	string &strRetCode
	)
{
	int iRet = 0;
	
	CCurrList *ptrCurrList = new CCurrList;
	iRet = ptrCurrList->GetListSub(jReq, jRes, strRetCode);
	if (0 != iRet)
	{
		ERROR("Call ptrCurrList->GetListSub. strRetCode: "<<strRetCode);
		delete ptrCurrList;
		ptrCurrList = NULL;
		return iRet;
	}
	delete ptrCurrList;
	ptrCurrList = NULL;
	
	return iRet;
}

int CReq::ListSort(
	json_t *jReq, 
	json_t *&jRes,
	string &strRetCode
	)
{
	int iRet = 0;
	
	CCurrList *ptrCurrList = new CCurrList;
	iRet = ptrCurrList->GetListSort(jReq, jRes, strRetCode);
	if (0 != iRet)
	{
		ERROR("Call ptrCurrList->GetListSort. strRetCode: "<<strRetCode);
		delete ptrCurrList;
		ptrCurrList = NULL;
		return iRet;
	}
	delete ptrCurrList;
	ptrCurrList = NULL;
	
	return iRet;
}
	
int CReq::ListShow(
	json_t *jReq, 
	json_t *&jRes,
	string &strRetCode
	)
{
	int iRet = 0;
	
	CCurrList *ptrCurrList = new CCurrList;
	iRet = ptrCurrList->GetListShow(jReq, jRes, strRetCode);
	if (0 != iRet)
	{
		ERROR("Call ptrCurrList->GetListShow. strRetCode: "<<strRetCode);
		delete ptrCurrList;
		ptrCurrList = NULL;
		return iRet;
	}
	delete ptrCurrList;
	ptrCurrList = NULL;
	
	return iRet;
}

int CReq::SingleLine(
	json_t *jReq, 
	json_t *&jRes,
	string &strRetCode
	)
{
	int iRet = 0;
	
	CSingleLine *ptrSingleLine = new CSingleLine;
	iRet = ptrSingleLine->GetLines(jReq, jRes, strRetCode);
	if (0 != iRet)
	{
		ERROR("Call ptrSingleLine->GetLines. strRetCode: "<<strRetCode);
		delete ptrSingleLine;
		ptrSingleLine = NULL;
		return iRet;
	}
	delete ptrSingleLine;
	ptrSingleLine = NULL;
	
	return iRet;
}

int CReq::SingleZoom(
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
	int iRet = 0;
	
	CSingleLine *ptrSingleLine = new CSingleLine;
	//iRet = ptrSingleLine->ZoomLines(jReq, jRes, strRetCode);
	if (0 != iRet)
	{
		ERROR("Call ptrSingleLine->ZoomLines. strRetCode: "<<strRetCode);
		delete ptrSingleLine;
		ptrSingleLine = NULL;
		return iRet;
	}
	delete ptrSingleLine;
	ptrSingleLine = NULL;
	
	return iRet;
}

int CReq::SingleShow(
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
	int iRet = 0;
	
	CSingleLine *ptrSingleLine = new CSingleLine;
	//iRet = ptrSingleLine->GetShowInfo(jReq, jRes, strRetCode);
	if (0 != iRet)
	{
		ERROR("Call ptrSingleLine->GetShowInfo. strRetCode: "<<strRetCode);
		delete ptrSingleLine;
		ptrSingleLine = NULL;
		return iRet;
	}
	delete ptrSingleLine;
	ptrSingleLine = NULL;
	
	return iRet;
}

int CReq::BatchLine(
	json_t *jReq, 
	json_t *&jRes,
	string &strRetCode
	)
{
	int iRet = 0;
	
	CBatchLine *ptrBatchLine = new CBatchLine;
	iRet = ptrBatchLine->GetLines(jReq, jRes, strRetCode);
	if (0 != iRet)
	{
		ERROR("Call ptrBatchLine->GetLines. strRetCode: "<<strRetCode);
		delete ptrBatchLine;
		ptrBatchLine = NULL;
		return iRet;
	}
	delete ptrBatchLine;
	ptrBatchLine = NULL;
	
	return iRet;
}

int CReq::CondFilter(
	json_t *jReq, 
	json_t *&jRes,
	string &strRetCode
	)
{
    int iRet = 0;

	CCondList *ptrCondList = new CCondList;
	iRet = ptrCondList->Filter(jReq, jRes, strRetCode);
	if (0 != iRet)
	{
	    ERROR("Call ptrCondList->Filter failed. strRetCode: "<<strRetCode);
	}
	delete ptrCondList;
	ptrCondList = NULL;
	
    return iRet;
}

int CReq::CondCompare(
	json_t *jReq, 
	json_t *&jRes,
	string &strRetCode
	)
{
	int iRet = 0;
	
	CCondList *ptrCondList = new CCondList;
	iRet = ptrCondList->Compare(jReq, jRes, strRetCode);
	if (0 != iRet)
	{
		ERROR("Call ptrCondList->Compare failed. strRetCode: "<<strRetCode);
	}
	delete ptrCondList;
	ptrCondList = NULL;
	
	return iRet;
}

int CReq::CondSubLeader(
    json_t *jReq, 
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    
    CCondList *ptrCondList = new CCondList;
    iRet = ptrCondList->SubLeader(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call ptrCondList->IndustryLeader failed. strRetCode: "<<strRetCode);
    }
    delete ptrCondList;
    ptrCondList = NULL;
    
    return iRet;
}

int CReq::OptTab(
	json_t *jReq, 
	json_t *&jRes,
	string &strRetCode
    )
{
	int iRet = 0;
	
	COptionList *ptrOptionList = new COptionList;
	iRet = ptrOptionList->GetTabs(jReq, jRes, strRetCode);
	if (0 != iRet)
	{
		ERROR("Call ptrOptionList->GetTabs failed. strRetCode: "<<strRetCode);
	}
	delete ptrOptionList;
	ptrOptionList = NULL;
	
	return iRet;
}

int CReq::OptItem(
	json_t *jReq, 
	json_t *&jRes,
	string &strRetCode
	)
{
	int iRet = 0;
	
	COptionList *ptrOptionList = new COptionList;
	iRet = ptrOptionList->GetItems(jReq, jRes, strRetCode);
	if (0 != iRet)
	{
		ERROR("Call ptrOptionList->GetItems failed. strRetCode: "<<strRetCode);
	}
	delete ptrOptionList;
	ptrOptionList = NULL;
	
	return iRet;
}

int CReq::OptCheck(
	json_t *jReq, 
	json_t *&jRes,
	string &strRetCode
	)
{
	int iRet = 0;
	
	COptionList *ptrOptionList = new COptionList;
	iRet = ptrOptionList->CheckItems(jReq, jRes, strRetCode);
	if (0 != iRet)
	{
		ERROR("Call ptrOptionList->CheckItems failed. strRetCode: "<<strRetCode);
	}
	delete ptrOptionList;
	ptrOptionList = NULL;
	
	return iRet;
}
	
int CReq::OptModify(
	json_t *jReq, 
	json_t *&jRes,
	string &strRetCode
	)
{
	int iRet = 0;
	
	COptionList *ptrOptionList = new COptionList;
	iRet = ptrOptionList->ModifyItems(jReq, jRes, strRetCode);
	if (0 != iRet)
	{
		ERROR("Call ptrOptionList->ModifyItems failed. strRetCode: "<<strRetCode);
	}
	delete ptrOptionList;
	ptrOptionList = NULL;
	
	return iRet;
}

int CReq::KLine(
	json_t *jReq, 
	json_t *&jRes,
	string &strRetCode
	)
{
	int iRet = 0;
	CKLine *ptrKLine = new CKLine;
	iRet = ptrKLine->GetLines(jReq, jRes, strRetCode);
	if (0 != iRet)
	{
		ERROR("Call ptrKLine->GetLines failed. strRetCode: "<<strRetCode);
	}
	delete ptrKLine;
	ptrKLine = NULL;
	
	return iRet;
}

int CReq::KTrainBegin(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CKTrain *pKTrain = new CKTrain;
    iRet = pKTrain->Begin(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pKTrain->Begin failed. strRetCode: "<<strRetCode);
    }
    delete pKTrain;
    pKTrain = NULL;
    
    return iRet;
}

int CReq::KTrainBuy(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CKTrain *pKTrain = new CKTrain;
    iRet = pKTrain->Buy(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pKTrain->Buy failed. strRetCode: "<<strRetCode);
    }
    delete pKTrain;
    pKTrain = NULL;
    
    return iRet;
}

int CReq::KTrainSale(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CKTrain *pKTrain = new CKTrain;
    iRet = pKTrain->Sale(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pKTrain->Sale failed. strRetCode: "<<strRetCode);
    }
    delete pKTrain;
    pKTrain = NULL;
    
    return iRet;
}

int CReq::KTrainEnd(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CKTrain *pKTrain = new CKTrain;
    iRet = pKTrain->End(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pKTrain->End failed. strRetCode: "<<strRetCode);
    }
    delete pKTrain;
    pKTrain = NULL;
    
    return iRet;
}

int CReq::KTrainList(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CKTrain *pKTrain = new CKTrain;
    iRet = pKTrain->List(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pKTrain->List failed. strRetCode: "<<strRetCode);
    }
    delete pKTrain;
    pKTrain = NULL;
    
    return iRet;
}

int CReq::KTrainSearch(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CKTrain *pKTrain = new CKTrain;
    iRet = pKTrain->Search(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pKTrain->Search failed. strRetCode: "<<strRetCode);
    }
    delete pKTrain;
    pKTrain = NULL;
    
    return iRet;
}

int CReq::PointInsert(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CTradePoint *pTradePoint = new CTradePoint;
    iRet = pTradePoint->Insert(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pKTrain->Search failed. strRetCode: "<<strRetCode);
    }
    delete pTradePoint;
    pTradePoint = NULL;
    
    return iRet;
}

int CReq::PointDelete(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CTradePoint *pTradePoint = new CTradePoint;
    iRet = pTradePoint->Delete(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pKTrain->Search failed. strRetCode: "<<strRetCode);
    }
    delete pTradePoint;
    pTradePoint = NULL;
    
    return iRet;
}

int CReq::PointSearch(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CTradePoint *pTradePoint = new CTradePoint;
    iRet = pTradePoint->Search(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pKTrain->Search failed. strRetCode: "<<strRetCode);
    }
    delete pTradePoint;
    pTradePoint = NULL;
    
    return iRet;
}

int CReq::PointStatistics(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CTradePoint *pTradePoint = new CTradePoint;
    iRet = pTradePoint->Statistics(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pKTrain->Search failed. strRetCode: "<<strRetCode);
    }
    delete pTradePoint;
    pTradePoint = NULL;
    
    return iRet;
}

int CReq::PointEndDate(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CTradePoint *pTradePoint = new CTradePoint;
    iRet = pTradePoint->EndDate(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pTradePoint->EndDate failed. strRetCode: "<<strRetCode);
    }
    delete pTradePoint;
    pTradePoint = NULL;
    
    return iRet;
}

int CReq::PointPrCond(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CTradePoint *pTradePoint = new CTradePoint;
    iRet = pTradePoint->PrCond(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pTradePoint->PrCond failed. strRetCode: "<<strRetCode);
    }
    delete pTradePoint;
    pTradePoint = NULL;
    
    return iRet;
}

int CReq::UserCheckitem(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CUser *pUser = new CUser;
    iRet = pUser->CheckItem(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pUser->CheckItem failed. strRetCode: "<<strRetCode);
    }
    delete pUser;
    pUser = NULL;
    
    return iRet;
}

int CReq::UserSendverifycode(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CUser *pUser = new CUser;
    iRet = pUser->SendVerifyCode(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pUser->SendVerifyCode failed. strRetCode: "<<strRetCode);
    }
    delete pUser;
    pUser = NULL;
    
    return iRet;
}

int CReq::UserRegister(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CUser *pUser = new CUser;
    iRet = pUser->Register(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pUser->Register failed. strRetCode: "<<strRetCode);
    }
    delete pUser;
    pUser = NULL;
    
    return iRet;
}

int CReq::UserLogin(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CUser *pUser = new CUser;
    iRet = pUser->Login(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pUser->Login failed. strRetCode: "<<strRetCode);
    }
    delete pUser;
    pUser = NULL;
    
    return iRet;
}

int CReq::UserLogout(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CUser *pUser = new CUser;
    iRet = pUser->Logout(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pUser->Logout failed. strRetCode: "<<strRetCode);
    }
    delete pUser;
    pUser = NULL;
    
    return iRet;
}

int CReq::UserSearch(
    json_t *jReq,
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    CUser *pUser = new CUser;
    iRet = pUser->Search(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call pUser->Search failed. strRetCode: "<<strRetCode);
    }
    delete pUser;
    pUser = NULL;
    
    return iRet;
}

int CReq::SelectedGetTab(
	json_t *jReq, 
	json_t *&jRes,
	string &strRetCode
    )
{
    int iRet = 0;
    
    CSelfSelected *ptrSelected = new CSelfSelected;
    iRet = ptrSelected->GetTab(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call ptrSelected->GetTab failed. strRetCode: "<<strRetCode);
    }
    delete ptrSelected;
    ptrSelected = NULL;
    
    return iRet;
}

int CReq::SelectedGetItem(
	json_t *jReq, 
	json_t *&jRes,
	string &strRetCode
	)
{
    int iRet = 0;

    CSelfSelected *ptrSelected = new CSelfSelected;
    iRet = ptrSelected->GetItem(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call ptrSelected->GetItem failed. strRetCode: "<<strRetCode);
    }
    delete ptrSelected;
    ptrSelected = NULL;
    
    return iRet;
}

int CReq::SelectedAddItem(
    json_t *jReq, 
    json_t *&jRes,
    string &strRetCode
    )
{
    int iRet = 0;
    
    CSelfSelected *ptrSelected = new CSelfSelected;
    iRet = ptrSelected->AddItem(jReq, jRes, strRetCode);
    if (0 != iRet)
    {
        ERROR("Call ptrSelected->AddItem failed. strRetCode: "<<strRetCode);
    }
    delete ptrSelected;
    ptrSelected = NULL;
    
    return iRet;
}

