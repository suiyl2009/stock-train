
#include<iostream>  
  
#include "mysql_driver.h"  
#include "mysql_connection.h"  
  
#include <cppconn/driver.h>  
  
using namespace std;  
  
void RunConnectMySQL()  
{  
    sql::mysql::MySQL_Driver *driver;  
    sql::Connection *con;  
  
    driver = sql::mysql::get_mysql_driver_instance();  
    con = driver->connect("tcp://192.168.72.128:3306", "root", "123456");  
  
    delete con;  
  
}  
  
int main(void)  
{  
    RunConnectMySQL();  
    return 0;  
}
