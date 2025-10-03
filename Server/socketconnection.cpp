#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <cstring>
#include <iostream>
#include<stdio.h>
#include <cstdio>
#include <string>



#pragma comment(lib,"ws2_32.lib")

int main(){
    WSADATA wsa;
	SOCKET s;
	int result;
    struct sockaddr_in server;
	char recvbuf[512];
    int recvbuflen = 512;
	std::string messages;

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

    server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );

    if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d" , WSAGetLastError());
	}
	
	puts("Bind done");
	if(listen(s, 1) == SOCKET_ERROR){
        printf("Error listening on socket : %d" , WSAGetLastError());
    }else printf("Listen() is OK, I'm waiting for connctions...");
	
	SOCKET acceptSocket;
    while((acceptSocket = accept(s, NULL, NULL)) != INVALID_SOCKET){
		puts("Connection Established");
		while(true){
			ssize_t result = recv(acceptSocket, recvbuf, recvbuflen, 0);
			if (result > 0) {
			recvbuf[result] = '\0'; // Asegura que sea una cadena válida
            printf("%s> ", recvbuf);
			} else {
			puts("Connection Error");
			}
			std::getline(std::cin, messages);
        	send(acceptSocket,messages.c_str(),messages.size(),0);
			result = recv(acceptSocket, recvbuf, recvbuflen, 0);
			if (result > 0) {
			recvbuf[result] = '\0'; // Asegura que sea una cadena válida
            printf(recvbuf);
			} else {
			puts("Connection Error");
			}

		}
	}

    if(acceptSocket == INVALID_SOCKET){
        printf("accept failed: %d", WSAGetLastError());
        WSACleanup();
        return -1;
    }else printf("Connection Established");

	closesocket(s);
    return 0;

}