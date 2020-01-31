#ifndef _DB_DATA__H
#define _DB_DATA__H

#include <iostream>
//#include <WinSock2.h>  
#include <mysql/mysql.h>  
#include <map>
#include <vector>
#include "Data.h"

using namespace std;

//#pragma comment(lib,"wsock32.lib")  
//#pragma comment(lib,"libmysql.lib")

//#define test

#ifdef test
const string cstrSHDir = "../../bakStockServer/DataTest/sh/lday/";
const string cstrSZDir = "../../bakStockServer/DataTest/sz/lday/";
const string cStrDBName  = "stocktest"; //"stockinfo";
#else
const string cstrSHDir = "../../bakStockServer/Data/sh/lday/";
const string cstrSZDir = "../../bakStockServer/Data/sz/lday/";
const string cStrDBName  = "stock";
#endif

//const string cStrDBURL   = "localhost";
const string cStrDBURL   = "192.168.0.105";
const string cStrUser    = "root";
const string cStrPwd     = "123456";

const int cIDBPort       = 3306;

class CDBData
{
public:
	static CDBData* Instance();
	static void Remove();
	~CDBData();

	int Init();

	int GetData(
		map<string, StockData*>& mapStock
		);

	int GetSubIndustry(
		vector<CIndustryItem> &vecSubIndustry
		);

	int GetOptionInfo(
		vector<COptionInfo*> &vecOptionInfo
		);

	int UpdateOptionInfo(
		int iType,
		COptionInfo objOptionInfo
		);

	int UpdateDeclDate(
		string strCode,
		string strCloseDate,
		string strDeclDate
		);

private:
	CDBData();

	int GetInfoData(
	/*io */map<string, StockData*>& mapStock
	);

	int GetReportTable(
		/*out*/vector<string> &vecTable
		);

	int GetReportData(
		/*io */map<string, StockData*>& mapStock
		);

	int GetDivideData(
		/*io */map<string, StockData*>& mapStock
		);

	static CDBData* m_ptrInstance;

	MYSQL mysql;
	bool bInited;
};

#endif
