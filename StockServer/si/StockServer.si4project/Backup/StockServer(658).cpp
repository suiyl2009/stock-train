#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "DBData.h"
#include "Data.h"
#include "Log.h"
#include "Req.h"
#include "DBPool.h"

#include "fcgi_config.h"
#include "fcgiapp.h"

using namespace std;

int g_debug;

#define THREAD_COUNT 1000
int listen_socket;
int g_nServerID;

void* FcgiThread(void* param)
{

    FCGX_Request request;
	string strQuery = "";
	char *bufp = NULL;
   	CReq *ptrReq = new CReq;
	ptrReq->Init();

    FCGX_InitRequest(&request, listen_socket, 0);
    for ( ; ; )
    {
        static pthread_mutex_t accept_mutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_lock(&accept_mutex);
        int rc = FCGX_Accept_r(&request);
        pthread_mutex_unlock(&accept_mutex);
        if (rc < 0)
        {
            //ERRORLOG( "FCGX_Accept_r failed!!");
            break;
        }
		string strMethod = FCGX_GetParam("REQUEST_METHOD", request.envp);
		if (0 == strMethod.compare("POST"))
		{
		    int iLen = atoi(FCGX_GetParam("CONTENT_LENGTH", request.envp));
			bufp = (char*)malloc(iLen+1);
			bufp[iLen] = 0;
			FCGX_GetStr(bufp, iLen, request.in);
			strQuery = bufp;
		}
		/*if (!strcmp(method, "POST")) {
		    int ilen = atoi(getenv("CONTENT_LENGTH"));
		    char *bufp = malloc(ilen);
		    fread(bufp, ilen, 1, stdin);
		    printf("The POST data is<P>%s\n", bufp);
		    free(bufp);
		}*/
		else
		{
            strQuery = FCGX_GetParam("QUERY_STRING", request.envp);
		}
		cout<<"Request: "<<strQuery<<endl;
        string strResponse = "haha c++ fastcgi";
		ptrReq->Command(strQuery, strResponse);
        FCGX_FPrintF(request.out,
                   "Content-type: text/html\r\n"
                   "Content-Length: %d \r\n"
                   "\r\n"
                   "%s", strResponse.length(), strResponse.c_str());
        FCGX_Finish_r(&request);
		cout<<"Response: "<<strResponse<<endl;
		if (NULL != bufp)
		{
			delete bufp;
			bufp = NULL;
		}
    }
	delete ptrReq;
    ptrReq = NULL;
	
    return NULL;
}

int main(int argc, char *argv[])
{
    g_debug = 1;
    DEBUG("Hello, world!");

	if (0 != CDBPool::Instance()->Init(cStrDBURL, cStrUser, cStrPwd, cStrDBName, cIDBPort))
	{
	    cout<<"CDBPool::Instance()->Init failed. cstrDBURL: "<<cStrDBURL<<", cstrUser: "<<cStrUser
			<<", cstrPwd: "<<cStrPwd<<", cstrDBName: "<<cStrDBName<<", cIDBPort: "<<cIDBPort<<endl;
		return 1;
	}

    if (0 != CDBData::Instance()->Init())
	{
	    cout<<"CDBData::Instance()->Init() failed."<<endl;
		return 1;
	}
	DEBUG("CDBData::Instance()->Init()");
	
	if (0 != CStockMap::Instance()->Init())
	{
	    cout<<"CStockMap::Instance()->Init() failed."<<endl;
		return 1;
	}

	if (0 != COptionMap::Instance()->Init())
	{
		cout<<"COptionMap::Instance()->Init() failed."<<endl;
		return 1;
	}

    if (0 != CInitPack::Instance()->Init())
    {
		cout<<"CInitPack::Instance()->Init() failed."<<endl;
		return 1;
    }
	
    pthread_t id[THREAD_COUNT];
    FCGX_Init();
    int listenQueueBacklog = 1000;
    mode_t oldMask = umask(0);
    char tempBuf[128] = {0};
	g_nServerID = 1000;
    sprintf(tempBuf, "/tmp/StockServer_%d.sock", g_nServerID);
    listen_socket = FCGX_OpenSocket(tempBuf, listenQueueBacklog);
    umask(oldMask);

    if (listen_socket < 0)
    {
        cout << "fcgi open socket " << tempBuf << " failed" << endl;
        return 0;
    }
    else
    {
        cout << "fcgi open socket "<< tempBuf << " success" << endl;
    }

    int nThreadCount = 10;
    nThreadCount = min(THREAD_COUNT, nThreadCount);
    int *index = new int[nThreadCount];
    for (int i = 0; i < nThreadCount; i++)
        pthread_create(&id[i], NULL, FcgiThread, (void*)&index[i]);

    for (int i = 0; i < nThreadCount; i++)
        pthread_join(id[i], 0);
    delete []index;
    
    return 0;
}

