//#include "stdafx.h"
#include <string>
#include <iostream>
#include "DBData.h"
#include "Log.h"
#include "StrSplit.h"
#include <string.h>
#include "convert.h"

using namespace std;

/*
std::string UTF8_To_GBK(const std::string &source)  
{  
    enum {GB2312 = 936};  
  
    unsigned long len = ::MultiByteToWideChar(CP_UTF8, NULL, source.c_str(), -1, NULL, NULL);  
    if (len == 0)  
        return std::string();  
    wchar_t *wide_char_buffer = new wchar_t[len];  
    ::MultiByteToWideChar(CP_UTF8, NULL, source.c_str(), -1, wide_char_buffer, len);  
      
    len = ::WideCharToMultiByte(GB2312, NULL, wide_char_buffer, -1, NULL, NULL, NULL, NULL);  
    if (len == 0)  
    {  
        delete [] wide_char_buffer;  
        return std::string();  
    }  
    char *multi_byte_buffer = new char[len];  
    ::WideCharToMultiByte(GB2312, NULL, wide_char_buffer, -1, multi_byte_buffer, len, NULL, NULL);  
      
    std::string dest(multi_byte_buffer);  
    delete [] wide_char_buffer;  
    delete [] multi_byte_buffer;  
    return dest;  
}  

//std::wstring UTF8_To_UTF16(const std::string &source)  
std::string UTF8_To_UTF16(const std::string &source)  
{  
    unsigned long len = ::MultiByteToWideChar(CP_UTF8, NULL, source.c_str(), -1, NULL, NULL);  
    if (len == 0)  
        return std::wstring();  
    wchar_t *buffer = new wchar_t[len];  
    ::MultiByteToWideChar(CP_UTF8, NULL, source.c_str(), -1, buffer, len);  
      
    std::wstring dest(buffer);  
    delete [] buffer;  
    return dest;  
}  
*/

CDBData* CDBData::m_ptrInstance = NULL;

CDBData* CDBData::Instance()
{
	if (NULL == m_ptrInstance)
	{
		m_ptrInstance = new CDBData;
	}
	return m_ptrInstance;
}
void CDBData::Remove()
{
	return;
}


CDBData::CDBData()
{
	bInited = false;
}

CDBData::~CDBData()
{
}

int CDBData::Init()
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	string strTest = "";
	int iRet = 0;

	mysql_init(&mysql);
	if (NULL == mysql_real_connect(&mysql, cStrDBURL.c_str(), cStrUser.c_str(), cStrPwd.c_str(), cStrDBName.c_str(), cIDBPort, NULL, 0))
	{
		ERROR("Call mysql_real_connect error");
		return 1;
	}
	DEBUG("mysql_real_connect ok");
	mysql_real_query(&mysql,"SET NAMES UTF8;",(unsigned long) strlen ("SET NAMES UTF8;"));
	/*
	char *sql = "select * from user";//unicode下用Cstring会很麻烦，直接用char  
	iRet = mysql_query(&mysql, sql);
	if (0 != iRet)
	{
		ERROR("Call mysql_query error. iRet: "<<iRet);
		return 1;
	}

	result = mysql_store_result(&mysql);
	if (NULL == result)
	{
		ERROR("Call mysql_store_result error. result is null.");
		return 1;
	}

	while ((row = mysql_fetch_row(result)) != NULL)
	{
		DEBUG("mysql_fetch_row user: "<<row[0]);
		cout<<row[1]<<endl;//这里是个显示的函数，控制台可以用cout  
		OutputDebugString(row[1]);
		OutputDebugString("\n");
	}
	*/

	/*
	    int field_num = mysql_field_count(&mysql);  
    //表头  
    MYSQL_FIELD * fields = mysql_fetch_fields(result);  
    int i = 0;  
  
    printf("------------------------------------------\n");  
    for(i= 0; i < field_num; i++){  
        printf("%s \t", fields[i].name);   
    }  
    printf("\n------------------------------------------\n");  
  
  
    //表内容  
    MYSQL_ROW row = NULL;  
    while(row = mysql_fetch_row(result)){  
        for(i= 0; i < field_num; i++){  
            printf("%s \t", row[i]);   
        }  
        printf("\n");  
    }  
	*/

	//mysql_free_result(result);

	bInited = true;
	return 0;
}

