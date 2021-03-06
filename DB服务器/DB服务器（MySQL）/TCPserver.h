#ifndef __TCPSERVER_H
#define __TCPSERVER_H
#pragma comment(lib, "ws2_32")
#pragma comment(lib,"Mswsock.lib")
#pragma warning(disable:4996)
#include "IDefine.h"
#include "AppLogin.h"
#include <thread>
#include <winsock2.h>
#include <mswsock.h>




class Tcpserver
{
public:

	SOCKET ServerSocket;
	sockaddr_in Server_adr;
	HANDLE CompletionPort;


	bool InitServer(int port);
	bool StartServer(int IOthreadnum);
	void ProcessRead(LPVOID lpParam);
	void Send(SOCKET sendsocket,int head,char * sendbuffer,int sendsize);
};

extern Tcpserver* __TCPSERVER;
#endif // !__TCPSERVER_H

