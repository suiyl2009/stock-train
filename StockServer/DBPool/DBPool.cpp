#include "DBPool.h"
#include "Log.h"

CDBPool* CDBPool::m_pInstance = NULL;

CDBPool* CDBPool::Instance()
{
    if (NULL == m_pInstance)
	{
	    m_pInstance = new CDBPool;
	}
	return m_pInstance;
}

void CDBPool::Remove()
{
    if (NULL != m_pInstance)
	{
	    delete m_pInstance;
		m_pInstance = NULL;
	}
}

CDBPool::CDBPool()
{
	m_strDBUrl   = "";
	m_strUser    = "";
	m_strPwd     = "";
	m_strDBName  = "";
    m_strDBTcp   = "";
	m_iPort      = 0;
    driver       = NULL;
    
    pthread_mutex_init(&m_Mutex, NULL);
	m_listPool.clear();
}

CDBPool::~CDBPool()
{
}

int CDBPool::Init(string strDBUrl, string strUser, string strPwd, string strDBName, int iPort)
{
    m_strDBUrl   = strDBUrl;
	m_strUser    = strUser;
	m_strPwd     = strPwd;
	m_strDBName  = strDBName;
	m_iPort      = iPort;

	char caURL[256] = { 0 };
	sprintf(caURL, "tcp://%s:%d/%s", m_strDBUrl.c_str(), m_iPort, strDBName.c_str());
	m_strDBTcp = caURL;

	try {
		this->driver = sql::mysql::get_driver_instance();
	}
	catch (sql::SQLException&e) 
	{
		perror("驱动连接出错;\n");
		return false;
	}
	catch (std::runtime_error&e) 
	{
		perror("运行出错了\n");
		return false;
	}

    Connection *pConn = CreateConnect();
	if (NULL == pConn)
	{
	    perror("CreateConnect error;\n");
	    return 1;
	}
	PushConnect(pConn);
	
	return 0;
}

Connection* CDBPool::GetConnect()
{
	Connection* pConn = NULL;

	pthread_mutex_lock(&m_Mutex);
	if (!m_listPool.empty())
	{
		pConn = m_listPool.front(); //得到第一个连接
		m_listPool.pop_front();   //移除第一个连接
		
		if (false == testConn(pConn)) 
		{   //如果连接已经被关闭，删除后重新建立一个
			delete pConn;
			pConn = CreateConnect();
            if (NULL == pConn)
    		{
    		    ERROR("Call CreateConnect error.");
    		}
		}
	}
	else
	{
	    pConn = CreateConnect();
		if (NULL == pConn)
		{
		    ERROR("Call CreateConnect error.");
		}
	}
    pthread_mutex_unlock(&m_Mutex);

	return pConn;
}

void CDBPool::PushConnect(Connection* pMysql)
{
    if (NULL != pMysql)
	{
		if (pMysql->isClosed()) 
		{   //如果连接已经被关闭，删除后重新建立一个
			delete pMysql;
            pMysql = NULL;
		}
        else
        {
        	pthread_mutex_lock(&m_Mutex);
    		m_listPool.push_front(pMysql);
    		pthread_mutex_unlock(&m_Mutex);
        }
	}
}

Connection* CDBPool::CreateConnect()
{
	Connection* conn = NULL;
	try 
	{
		INFO("Connect to database...");
		conn = driver->connect(m_strDBTcp.c_str(), m_strUser.c_str(), m_strPwd.c_str()); //建立连接
		Statement* state = conn->createStatement();
		state->execute("SET CHARSET utf8");
		delete state;

		INFO("创建数据库连接");
		return conn;
	}
	catch (sql::SQLException&e) 
	{
		perror("创建连接出错");
		return NULL;
	}
	catch (std::runtime_error&e) 
	{
		perror("运行时出错");
		return NULL;
	}

	return conn;

}

bool CDBPool::testConn(Connection* pConn)
{
    PreparedStatement* stmt = NULL;
    sql::ResultSet *res = NULL;
    try
    {
        stmt = pConn->prepareStatement("select now()");
		res = stmt->executeQuery();
        delete res;
        res = NULL;
		delete stmt;
        stmt = NULL;
    }
    catch (sql::SQLException ex)
    {
        ERROR( "sql::SQLException:" << ex.what());
        return false;
    }  
    catch (exception ex)
    {
        ERROR( "unknown Exception:" << ex.what());
        return false;
    } 

    return true;
}

/*
static CDBPool* m_pInstance;

string m_strDBUrl;
string m_strUser;
string m_strPwd;
string m_strDBName;
int m_iPort;

list<MYSQL*> m_listPool; //连接池的容器队列
pthread_mutex_t m_Mutex; //线程锁
*/

