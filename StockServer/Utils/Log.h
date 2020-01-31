#ifndef __LOG_H__
#define __LOG_H__

#include <fstream>
#include <iostream>
using namespace std;

extern int g_debug;

#define Log(x) {ofstream write;write.open("./log", ios::app);write<<x<<endl;write.close();}

#define INFO(x) Log("INFO: time: "<<time(NULL)<<", file: "<<__FILE__<<", line: "<<__LINE__<<". "<<x)
#define ERROR(x) Log("ERROR: time: "<<time(NULL)<<", file: "<<__FILE__<<", line: "<<__LINE__<<". "<<x)
#define DEBUG(x) if(g_debug)Log("DEBUG: time: "<<time(NULL)<<", file: "<<__FILE__<<", line: "<<__LINE__<<". "<<x)

#endif