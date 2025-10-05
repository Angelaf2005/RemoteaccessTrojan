#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <string>


bool initWinsock();
SOCKET createServerSocket(int port);
SOCKET waitForClient(SOCKET serverSocket);
bool handleClient(SOCKET clientSocket);
void runServer(int port);

#endif 