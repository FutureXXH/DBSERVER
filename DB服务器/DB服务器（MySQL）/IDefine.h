#ifndef __DEFINE_H
#define __DEFINE_H
#include <winsock2.h>
#include <windows.h>
#include<iostream>
#include "ConsoleColor.h"

#define NOWTIME(){ SYSTEMTIME sys;GetLocalTime(&sys);std::cout << sys.wHour << ":" << sys.wMinute << ":" << sys.wSecond;}
#define SERVERPRINT_INFO NOWTIME();std::cout << green <<"[INFO]" << white << ":"
#define SERVERPRINT_WARNING NOWTIME();std::cout<< yellow <<"[WARNING]" << white << ":"
#define SERVERPRINT_ERROR NOWTIME();std::cout  << red<<"[ERROR]" << white << ":"
#define DATA_BUFSIZE 1024



typedef struct
{
	OVERLAPPED Overlapped;
	WSABUF DataBuf;
	char data[DATA_BUFSIZE];
	SOCKET sock;
	int Type;  // 1 接收 2 发送

}PER_IO_OPERATION_DATA, * LPPER_IO_OPERATION_DATA;





#endif