int CDBData::GetData(
	map<string, StockData*>& mapStock
	)
{
	int iRet = 0;
	vector<string> vecTable;

	// 从基本信息表获取数据
	iRet = GetInfoData(mapStock);
	if (0 != iRet)
	{
		ERROR("Call GetInfoData error. iRet: "<<iRet);
		return iRet;
	}
	DEBUG("After GetInfoData");
	// 从财报数据表获取数据
	iRet = GetReportData(mapStock);
	if (0 != iRet)
	{
		ERROR("Call GetReportData error. iRet: "<<iRet);
		return iRet;
	}
	DEBUG("After GetReportData");
	
	// 从分红送配表获取数据
	iRet = GetDivideData(mapStock);
	if (0 != iRet)
	{
		ERROR("Call GetDivideData error. iRet: "<<iRet);
		return iRet;
	}
	DEBUG("After GetDivideData");

	return iRet;
}

int CDBData::GetIndexData(
	/*io */map<string, CIndexData* > &mapIndex
	)
{
	int iRet = 0;
	CIndexData* ptrIndexData = NULL;
	map<string, CIndexData*>::iterator iter;

	MYSQL_RES *result;
	MYSQL_ROW row;
	string strSQL = "SELECT code,name,exchange from tb_index_info";
	iRet = mysql_query(&mysql, strSQL.c_str());
	if (0 != iRet)
	{
		ERROR("GetInfoData call mysql_query error. strSQL: "<<strSQL);
		return iRet;
	}
	
	result = mysql_store_result(&mysql);
	if (NULL == result)
	{
		ERROR("GetInfoData call mysql_store_result error. strSQL: "<<strSQL);
		return 1;
	}
	DEBUG("number of result: "<<(unsigned long)mysql_num_rows(result));
	int j=mysql_num_fields(result);
	DEBUG("mysql_num_fields: "<<j);
	string strTemp;
	while (row = mysql_fetch_row(result))
	{
		//for (int i=0; i<j; i++)
		//	DEBUG("i: "<<i<<", value: "<<(row[i]? row[i]:"null"));
		objBaseInfo.m_strCode         = (row[0] ? row[0] : "");
		objBaseInfo.m_strName         = (row[1] ? row[1] : "");
		vecBaseInfo.push_back(objBaseInfo);
	}
	mysql_free_result(result);

	DEBUG("GetInfoData vecBaseInfo.size(): "<<vecBaseInfo.size());
	DEBUG("GetInfoData before mapStock.size(): "<<mapStock.size());
	for (int idx=0; idx<vecBaseInfo.size(); idx++)
	{
		string strCode = vecBaseInfo[idx].m_strCode;
		iter = mapStock.find(strCode);
		if (mapStock.end() != iter)
		{
			ptrStockData = iter->second;
		}
		else
		{
			ptrStockData = new StockData;
			ptrStockData->m_strCode = vecBaseInfo[idx].m_strCode;
			ptrStockData->m_strName = vecBaseInfo[idx].m_strName;
			mapStock[vecBaseInfo[idx].m_strCode] = ptrStockData;
		}
		ptrStockData->m_objInfo = vecBaseInfo[idx];
		ptrStockData->m_bInfo   = true;
	}
	DEBUG("GetInfoData after mapStock.size(): "<<mapStock.size());

	return iRet;
}

