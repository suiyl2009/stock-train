#include <iostream>
#include <string>
#include <algorithm>

#include "CurrList.h"
#include "Constant.h"
#include "Log.h"
#include "OutInfo.h"

using namespace std;

int CCurrList::GetAllCodes(
    int nDate,
    bool bReportDate,
    json_t *&jArrCodes,
    string &strRetCode
    )
{
	map<string, StockData*>::iterator iter;
	StockData *ptrStock = NULL;

    for (iter=CStockMap::Instance()->m_mapStock.begin(); iter!=CStockMap::Instance()->m_mapStock.end(); iter++)
    {
        ptrStock = iter->second;
        if (0 != PushItem(ptrStock, nDate, bReportDate, strRetCode))
        {
            ERROR("GetListStock Call PushItem error. strRetCode: "<<strRetCode<<", ptrStock->m_strCode: "<<ptrStock->m_strCode);
            return 1;
        }
    }

    for (int i=0; i<m_vecStock.size(); i++){
		json_array_append_new(jArrCodes, json_string((m_vecStock[i]->m_strCode).c_str()));
    }

    return 0;
}


/* GetListAll
字段名称	值类型	备注
method	字符串cond.filter	
code	数组	代码列表[code1, code2, code3, …]
item	字符串	需要比较的字段
date	整型或空	结束日期，例如20170121
traverse	整数	遍历数量
match	整数	匹配数量
value	整数	基准值，traverse有match个值大于value
sort	字符串	match个值的平均做asc或者desc

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
result	数组[{},{},{}…]	响应字段。数组元素{“cde”: xxx, ”name”: xxx, ”avg”: xxx, “match”:xxx, “rank”:xxx}
*/
int CCurrList::GetListAll(
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
    size_t sJsonSize   = 0;
	int iDate          = 0;
	string strTemp     = "";
	bool bReportDate   = false;
	vector<string> vecItems;
	map<string, StockData*>::iterator iter;
	StockData *ptrStock = NULL;

    // 解析json，得到各字段item,date
    json_t *jDate = json_object_get(jReq, "date");
    if (jDate && json_is_integer(jDate)) 
	{
		iDate = json_integer_value(json_object_get(jReq, "date"));
		if (((0 != iDate) && (iDate < 19800000)) || (iDate > 99999999))
		{
	        strRetCode = CErrJsonType;
			ERROR("GetListAll Get json date value error. strRetCode: "<<strRetCode<<", iDate: "<<iDate);
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

	json_t *jItem = json_object_get(jReq, "item");
    if (!jItem || !json_is_array(jItem)) {
        strRetCode = CErrJsonType;
		ERROR("GetListAll Get json item error. strRetCode: "<<strRetCode);
		return 1;
    }
	sJsonSize = json_array_size(jItem);
	if (0 == sJsonSize)
	{
        strRetCode = CErrJsonType;
		ERROR("GetListAll Get json item error. strRetCode: "<<strRetCode);
		return 1;
	}
	
	for (int i=0; i<sJsonSize; i++)
	{
	    vecItems.push_back(json_string_value(json_array_get(jItem, i)));
	}

	// 从stockmap中找到满足条件的放入容器
	for (iter=CStockMap::Instance()->m_mapStock.begin(); iter!=CStockMap::Instance()->m_mapStock.end(); iter++)
	{
	    ptrStock = iter->second;
		if (0 != PushItem(ptrStock, iDate, bReportDate, strRetCode))
		{
			ERROR("GetListStock Call PushItem error. strRetCode: "<<strRetCode<<", ptrStock->m_strCode: "<<ptrStock->m_strCode);
			return 1;
		}
		//m_vecStock.push_back(ptrStock);
	}
	
	// 根据item生成容器元素的信息
	json_object_set_new(jRes, "method", json_string("list.all"));
	if (0 != ShowResult(vecItems, jRes,  strRetCode))
	{
	    ERROR("Call ShowResult error. strRetCode: "<<strRetCode);
		return 1;
	}
	
    return 0;
}

int CCurrList::GetListSub(
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
	size_t sJsonSize   = 0;
	string strSub      = "";
	string strTemp     = "";
	int iDate          = 0;
	bool bReportDate   = false;
	vector<string> vecItems;
	map<string, StockData*>::iterator iter;
	StockData *ptrStock = NULL;
	
	// 解析json，得到各字段sub,item,date
	json_t *jSub = json_object_get(jReq, "sub");
    if (!jSub || !json_is_string(jSub)) {
		strRetCode = CErrJsonType;
	    ERROR("GetListSub json sub is not string. strRetCode: "<<strRetCode);
		return 1;
    }
	strSub = json_string_value(jSub);
		
	json_t *jDate = json_object_get(jReq, "date");
	if (jDate && json_is_integer(jDate)) 
	{
		iDate = json_integer_value(json_object_get(jReq, "date"));
		if (((0 != iDate) && (iDate < 19800000)) || (iDate > 99999999))
		{
			strRetCode = CErrJsonType;
			ERROR("GetListSub Get json date value error. strRetCode: "<<strRetCode<<", iDate: "<<iDate);
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
	
	json_t *jItem = json_object_get(jReq, "item");
	if (!jItem || !json_is_array(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("GetListSub Get json item error. strRetCode: "<<strRetCode);
		return 1;
	}
	sJsonSize = json_array_size(jItem);
	if (0 == sJsonSize)
	{
		strRetCode = CErrJsonType;
		ERROR("GetListSub Get json item error. strRetCode: "<<strRetCode);
		return 1;
	}
	
	for (int i=0; i<sJsonSize; i++)
	{
		vecItems.push_back(json_string_value(json_array_get(jItem, i)));
	}
	
	// 从stockmap中找到满足条件的放入容器
	for (iter=CStockMap::Instance()->m_mapStock.begin(); iter!=CStockMap::Instance()->m_mapStock.end(); iter++)
	{
		ptrStock = iter->second;
		if (0 != strSub.compare(ptrStock->m_objInfo.m_strSubIndustry))
		{
		    continue;
		}
		
		if (0 != PushItem(ptrStock, iDate, bReportDate, strRetCode))
		{
			ERROR("GetListStock Call PushItem error. strRetCode: "<<strRetCode<<", ptrStock->m_strCode: "<<ptrStock->m_strCode);
			return 1;
		}
		//m_vecStock.push_back(ptrStock);
	}
	
	// 根据item生成容器元素的信息
	json_object_set_new(jRes, "method", json_string("list.sub"));
	if (0 != ShowResult(vecItems, jRes,  strRetCode))
	{
		ERROR("Call ShowResult error. strRetCode: "<<strRetCode);
		return 1;
	}
	
	return 0;

}

int CCurrList::GetListSort(
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
	size_t sJsonSize    = 0;
	json_t *jItem       = NULL;
	int iDate           = 0;
	bool bReportDate    = false;
	string strTemp      = "";
	string strSortItem  = "";
	string strSortType  = "";
	vector<string> vecCodes;
	vector<string> vecItems;
	map<string, StockData*>::iterator iter;
	StockData *ptrStock = NULL;
	
	// 解析json，得到各字段code,item,date
	// code
	jItem = json_object_get(jReq, "code");
	if (!jItem || !json_is_array(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("GetListSort Get json code error. strRetCode: "<<strRetCode);
		return 1;
	}
	sJsonSize = json_array_size(jItem);
	if (0 == sJsonSize)
	{
		strRetCode = CErrJsonType;
		ERROR("GetListSort Get json code error. strRetCode: "<<strRetCode);
		return 1;
	}
	for (int i=0; i<sJsonSize; i++)
	{
		vecCodes.push_back(json_string_value(json_array_get(jItem, i)));
	}

    // sortitem
	jItem = json_object_get(jReq, "sortitem");
	if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("GetListSort json sortitem is not string. strRetCode: "<<strRetCode);
		return 1;
	}
	strSortItem = json_string_value(jItem);

    // sorttype
	jItem = json_object_get(jReq, "sorttype");
	if (!jItem || !json_is_string(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("GetListSort json sorttype is not string. strRetCode: "<<strRetCode);
		return 1;
	}
	strSortType = json_string_value(jItem);

	// date
	jItem = json_object_get(jReq, "date");
	if (jItem && json_is_integer(jItem)) 
	{
		iDate = json_integer_value(jItem);
		if (((0 != iDate) && (iDate < 19800000)) || (iDate > 99999999))
		{
			strRetCode = CErrJsonType;
			ERROR("GetListSort Get json date value error. strRetCode: "<<strRetCode<<", iDate: "<<iDate);
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

    // item
	jItem = json_object_get(jReq, "item");
	if (!jItem || !json_is_array(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("GetListSort Get json item error. strRetCode: "<<strRetCode);
		return 1;
	}
	sJsonSize = json_array_size(jItem);
	if (0 == sJsonSize)
	{
		strRetCode = CErrJsonType;
		ERROR("GetListSort Get json item error. strRetCode: "<<strRetCode);
		return 1;
	}
	for (int i=0; i<sJsonSize; i++)
	{
		vecItems.push_back(json_string_value(json_array_get(jItem, i)));
	}
	
	// 从stockmap中找到满足条件的放入容器
	for (int i=0; i<vecCodes.size(); i++)
	{
	    iter = CStockMap::Instance()->m_mapStock.find(vecCodes[i]);
		if (iter == CStockMap::Instance()->m_mapStock.end())
		{
		    continue;
		}

		if (0 != PushItem(iter->second, iDate, bReportDate, strRetCode))
		{
			ERROR("GetListSort Call PushItem error. strRetCode: "<<strRetCode<<", strCode: "<<iter->second->m_strCode);
			return 1;
		}
	}

	// 排序
	if (0 != SortList(strSortItem, strSortType, strRetCode))
	{
	    ERROR("SortList error. strRetCode: "<<strRetCode<<", strSortItem: "<<strSortItem<<", strSortType: "<<strSortType);
		return 1;
	}
	
	// 根据item生成容器元素的信息
	json_object_set_new(jRes, "method", json_string("list.sort"));
	if (0 != ShowResult(vecItems, jRes,  strRetCode))
	{
		ERROR("Call ShowResult error. strRetCode: "<<strRetCode);
		return 1;
	}
	
	return 0;

}

int CCurrList::GetListShow(
	json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	)
{
	size_t sJsonSize    = 0;
	json_t *jItem       = NULL;
	int iDate           = 0;
	bool bReportDate    = false;
	string strTemp      = "";
	vector<string> vecCodes;
	vector<string> vecItems;
	map<string, StockData*>::iterator iter;
	StockData *ptrStock = NULL;
	
	// 解析json，得到各字段code,item,date
	// code
	jItem = json_object_get(jReq, "code");
	if (!jItem || !json_is_array(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("GetListShow Get json code error. strRetCode: "<<strRetCode);
		return 1;
	}
	sJsonSize = json_array_size(jItem);
	if (0 == sJsonSize)
	{
		strRetCode = CErrJsonType;
		ERROR("GetListShow Get json code error. strRetCode: "<<strRetCode);
		return 1;
	}
	for (int i=0; i<sJsonSize; i++)
	{
		vecCodes.push_back(json_string_value(json_array_get(jItem, i)));
	}
	
	// date
	jItem = json_object_get(jReq, "date");
	if (jItem && json_is_integer(jItem)) 
	{
		iDate = json_integer_value(jItem);
		if (((0 != iDate) && (iDate < 19800000)) || (iDate > 99999999))
		{
			strRetCode = CErrJsonType;
			ERROR("GetListShow Get json date value error. strRetCode: "<<strRetCode<<", iDate: "<<iDate);
			return 1;
		}
	}

	jItem = json_object_get(jReq, "datetype");
    if (jItem && json_is_string(jItem)) {
		strTemp = json_string_value(jItem);
		if (strTemp == "report")
		{
		    bReportDate = true;
		}
		DEBUG("strTemp: ["<<strTemp<<"], bReportDate: "<<bReportDate);
    }
	else
	{
	    DEBUG("strTemp is not exist");
	}
	
	// item
	jItem = json_object_get(jReq, "item");
	if (!jItem || !json_is_array(jItem)) {
		strRetCode = CErrJsonType;
		ERROR("GetListShow Get json item error. strRetCode: "<<strRetCode);
		return 1;
	}
	sJsonSize = json_array_size(jItem);
	if (0 == sJsonSize)
	{
		strRetCode = CErrJsonType;
		ERROR("GetListShow Get json item error. strRetCode: "<<strRetCode);
		return 1;
	}
	for (int i=0; i<sJsonSize; i++)
	{
		vecItems.push_back(json_string_value(json_array_get(jItem, i)));
	}
	
	// 从stockmap中找到满足条件的放入容器
	for (int i=0; i<vecCodes.size(); i++)
	{
		iter = CStockMap::Instance()->m_mapStock.find(vecCodes[i]);
		if (iter == CStockMap::Instance()->m_mapStock.end())
		{
			continue;
		}
	
		if (0 != PushItem(iter->second, iDate, bReportDate, strRetCode))
		{
			ERROR("GetListShow Call PushItem error. strRetCode: "<<strRetCode<<", strCode: "<<iter->second->m_strCode);
			return 1;
		}
	}
	
	// 根据item生成容器元素的信息
	json_object_set_new(jRes, "method", json_string("list.show"));
	if (0 != ShowResult(vecItems, jRes,  strRetCode))
	{
		ERROR("Call ShowResult error. strRetCode: "<<strRetCode);
		return 1;
	}
	
	return 0;
}
	
int CCurrList::PushItem(
	StockData *ptrStock,
	int iDate,
	bool bReportDate,
	string &strRetCode
	)
{
	if (NULL == ptrStock)
	{
		return 0;
	}

    //DEBUG("code: "<<ptrStock->m_strCode<<", iDate: "<<iDate<<", bReportDate: "<<bReportDate);
	
	CListIdx objIdx;
	do
	{
	    /////////////// begin: 没有设定截止日期////////////////////////////
		if (0 == iDate)
		{
			if (ptrStock->m_vecQuart.size() > 0)  // 得到对应的坐标
			{
				objIdx.m_iQuartIdx1 = 0;  // 年坐标
				for (int i=3; i>=0; i--)  // 季度坐标
				{
					if (!ptrStock->m_vecQuart[0].m_rQuart[i].m_strCode.empty())
					{
						objIdx.m_iQuartIdx2 = i;
						break;
					}
				}
			}
			if (ptrStock->m_vecDayData.size() > 0)
			{
				objIdx.m_iDayIdx = 0;
			}
			break;
		}
		/////////////// end: 没有设定截止日期////////////////////////////

		
		//////////////begin: 设定了截止日期,且元素最晚日期在截止日期之前////////////////
		objIdx.m_iQuartIdx1 = ptrStock->GetQuartIdx(iDate/10000);
		if (0 > objIdx.m_iQuartIdx1) // 没有找到对应财报年数据
		{
		    // 日期在截止日期之前结束，判断是否加入选择
			if ((ptrStock->m_vecQuart.size() > 0) && (ptrStock->m_vecQuart[0].m_iYear < iDate/10000))
			{
				objIdx.m_iQuartIdx1 = 0;  // 年坐标
				for (int i=3; i>=0; i--)  // 季度坐标
				{
					if (!ptrStock->m_vecQuart[0].m_rQuart[i].m_strCode.empty())
					{
						objIdx.m_iQuartIdx2 = i;
						break;
					}
				}
			}
			objIdx.m_iDayIdx = ptrStock->GetDayIdx(iDate);
			if ((ptrStock->m_vecDayData.size() > 0) && (ptrStock->m_vecDayData[0].date < iDate)) // 最新日期在截止日期之前
			{
			    objIdx.m_iDayIdx = 0;
			}
		    break;
		}
		//////////////end: 设定了截止日期,且元素最晚日期在截止日期之前////////////////

        /////////////begin: 设定了截止日期，且元素日期包含截止日期
		if (objIdx.m_iQuartIdx1 >= ptrStock->m_vecQuart.size())
		{
			strRetCode = CErrOutOfRange;
			ERROR("out of size. strRetCode: "<<strRetCode<<", strCptrStock code: "<<ptrStock->m_strCode<<", ptrStock->m_vecQuart.size(): "
				<<ptrStock->m_vecQuart.size()<<", objIdx.m_iQuartIdx1: "<<objIdx.m_iQuartIdx1);
			return 1;
		}

		//DEBUG("iDate: "<<iDate<<", objIdx.m_iQuartIdx1: "<<objIdx.m_iQuartIdx1<<", year: "<<ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_strYear
		//	<<", m_strDeclarationDate: "<<DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[3].m_strDeclarationDate));

		for (int i=3; i>=0; i--)
		{
			if (false == bReportDate)	// 交易日期为准
			{
				if ((!ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[i].m_strCode.empty()) 
					&& (iDate > DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[i].m_strDeclarationDate)))
				{
					objIdx.m_iQuartIdx2 = i;
					break;
				}
			}
			else  // 财报日期为准
			{
				if ((!ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[i].m_strCode.empty()) 
					&& (iDate > DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[i].m_strClosingDate)))
				{
					objIdx.m_iQuartIdx2 = i;
					break;
				}
			}
		}

		if (0 > objIdx.m_iQuartIdx2)  // 公告日期在上一年
		{
			if (objIdx.m_iQuartIdx1 + 1 >= ptrStock->m_vecQuart.size())
			{
				objIdx.m_iQuartIdx1 = -1;
			}
			else
			{
				objIdx.m_iQuartIdx1++;

				if (false == bReportDate)	// 交易日期为准
				{
					if (iDate > DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[3].m_strDeclarationDate))
					{
						objIdx.m_iQuartIdx2 = 3;
					}
					else if (iDate > DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[2].m_strDeclarationDate))
					{
						objIdx.m_iQuartIdx2 = 2;
					}
					else if (iDate > DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[1].m_strDeclarationDate))
					{
						objIdx.m_iQuartIdx2 = 1;
					}
					else // 此处仍然没有公告，则认为有问题
					{
						objIdx.m_iQuartIdx1 = -1;
						objIdx.m_iQuartIdx2 = -1;
					}
				}
				else  // 财报日期为准
				{
					if (iDate > DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[3].m_strClosingDate))
					{
						objIdx.m_iQuartIdx2 = 3;
					}
					else if (iDate > DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[2].m_strClosingDate))
					{
						objIdx.m_iQuartIdx2 = 2;
					}
					else if (iDate > DateStr2Int(ptrStock->m_vecQuart[objIdx.m_iQuartIdx1].m_rQuart[1].m_strClosingDate))
					{
						objIdx.m_iQuartIdx2 = 1;
					}
					else // 此处仍然没有公告，则认为有问题
					{
						objIdx.m_iQuartIdx1 = -1;
						objIdx.m_iQuartIdx2 = -1;
					}
				}
			}
		}
		objIdx.m_iDayIdx = ptrStock->GetDayIdx(iDate);
		if ((ptrStock->m_vecDayData.size() > 0) && (ptrStock->m_vecDayData[0].date < iDate))
		{
		    objIdx.m_iDayIdx = 0;
		}
		break;
		/////////////end: 设定了截止日期，且元素日期包含截止日期
	}while (false);
	
	//DEBUG("iDate: "<<iDate<<", objIdx.m_iQuartIdx1: "<<objIdx.m_iQuartIdx1<<", objIdx.m_iQuartIdx2: "<<objIdx.m_iQuartIdx1<<", objIdx.m_iDayIdx: "<<objIdx.m_iDayIdx);
	if ((objIdx.m_iDayIdx < 0) || (objIdx.m_iQuartIdx1 < 0) || (objIdx.m_iQuartIdx2 < 0))
	{
		return 0;
	}
	m_vecIdx.push_back(objIdx);
	m_vecStock.push_back(ptrStock);

	return 0;
}

int CCurrList::ShowResult(
	vector<string> vecItems,
	json_t *&jRes,
	string &strRetCode
	)
{
    eItemType eValueType;
	string strItemVal = "";
	json_t *jResult = json_array();
	
	//DEBUG("m_vecStock.size(): "<<m_vecStock.size());
	for (int i=0; i<m_vecStock.size(); i++)
	{
		//DEBUG("idx1: "<<m_vecIdx[i].m_iQuartIdx1<<", idx2: "<<m_vecIdx[i].m_iQuartIdx2<<", DayIdx: "<<m_vecIdx[i].m_iDayIdx);
		json_t *jItem = json_object();
		//DEBUG("vecItems.size(): "<<vecItems.size());
		for (int j=0; j<vecItems.size(); j++)
		{
			//DEBUG("Code: "<<m_vecStock[i]->m_strCode<<", Name: "<<m_vecStock[i]->m_strName); 
			if (1 != COutInfo::Instance()->m_mapItem.count(vecItems[j]))
			{
			    continue;
			}

			strItemVal = (m_vecStock[i]->*(COutInfo::Instance()->m_mapItem[vecItems[j]]))(m_vecIdx[i], eValueType, strRetCode);
			//DEBUG("strItemVal: "<<strItemVal);
			json_object_set_new(jItem, vecItems[j].c_str(), json_string(strItemVal.c_str()));
		}
		json_array_append_new(jResult, jItem);
	}
	
	json_object_set_new(jRes, "retcode", json_string(strRetCode.c_str()));
	json_object_set_new(jRes, "result", jResult);

	return 0;

}

// 不同类型的排序代码有重复，尚未想到如何优化一下
int CCurrList::SortList(     // 根据指定字段和升降序排序
	string strSortItem,
	string strSortType,
	string &strRetCode
	)
{
    eItemType eSortType = eEnd;
    string strItemVal = "";
	SortDoubleItem objDbItem;
	SortIntItem objIntItem;
	SortStrItem objStrItem;
	
	vector<SortDoubleItem> vecDbItem;
	vector<SortIntItem> vecIntItem;
	vector<SortStrItem> vecStrItem;
	vecDbItem.clear();
	vecIntItem.clear();
	vecStrItem.clear();

	if (0 == m_vecStock.size())
	{
	    return 0;
	}
	
    if (1 != COutInfo::Instance()->m_mapItem.count(strSortItem))  // 是否支持此字段
	{
	    strRetCode = CErrItemNone;
		ERROR("SortList error. strRetCode: "<<strRetCode<<", strSortItem: "<<strSortItem);
		return 1;
	}

	DEBUG("SortItem: "<<strSortItem<<", SortType: "<<strSortType);
	for (int i=0; i<m_vecStock.size(); i++)
	{
	    strItemVal = (m_vecStock[i]->*(COutInfo::Instance()->m_mapItem[strSortItem]))(m_vecIdx[i], eSortType, strRetCode);
		if (eDouble == eSortType) // 根据字段类型不同，放入不同容器,只有一个容器有用
		{
		    objDbItem.m_idx = i;
			objDbItem.m_dKey = atof(strItemVal.c_str());
			DEBUG("objDbItem.m_dKey: "<<objDbItem.m_dKey);
		    vecDbItem.push_back(objDbItem);
			continue;
		}
		if (eInt == eSortType) // 根据字段类型不同，放入不同容器,只有一个容器有用
		{
		    objIntItem.m_idx = i;
			objIntItem.m_iKey = atoi(strItemVal.c_str());
			DEBUG("objIntItem.m_iKey: "<<objIntItem.m_iKey);
		    vecIntItem.push_back(objIntItem);
			continue;
		}
		if (eStr == eSortType)
		{
		    objStrItem.m_idx = i;
			objStrItem.m_strKey = strItemVal;
			vecStrItem.push_back(objStrItem);
			continue;
		}
	}

    if (eDouble == eSortType)
	{
	    if (m_vecStock.size() != vecDbItem.size())
    	{
    	    strRetCode = CErrCount;
			ERROR("m_vecStock.size(): "<<m_vecStock.size()<<" != vecDbItem.size(): "<<vecDbItem.size()
				<<". strRetCode: "<<strRetCode);
			return 1;
    	}
	    QSort(vecDbItem, 0, vecDbItem.size()-1); // 快速排序
		vector<StockData*> vecStock;
		vecStock.clear();
		vector<CListIdx> vecIdx;
		vecIdx.clear();
		for (int i=0; i<vecDbItem.size(); i++) // m_vecStock调整为升序排列, m_vecIdx做对应调整
		{
			vecStock.push_back(m_vecStock[vecDbItem[i].m_idx]);
			vecIdx.push_back(m_vecIdx[vecDbItem[i].m_idx]);
		}
		m_vecStock.clear();
		m_vecStock = vecStock;
		m_vecIdx.clear();
		m_vecIdx = vecIdx;

	}
	
    if (eInt == eSortType)
	{
	    if (m_vecStock.size() != vecIntItem.size())
    	{
    	    strRetCode = CErrCount;
			ERROR("m_vecStock.size(): "<<m_vecStock.size()<<" != vecDbItem.size(): "<<vecIntItem.size()
				<<". strRetCode: "<<strRetCode);
			return 1;
    	}
	    QSort(vecIntItem, 0, vecIntItem.size()-1); // 快速排序
		vector<StockData*> vecStock;
		vecStock.clear();
		vector<CListIdx> vecIdx;
		vecIdx.clear();
		for (int i=0; i<vecIntItem.size(); i++) // m_vecStock调整为升序排列, m_vecIdx做对应调整
		{
			vecStock.push_back(m_vecStock[vecIntItem[i].m_idx]);
			vecIdx.push_back(m_vecIdx[vecIntItem[i].m_idx]);
		}
		m_vecStock.clear();
		m_vecStock = vecStock;
		m_vecIdx.clear();
		m_vecIdx = vecIdx;

	}

	if (eStr == eSortType)
	{
	    if (m_vecStock.size() != vecStrItem.size())
    	{
    	    strRetCode = CErrCount;
			ERROR("m_vecStock.size(): "<<m_vecStock.size()<<" != vecStrItem.size(): "<<vecStrItem.size()
				<<". strRetCode: "<<strRetCode);
			return 1;
    	}
	    QSort(vecStrItem, 0, vecStrItem.size()-1);
		vector<StockData*> vecStock;
		vecStock.clear();
		vector<CListIdx> vecIdx;
		vecIdx.clear();
		for (int i=0; i<vecStrItem.size(); i++)
		{
			vecStock.push_back(m_vecStock[vecStrItem[i].m_idx]);
			vecIdx.push_back(m_vecIdx[vecStrItem[i].m_idx]);
		}
		m_vecStock.clear();
		m_vecStock = vecStock;
		m_vecIdx.clear();
		m_vecIdx = vecIdx;
	}

    if (0 == strSortType.compare("asc")) // 默认降序排列，如果是升序，逆转一下vec的元素
    {
		reverse(m_vecStock.begin(), m_vecStock.end());
		reverse(m_vecIdx.begin(), m_vecIdx.end());
    }

    return 0;
}

