#ifndef __DB_POOL_H__
#define __DB_POOL_H__

#include <iostream>
#include <list>
#include <string.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/exception.h>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>

using namespace std;
using namespace sql;

class CDBPool
{
public:
	static CDBPool* Instance();
	static void Remove();
	
	~CDBPool();
	
	int Init(string strDBUrl, string strUser, string strPwd, string strDBName, int iPort);
	
	Connection* GetConnect();
	
	void PushConnect(Connection* pMysql);

private:
	static CDBPool* m_pInstance;
	
	CDBPool();
	Connection* CreateConnect();
    bool testConn(Connection* pConn);

	string m_strDBUrl;
	string m_strUser;
	string m_strPwd;
	string m_strDBName;
    string m_strDBTcp;
	int m_iPort;

    Driver* driver;
	list<Connection*> m_listPool; //连接池的容器队列
	pthread_mutex_t m_Mutex; //线程锁
};

#endif