int CDBData::GetInfoData(
	/*io */map<string, StockData* > &mapStock
	)
{
	int iRet = 0;
	StockData* ptrStockData = NULL;
	map<string, StockData*>::iterator iter;
	BaseInfo objBaseInfo;
	vector<BaseInfo> vecBaseInfo;
	vecBaseInfo.clear();

	MYSQL_RES *result;
	MYSQL_ROW row;
	string strSQL = "SELECT code,name,pe,industry,subindustry,area,floatstock,floatvalue,totalvalue,totalstock,launchdate,";
	strSQL += "shareholders,perholding,pervalue,getdate from tb_stock_info";
	iRet = mysql_query(&mysql, strSQL.c_str());
	if (0 != iRet)
	{
		ERROR("GetInfoData call mysql_query error. strSQL: "<<strSQL);
		return iRet;
	}
	
	result = mysql_store_result(&mysql);
	if (NULL == result)
	{
		ERROR("GetInfoData call mysql_store_result error. strSQL: "<<strSQL);
		return 1;
	}
	DEBUG("number of result: "<<(unsigned long)mysql_num_rows(result));
	int j=mysql_num_fields(result);
	DEBUG("mysql_num_fields: "<<j);
	string strTemp;
	while (row = mysql_fetch_row(result))
	{
		//for (int i=0; i<j; i++)
		//	DEBUG("i: "<<i<<", value: "<<(row[i]? row[i]:"null"));
		objBaseInfo.m_strCode         = (row[0] ? row[0] : "");
		objBaseInfo.m_strName         = (row[1] ? row[1] : "");
		//strTemp                       = UTF8_To_GBK(objBaseInfo.m_strName);
		//objBaseInfo.m_strName         = strTemp;
		objBaseInfo.m_iPE             = (row[2] ? int(100*atof(row[2])) : 0);
		objBaseInfo.m_strIndustry     = (row[3] ? row[3] : "");
		objBaseInfo.m_strSubIndustry  = (row[4] ? row[4] : "");
		//strTemp                       = UTF8_To_GBK(objBaseInfo.m_strSubIndustry);
		//objBaseInfo.m_strSubIndustry  = strTemp;
		objBaseInfo.m_strArea         = (row[5] ? row[5] : "");
		objBaseInfo.m_iFloatStock     = (row[6] ? int(100*atof(row[6])) : 0);
		objBaseInfo.m_iFloatValue     = (row[7] ? int(100*atof(row[7])) : 0);
		objBaseInfo.m_iTotalValue     = (row[8] ? int(atof(row[8])) : 0);
		objBaseInfo.m_iTotalStock     = (row[9] ? int(atof(row[9])) : 0);
		objBaseInfo.m_strLaunchDate   = (row[10] ? row[10] : "");
		objBaseInfo.m_iShareHolder    = (row[11] ? int(atof(row[11])) : 0);
		objBaseInfo.m_iPerHolding     = (row[12] ? int(atof(row[12])) : 0);
		objBaseInfo.m_iPerValue       = (row[13] ? int(100*atof(row[13])) : 0);
		objBaseInfo.m_strGetDate      = (row[14] ? row[14] : "");
		vecBaseInfo.push_back(objBaseInfo);
	}
	mysql_free_result(result);

	DEBUG("GetInfoData vecBaseInfo.size(): "<<vecBaseInfo.size());
	DEBUG("GetInfoData before mapStock.size(): "<<mapStock.size());
	for (int idx=0; idx<vecBaseInfo.size(); idx++)
	{
		string strCode = vecBaseInfo[idx].m_strCode;
		iter = mapStock.find(strCode);
		if (mapStock.end() != iter)
		{
			ptrStockData = iter->second;
		}
		else
		{
			ptrStockData = new StockData;
			ptrStockData->m_strCode = vecBaseInfo[idx].m_strCode;
			ptrStockData->m_strName = vecBaseInfo[idx].m_strName;
			mapStock[vecBaseInfo[idx].m_strCode] = ptrStockData;
		}
		ptrStockData->m_objInfo = vecBaseInfo[idx];
		ptrStockData->m_bInfo   = true;
	}
	DEBUG("GetInfoData after mapStock.size(): "<<mapStock.size());

	return iRet;
}

