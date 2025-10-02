#include<stdio.h>
#include<winsock2.h>
#include<string>
#include<iostream>

#pragma comment(lib,"ws2_32.lib") 
 
int main(int argc , char *argv[])
{
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;
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
        std::getline(std::cin, messages);
        send(s,messages.c_str(),messages.size(),0);


    }
    
    
    
 
    return 0;
}