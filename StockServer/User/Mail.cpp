#include "Mail.h"
#include <cstdlib>

int SendMail(
    string strTo, 
    string strSubject, 
    string strMsg
    )
{
    //int system(const char *cmdline /* 命令字符串 */);
    //echo 'this is test' | mail -s "test email." suiyl2009@163.com
    string strCmd = "";

    strCmd = "echo '" + strMsg + "' | mail -s '" + strSubject + "' " + strTo;
    //cout<<"strCmd: "<<strCmd<<endl;
    int nRet = system(strCmd.c_str());
    
    return nRet;
}

/*int main()
{
    string strTo = "632400443@qq.com";
    string strSubject = "puyu.com注册验证码";
    string strMsg = "puyu.com 用户注册验证码: 124659";
    int nRet = SendMail(strTo, strSubject, strMsg);
    cout<<"nRet: "<<nRet<<endl;
}*/