int CDBData::GetReportTable(
	/*out*/vector<string> &vecTable
	)
{
	vecTable.clear();

	MYSQL_RES *result;
	MYSQL_ROW row;
	string strItem = "";

	int iRet = 0;
	string strSQL = "SELECT table_name FROM information_schema.tables WHERE table_schema='";
	strSQL += cStrDBName;
	strSQL += "' AND table_type='base table'  AND LEFT(table_name,1) ='a' ORDER BY table_name DESC";
	iRet = mysql_query(&mysql, strSQL.c_str());
	if (0 != iRet)
	{
		ERROR("GetReportTable call mysql_query error. strSQL: "<<strSQL);
		return 1;
	}

	result = mysql_store_result(&mysql);
	if (NULL == result)
	{
		ERROR("GetReportTable call mysql_store_result error. strSQL: "<<strSQL);
		return 1;
	}

	while ((row = mysql_fetch_row(result)) != NULL)
	{
		if (row[0])
		{
			strItem = row[0];
			vecTable.push_back(strItem);
		}
	}
	mysql_free_result(result);

	DEBUG("GetReportTable vecTable size: "<<vecTable.size()<<", vecTable[0]: "<<vecTable[0]
	    <<", vecTable[size-1]: "<<vecTable[vecTable.size()-1]);

	return 0;
}

