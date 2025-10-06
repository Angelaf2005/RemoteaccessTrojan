#ifndef KEYS
#define KEYS
#include <string>
#include <sodium.h> 

struct Keys {
    std::string publicKey;
    std::string privateKey;
};

Keys* KeyGeneration();

#endif