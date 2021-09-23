#include "TCPserver.h"

bool Tcpserver::InitServer(int port)
{
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
    {
        SERVERPRINT_ERROR << "Winsock初始化失败" << std::endl;
        return false;
    }



    ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    memset(&Server_adr, 0, sizeof(Server_adr));
    Server_adr.sin_family = AF_INET;
    Server_adr.sin_addr.S_un.S_addr = INADDR_ANY;
    Server_adr.sin_port = htons(port);

    if (::bind(ServerSocket, (struct sockaddr*)&Server_adr, sizeof(Server_adr)) == SOCKET_ERROR)
    {
        SERVERPRINT_ERROR << "bind error" << std::endl;
        return false;
    }

    if (listen(ServerSocket, 20) == SOCKET_ERROR)
    {
        SERVERPRINT_ERROR << "listen error" << std::endl;
        return false;
    }
    else
    {


        SERVERPRINT_INFO << "服务器网络初始化完成" << std::endl;
        return true;
    }

    SERVERPRINT_ERROR << "意外退出" << std::endl;
    return false;
}

bool Tcpserver::StartServer(int IOthreadnum)
{
    //设置服务器sock非阻塞模式
    unsigned long u = 1;
    if (ioctlsocket(ServerSocket, FIONBIO, &u) == SOCKET_ERROR)
    {
        SERVERPRINT_ERROR << "设置服务器非阻塞套接字失败!" << std::endl;
        return false;
    }

    //初始化完成端口
    CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    //把服务器监听sock加入进监听里
    CreateIoCompletionPort((HANDLE)ServerSocket, CompletionPort, (DWORD)ServerSocket, 0);



    for (int i = 0; i < IOthreadnum; i++)
    {
        thread* threadp = new thread(&Tcpserver::ProcessIO, this, CompletionPort);
    }


    //传递accept监听事件

    LPPER_IO_OPERATION_DATA PerIoData;
    PerIoData = new PER_IO_OPERATION_DATA();
    ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
    PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
    PerIoData->DataBuf.len = DATA_BUFSIZE;
    PerIoData->sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    DWORD dwRecv = 0;



    if (AcceptEx(ServerSocket, PerIoData->sock, PerIoData->DataBuf.buf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwRecv, &PerIoData->Overlapped) == false)
    {
        int error = WSAGetLastError();
        if (error != ERROR_IO_PENDING)
        {
            SERVERPRINT_ERROR << "投递服务器sock失败!" << error << std::endl;

            return false;
        }
    }

    return true;
}



void Tcpserver::ProcessIO(LPVOID lpParam)
{
    SERVERPRINT_INFO << "io线程开始运行" << std::endl;

    HANDLE CompletionPort = (HANDLE)lpParam;
    DWORD BytesTransferred;
    SOCKET csocket;
    LPPER_IO_OPERATION_DATA PerIoData;

    while (true)
    {
        if (0 == GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (PULONG_PTR)&csocket, (LPOVERLAPPED*)&PerIoData, 10000))
        {

            if ((GetLastError() == WAIT_TIMEOUT) || (GetLastError() == ERROR_NETNAME_DELETED))
            {
                //SERVERPRINT_INFO << "等待超时" << endl;
                //IOCPSERVER::deleteclient(*(ClientMap->at(csocket)));
            }
            else
            {

                SERVERPRINT_WARNING << "完成端口失败 可能是因为相关连接已经断开" << std::endl;
            }

            continue;

        }

        //如果有新socket连接
        if (csocket == ServerSocket)
        {
            SOCKADDR_IN* LocalAddr = nullptr;
            SOCKADDR_IN* ClientAddr = nullptr;
            int len = sizeof(SOCKADDR_IN);
            GetAcceptExSockaddrs(PerIoData->DataBuf.buf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, (LPSOCKADDR*)&LocalAddr, &len, (LPSOCKADDR*)&ClientAddr, &len);

            //把客户端SOCKET加入进完成端口里
            CreateIoCompletionPort((HANDLE)PerIoData->sock, CompletionPort, (DWORD)PerIoData->sock, 0);


            // 传递接收事件

            SOCKET temp = PerIoData->sock;
            ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
            PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
            PerIoData->DataBuf.len = DATA_BUFSIZE;
            DWORD Flags = 0;
            DWORD dwRecv = 0;
            WSARecv(temp, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);



            //继续传递连接事件
            PerIoData = new PER_IO_OPERATION_DATA();
            ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
            PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
            PerIoData->DataBuf.len = DATA_BUFSIZE;
            PerIoData->sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);


            if (AcceptEx(ServerSocket, PerIoData->sock, PerIoData->DataBuf.buf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwRecv, &PerIoData->Overlapped) == false)
            {
                int error = WSAGetLastError();
                if (error != ERROR_IO_PENDING)
                {
                    SERVERPRINT_ERROR << "投递服务器sock失败!" << std::endl;

                }
            }

            continue;
        }
        //连接关闭
        if (BytesTransferred == 0)
        {
            SERVERPRINT_INFO << "客户端退出" << csocket << std::endl;
            closesocket(csocket);
            delete PerIoData;

            continue;
        }

        //收到数据
        if (BytesTransferred >= 8)
        {
            SERVERPRINT_INFO << "接收到数据" << BytesTransferred  << "字节"<< std::endl;
            //处理数据
            int head = -1;
            int size = -1;

            memcpy(&head, PerIoData->data, 4);
            memcpy(&size, PerIoData->data + 4, 4);
            SERVERPRINT_INFO << "头" << head  << std::endl;
            switch (head)
            {
            case 1000:
                app::onLogin_1000();
                break;
            case 1001:
                app::onRegister_1001();
                break;

            default:
                break;
            }



        }
        else if(BytesTransferred > 0)
        {
            SERVERPRINT_INFO << "接收到不全数据" << BytesTransferred << "字节" << endl;
        }
        DWORD Flags = 0;
        DWORD dwRecv = 0;
        ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
        PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
        PerIoData->DataBuf.len = DATA_BUFSIZE;
        WSARecv(csocket, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
    }





}