int CDBData::GetReportData(
	/*io */map<string, StockData*>& mapStock
	)
{
	int iRet = 0;
	StockData* ptrStockData = NULL;
	QuartData* ptrQuart = NULL;
	map<string, StockData*>::iterator iter;
	ReportData objReport;
	vector<ReportData> vecReport;
	vecReport.clear();

	MYSQL_RES *result;
	MYSQL_ROW row;
	string strSQL = "SELECT closingDate,code,name,earnOne1,earnOne2,revenue,revenueRose1,revenueRose2,";
	strSQL += "netProfit,netProfitRose1,netProfitRose2,netAssetOne,netAssetRate,cashFlowOne,grossProfit,";
	strSQL += "allocationProfit,dividentRate,declarationDate,unknown, equity from tb_report_info ";
	strSQL += "order by closingDate desc";
	iRet = mysql_query(&mysql, strSQL.c_str());
	if (0 != iRet)
	{
		ERROR("GetReportData call mysql_query error. strSQL: "<<strSQL);
		return iRet;
	}

	result = mysql_store_result(&mysql);
	if (NULL == result)
	{
		ERROR("GetReportData call mysql_store_result error. strSQL: "<<strSQL);
		return 1;
	}

	int i = 0;
	string strTemp;
	double dBig = 0;
	while ((row = mysql_fetch_row(result)) != NULL)
	{
		objReport.m_strClosingDate      = (row[0]  ? row[0] : "");
		objReport.m_strCode             = (row[1]  ? row[1] : "");
		objReport.m_strName             = (row[2]  ? row[2] : "");
		//if (i%2==0)
		//    strTemp = UTF8_To_GBK(objReport.m_strName);
		//else
		//	strTemp = UTF8_To_UTF16(objReport.m_strName);
		//objReport.m_strName = strTemp;
		objReport.m_iEarnOne1           = (row[3]  ? int(100*atof(row[3]))  : 0);
		objReport.m_iEarnOne2           = (row[4]  ? int(100*atof(row[4]))  : 0);
		dBig                            = (row[5]  ? atof(row[5])  : 0);
		objReport.m_iRevenue            = int(dBig/1000000);
		objReport.m_iRevenueRose1       = (row[6]  ? int(100*atof(row[6]))  : 0);
		objReport.m_iRevenueRose2       = (row[7]  ? int(100*atof(row[7]))  : 0);
		dBig                            = (row[8]  ? atof(row[8])  : 0);
		objReport.m_iNetProfit          = int(dBig/1000000);
		objReport.m_iNetProfitRose1     = (row[9]  ? int(100*atof(row[9]))  : 0);
		objReport.m_iNetProfitRose2     = (row[10] ? int(100*atof(row[10])) : 0);
		objReport.m_iNetAssertOne       = (row[11] ? int(100*atof(row[11])) : 0);
		objReport.m_iNetAssertRate      = (row[12] ? int(100*atof(row[12])) : 0);
		objReport.m_iCashFlowOne        = (row[13] ? int(100*atof(row[13])) : 0);
		objReport.m_iGrossProfit        = (row[14] ? int(100*atof(row[14])) : 0);
		//objReport.m_iGrossProfit        = objReport.m_iGrossProfit/100000000;
		objReport.m_strAllocationProfit = (row[15] ? row[15] : "");
		objReport.m_iDividentRate       = (row[16] ? int(100*atof(row[16])) : 0);
		objReport.m_strDeclarationDate  = (row[17] ? row[17] : "");
		objReport.m_strUnknown          = (row[18] ? row[18] : "");
		dBig                            = (row[19]  ? atof(row[19])  : 0);
		objReport.m_iEquity             = int(dBig/1000000);
		vecReport.push_back(objReport);
	}
	mysql_free_result(result);

	//DEBUG("GetReportData vecBaseInfo.size(): "<<vecReport.size());
	//DEBUG("GetReportData before mapStock.size(): "<<mapStock.size());
	for (int idx=0; idx<vecReport.size(); idx++)
	{
		string strCode = vecReport[idx].m_strCode;
		iter = mapStock.find(strCode);  // 查询映射表中是否存在stock记录，不存在则新建一个
		if (mapStock.end() != iter)
		{
			ptrStockData = iter->second;
		}
		else
		{
			/*ptrStockData = new StockData;
			ptrStockData->m_strCode = vecReport[idx].m_strCode;
			ptrStockData->m_strName = vecReport[idx].m_strName;
			mapStock[vecReport[idx].m_strCode] = ptrStockData;
			*/
			continue;
		}

		CStrSplit css;
		css.SetString(vecReport[idx].m_strClosingDate, "-"); // 切割日期字符串
		if (3 != css.Count())
		{
			ERROR("css.Count() is not 3. vecReport[idx].m_strClosingDate: "<<vecReport[idx].m_strClosingDate);
			return 1;
		}

		ptrQuart = NULL;
		if (0 == ptrStockData->m_vecQuart.size()) // 如果stock数据中没有report数据，新加一个
		{
			QuartData objQuartData;
			objQuartData.m_strYear = css.Item(0);
			ptrStockData->m_vecQuart.push_back(objQuartData);
			ptrQuart = &(ptrStockData->m_vecQuart[0]);
		}
		else
		{
			ptrQuart = &(ptrStockData->m_vecQuart[ptrStockData->m_vecQuart.size()-1]);
			if (ptrQuart->m_strYear != css.Item(0))  // 最后一个元素跟数据表元素不是同一年，需要新加一个元素
			{
				QuartData objQuartData;
				objQuartData.m_strYear = css.Item(0);
				ptrStockData->m_vecQuart.push_back(objQuartData);
				ptrQuart = &(ptrStockData->m_vecQuart[ptrStockData->m_vecQuart.size()-1]);
			}
		}
		if (NULL == ptrQuart)
		{
			ERROR("GetReportData unknown error. null == ptrQuart. strTable: tb_report_info, strCode: "<<strCode);
			return 1;
		}

		if (css.Item(1) == "03")
		{
			ptrQuart->m_rQuart[0] = vecReport[idx];
		}
		else if (css.Item(1) == "06")
		{
			ptrQuart->m_rQuart[1] = vecReport[idx];
		}
		else if (css.Item(1) == "09")
		{
			ptrQuart->m_rQuart[2] = vecReport[idx];
		}
		else if (css.Item(1) == "12")
		{
			ptrQuart->m_rQuart[3] = vecReport[idx];
		}
		else
		{
			ERROR("css mouth is error. vecReport[idx].m_strClosingDate: "<<vecReport[idx].m_strClosingDate);
			return 1;
		}
	}
	//DEBUG("GetReportData after mapStock.size(): "<<mapStock.size());

	return iRet;
}

