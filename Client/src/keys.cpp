#include <sodium.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include "keys.h"

// Convert bytes vector to hex
std::string toHex(const std::vector<unsigned char>& data) {
    std::string hex;
    char buf[3];
    for (unsigned char byte : data) {
        snprintf(buf, sizeof(buf), "%02x", byte);
        hex += buf;
    }
    return hex;
}

// initializes libsodium
bool initSodium() {
    return sodium_init() >= 0;
}

// Generate a key pair
void generateKeyPair(std::vector<unsigned char>& publicKey, std::vector<unsigned char>& privateKey) {
    publicKey.resize(crypto_box_PUBLICKEYBYTES);
    privateKey.resize(crypto_box_SECRETKEYBYTES);
    crypto_box_keypair(publicKey.data(), privateKey.data());
}

Keys KeyGeneration() {
    std::vector<unsigned char> publicKey, privateKey;
    generateKeyPair(publicKey, privateKey);
    Keys keys;
    keys.publicKey = toHex(publicKey);
    keys.privateKey = toHex(privateKey);
    
    return keys;
}