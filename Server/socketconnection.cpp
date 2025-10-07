#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <cstring>
#include <iostream>
#include<stdio.h>
#include <cstdio>
#include <string>
#include "include/keys.h"
#include "include/crypto.h"


#pragma comment(lib,"ws2_32.lib")

bool initWinsock() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    std::cout << "Winsock initialized.\n";
    return true;
}

SOCKET createServerSocket(int port) {
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        return INVALID_SOCKET;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(s, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(s);
        return INVALID_SOCKET;
    }

    if (listen(s, 1) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(s);
        return INVALID_SOCKET;
    }

    std::cout << "Server listening on port " << port << "\n";
    return s;
}

SOCKET waitForClient(SOCKET serverSocket) {
    SOCKET client = accept(serverSocket, NULL, NULL);
    if (client == INVALID_SOCKET) {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
    } else {
        std::cout << "Client connected.\n";
    }
    return client;
}

bool handleClient(SOCKET clientSocket, std::string& KeyAes) {
    char recvbuf[4096];
    std::string comando;
    std::string comando_enc;
    std::vector<unsigned char> key(KeyAes.begin(), KeyAes.end());
    int n;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(clientSocket, &readfds);
    TIMEVAL timeout = { 2, 0 };

    while (true) {
        if (!std::getline(std::cin, comando)) {
        std::cout << "Entrada finalizada por el usuario.\n";
        break;
        }
        if (comando.empty()) continue;




        comando_enc = encryptAES(key,comando);




        if (send(clientSocket, comando_enc.c_str(), comando_enc.size(), 0) == SOCKET_ERROR) {
            std::cerr << "Send failed.\n";
            break;
        }

        Sleep(300);
        int TBytes = 0;
        do {
            int ready = select(0, &readfds, NULL, NULL, &timeout);
            if (ready <= 0) {
                std::cerr << "Cliente no responde o desconectado.\n";
                return true;
            }
            n = recv(clientSocket, recvbuf, sizeof(recvbuf) - 1, 0);
        
            if (n <= 0) {
                std::cerr << "Connection lost or error: " << WSAGetLastError() << std::endl;
                break;
            }
            std::string comando_enc(recvbuf,n);
            comando = decryptAES(key,comando_enc);
            std::cout << comando;
            TBytes += n;
            Sleep(100);
        } while (n == sizeof(recvbuf) - 1 || TBytes == 0);
         if (n <= 0) break;
    }
    return true;
}


void runServer(int port) {
    char recvbuf[4096];
    SOCKET serverSocket = createServerSocket(port);
    if (serverSocket == INVALID_SOCKET) return;

    while (true) {
        SOCKET clientSocket = waitForClient(serverSocket);
        if (clientSocket == INVALID_SOCKET) {
            Sleep(100); // espera antes de reintentar
            continue;
        }


        int m;
        m = recv(clientSocket, recvbuf, sizeof(recvbuf) - 1, 0);
        if (m <= 0) {
            std::cerr << "Conexión cerrada, reconectando...\n";
            continue;
        }
        recvbuf[m] = '\0';
        std::string Public_key(recvbuf, m);
        std::string keyAes = AesKey();
        std::string ciphertext = encryptMessageSeal(keyAes,fromHex(Public_key));
        m = send(clientSocket,ciphertext.c_str(),ciphertext.size(),0);
        if(m == SOCKET_ERROR) {
        std::cerr << "Connection failed.\n";
        closesocket(clientSocket);
        break;
        };

    
        bool success = handleClient(clientSocket, keyAes);
        closesocket(clientSocket);

        std::cout << "Esperando reconexión...\n";
        Sleep(2000); // tiempo de espera antes de aceptar otro cliente
    }

    closesocket(serverSocket);
    WSACleanup();
}














/* /* /* int main(){ 
    WSADATA wsa;
	SOCKET s;
	int result;
    struct sockaddr_in server;
	char recvbuf[4096];
	std::string messages;
	int n;




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
	std::string comando;
	acceptSocket = accept(s, NULL, NULL);
	if(acceptSocket == INVALID_SOCKET){
        printf("accept failed: %d", WSAGetLastError());
        WSACleanup();
        return -1;
    }else printf("Connection Established\n");
	


	








    while(true){
		int TBytes = 0;
		std::getline(std::cin, comando);
		comando += "\r\n"; 
		int j = send(acceptSocket, comando.c_str(), comando.size(), 0);
		if (j == SOCKET_ERROR) break;
		Sleep(300);
		do {
			n = recv(acceptSocket, recvbuf, sizeof(recvbuf) - 1, 0);
			if (n <= 0) {
				std::cerr << "\nConexión cerrada o error: " << WSAGetLastError() << std::endl;
				break;
			}
			recvbuf[n] = '\0';
			std::cout << recvbuf;
			TBytes += n;
			Sleep(100);
		} while (n == sizeof(recvbuf) - 1 || TBytes == 0);
	}


	closesocket(s);

    return 0;

} */ 