int CDBData::GetDivideData(
	/*io */map<string, StockData*>& mapStock
	)
{
	int iRet = 0;
	StockData* ptrStockData = NULL;
	map<string, StockData*>::iterator iter;
	DivideInfo objDivideInfo;
	vector<DivideInfo> vecDivideInfo;
	vecDivideInfo.clear();
	
	MYSQL_RES *result;
	MYSQL_ROW row;
	string strSQL = "SELECT market, code, name, szzbl, sgbl, zgbl,xjfh, gxl, yaggr, yaggrhsrzf, gqdjrqsrzf, gqdjr, cqcxr, cqcxrhssrzf, ";
	strSQL += "ycqts, totalEquity, earningPerShare, netAssetsPerShare, mggjj, mgwfply, jlytbzz, reportingPeriod, resultsbyDate, ";
	strSQL += "projectProgress, allocationPlan FROM tb_divide_info ORDER BY CODE, reportingPeriod DESC;";
	iRet = mysql_query(&mysql, strSQL.c_str());
	if (0 != iRet)
	{
		ERROR("GetInfoData call mysql_query error. strSQL: "<<strSQL);
		return iRet;
	}
	result = mysql_store_result(&mysql);
	if (NULL == result)
	{
		ERROR("GetInfoData call mysql_store_result error. strSQL: "<<strSQL);
		return 1;
	}
	DEBUG("number of result: "<<(unsigned long)mysql_num_rows(result));
	int j=mysql_num_fields(result);
	DEBUG("mysql_num_fields: "<<j);
	string strTemp;
	while (row = mysql_fetch_row(result))
	{
		objDivideInfo.m_strMarket           = (row[0] ? row[0] : "");
		//objDivideInfo.m_strMarket           = UTF8_To_GBK(objDivideInfo.m_strMarket);
		objDivideInfo.m_strCode             = (row[1] ? row[1] : "");
		objDivideInfo.m_strName             = (row[2] ? row[2] : "");
		//objDivideInfo.m_strName             = UTF8_To_GBK(objDivideInfo.m_strName);
		objDivideInfo.m_iSzzbl              = (row[3] ? int(10*atof(row[3])) : 0);
		objDivideInfo.m_iSgbl               = (row[4] ? int(10*atof(row[4])) : 0);
		objDivideInfo.m_iZgbl               = (row[5] ? int(10*atof(row[5])) : 0);
		objDivideInfo.m_iXjfh               = (row[6] ? int(10*atof(row[6])) : 0);
		objDivideInfo.m_strYaggr            = (row[7] ? row[7] : "");            // 业绩披露日期
		objDivideInfo.m_iYaggrhsrzf         = (row[8] ? int(100*atof(row[8])) : 0);
		objDivideInfo.m_iGqdjrqsrzf         = (row[9] ? int(100*atof(row[9])) : 0);
		objDivideInfo.m_strGqdjr            = (row[10] ? row[10] : "");            // 股权登记日
		objDivideInfo.m_strCqcxr            = (row[11] ? row[11] : "");            // 除权除息日
		objDivideInfo.m_iCqcxrhssrzf        = (row[12] ? int(100*atof(row[12])) : 0);
		objDivideInfo.m_iYcqts              = (row[13] ? int(100*atof(row[13])) : 0);
		objDivideInfo.m_iTotalEquity        = (row[14] ? int(atof(row[14])) : 0);
		objDivideInfo.m_iEarningPerShare    = (row[15] ? int(100*atof(row[15])) : 0);    // 每股收益
		objDivideInfo.m_iNetAssetsPerShare  = (row[16] ? int(100*atof(row[16])) : 0);  // 每股净资产
		objDivideInfo.m_iMggjj              = (row[17] ? int(100*atof(row[17])) : 0);              // 每股公积金
		objDivideInfo.m_iMgwfply            = (row[18] ? int(100*atof(row[18])) : 0);            // 每股未分配利润（元）
		objDivideInfo.m_iJlytbzz            = (row[19] ? int(100*atof(row[19])) : 0);            // 净利润同比增长
		objDivideInfo.m_strReportingPeriod  = (row[20] ? row[20] : "");  // 报告期
		objDivideInfo.m_strResultsbyDate    = (row[21] ? row[21] : "");    // 业绩披露期
		objDivideInfo.m_strProjectProgress  = (row[22] ? row[22] : "");  // 分配进度
		//objDivideInfo.m_strProjectProgress  = UTF8_To_GBK(objDivideInfo.m_strProjectProgress);
		objDivideInfo.m_strAllocationPlan   = (row[23] ? row[23] : "");
		//objDivideInfo.m_strAllocationPlan   = UTF8_To_GBK(objDivideInfo.m_strAllocationPlan);
		vecDivideInfo.push_back(objDivideInfo);
	}
	mysql_free_result(result);

	DEBUG("GetDivideData vecDivideInfo.size(): "<<vecDivideInfo.size());
	DEBUG("GetDivideData before mapStock.size(): "<<mapStock.size());
	for (int i=0; i<vecDivideInfo.size(); i++)
	{
		if ((NULL == ptrStockData) || (ptrStockData->m_strCode != vecDivideInfo[i].m_strCode))
		{
			iter = mapStock.find(vecDivideInfo[i].m_strCode);
			if (mapStock.end() == iter)
			{
				continue;
			}
			ptrStockData = iter->second;
		}

		ptrStockData->m_vecDivide.push_back(vecDivideInfo[i]);
	}

	for (iter=mapStock.begin(); iter!=mapStock.end(); iter++) // 遍历mapStock
	{
		ptrStockData = iter->second;
		int i = ptrStockData->m_vecQuart.size();
		i = i-1;
		int j = i;
		int idx = ptrStockData->m_vecDivide.size();
		for (idx=idx-1; idx>=0; idx--) // 针对每个stock，建立report与divide对应关系
		{
			for (; i>=0; i--)  // 挂到截止日(close)财报记录下
			{
				 // 找到对应财报日期，挂到此财报记录下
				if (ptrStockData->m_vecQuart[i].m_strYear == ptrStockData->m_vecDivide[idx].m_strReportingPeriod.substr(0,4))
				{
					for (int t=0; t<4; t++)
					{
						if (ptrStockData->m_vecDivide[idx].m_strReportingPeriod == ptrStockData->m_vecQuart[i].m_rQuart[t].m_strClosingDate)
						{
							ptrStockData->m_vecQuart[i].m_rQuart[t].m_ptrDivide = &(ptrStockData->m_vecDivide[idx]);
							break;
						}
					}
					break;
				}
				// 没有对应财报日期，日期已过，处理下一个除权除息元素
				if (ptrStockData->m_vecQuart[i].m_strYear > ptrStockData->m_vecDivide[idx].m_strReportingPeriod.substr(0,4))
				{
					break;
				}
			}

			for (j; j>=0; j--) // 挂到除权除息日财报记录下
			{
				if (ptrStockData->m_vecQuart[j].m_strYear == ptrStockData->m_vecDivide[idx].m_strCqcxr.substr(0,4))
				{
					for (int t=3; t>=0; t--)
					{
						if ((!ptrStockData->m_vecQuart[j].m_rQuart[t].m_strClosingDate.empty())
							&& (ptrStockData->m_vecQuart[j].m_rQuart[t].m_strClosingDate <= ptrStockData->m_vecDivide[idx].m_strCqcxr))
						{
							ptrStockData->m_vecQuart[j].m_rQuart[t].m_ptrDivide = &(ptrStockData->m_vecDivide[idx]);
							break;
						}
					}
					break;
				}
				if (ptrStockData->m_vecQuart[j].m_strYear > ptrStockData->m_vecDivide[idx].m_strCqcxr.substr(0,4))
				{
					break;
				}
			}
		}
	}
	DEBUG("GetDivideData after mapStock.size(): "<<mapStock.size());

	return iRet;
}

