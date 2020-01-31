#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string.h>

#ifdef WIN32
#include <Windows.h>
#include <stdio.h>
#else

#define  MAX_PATH 260

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#endif

int GetCurrentPath(char buf[],char *pFileName);
char *GetIniKeyString(char *title,char *key,char *filename);
int GetIniKeyInt(char *title,char *key,char *filename);

#endif