#ifndef KEYS_H
#define KEYS_H

#include <string>
#include <sodium.h> 

struct Keys {
    std::string publicKey;
    std::string privateKey;
};

Keys KeyGeneration();
bool initSodium();

#endif