int CDBData::GetSubIndustry(
	vector<CIndustryItem> &vecSubIndustry
	)
{
	vecSubIndustry.clear();

	MYSQL_RES *result;
	MYSQL_ROW row;
	string strItem = "";

	int iRet = 0;
	string strSQL = "SELECT COUNT(subindustry), subindustry FROM tb_stock_info GROUP BY subindustry;";
	iRet = mysql_query(&mysql, strSQL.c_str());
	if (0 != iRet)
	{
		ERROR("GetSubIndustry call mysql_query error. strSQL: "<<strSQL);
		return 1;
	}

	result = mysql_store_result(&mysql);
	if (NULL == result)
	{
		ERROR("GetSubIndustry call mysql_store_result error. strSQL: "<<strSQL);
		return 1;
	}

	while ((row = mysql_fetch_row(result)) != NULL)
	{
		CIndustryItem objItem;
		objItem.m_nCount = (row[0]  ? atof(row[0])  : 0);
		objItem.m_strIndustry = (row[1] ? row[1] : "");
		//strItem = (row[1] ? row[1] : "");
		//objItem.m_strIndustry = UTF8_To_GBK(strItem);
		vecSubIndustry.push_back(objItem);
	}
	mysql_free_result(result);

	DEBUG("GetSubIndustry vecTable size: "<<vecSubIndustry.size());

	return 0;
}

