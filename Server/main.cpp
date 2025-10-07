#include <iostream>
#include "include/socketconnection.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port;
    port = std::atoi(argv[1]);

    if (!initWinsock()) return 0;

    runServer(port);
    return 0;
}