int CDBData::GetOptionInfo(
	vector<COptionInfo*> &vecOptionInfo
	)
{
	vecOptionInfo.clear();

	MYSQL_RES *result;
	MYSQL_ROW row;
	string strItem = "";

	int iRet = 0;
	string strSQL = "SELECT tab, code, note, date FROM tb_option_info;";
	iRet = mysql_query(&mysql, strSQL.c_str());
	if (0 != iRet)
	{
		ERROR("GetOptionInfo call mysql_query error. strSQL: "<<strSQL);
		return 1;
	}

	result = mysql_store_result(&mysql);
	if (NULL == result)
	{
		ERROR("GetOptionInfo call mysql_store_result error. strSQL: "<<strSQL);
		return 1;
	}

	while ((row = mysql_fetch_row(result)) != NULL)
	{
		COptionInfo *ptrItem = new COptionInfo;
		strItem = (row[0] ? row[0] : "");  // tab
		ptrItem->m_strTab = strItem; //UTF8_To_GBK(strItem);
		strItem = (row[1] ? row[1] : "");  // code
		ptrItem->m_strCode = strItem; // UTF8_To_GBK(strItem);
		strItem = (row[2] ? row[2] : "");  // note
		ptrItem->m_strNote = strItem; // UTF8_To_GBK(strItem);
		ptrItem->m_iCreateDate = (row[3] ? atoi(row[3]) : 0);
		vecOptionInfo.push_back(ptrItem);
	}
	mysql_free_result(result);

	DEBUG("GetOptionInfo vecTable size: "<<vecOptionInfo.size());

	return 0;
}

int CDBData::UpdateOptionInfo(
	int iType,
	COptionInfo objOptionInfo
	)
{

	return 1;
}

int CDBData::UpdateDeclDate(
	string strCode,
	string strCloseDate,
	string strDeclDate
	)
{
	if ((10 != strCloseDate.length()) || (strDeclDate.length() != 10))
	{
		return 1;
	}

	string strSQL = "update tb_report_info set declarationDate='" + strDeclDate + "' where code='" + strCode+"'";
	if (0 != mysql_query(&mysql, strSQL.c_str()))
	{
		ERROR("UpdateDeclDate call mysql_query error. strSQL: "<<strSQL);
		return 1;
	}
	DEBUG("strSQL: "<<strSQL);
	/*MYSQL_RES *result = mysql_store_result(&mysql);
	if (NULL != result)
	{
		mysql_free_result(result);
	}
	*/

	return 0;
}