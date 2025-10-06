#ifndef KEYS_H
#define KEYS_H

#include <string>
#include <sodium.h> 
#include <vector>

struct Keys {
    std::string publicKey;
    std::string privateKey;
};

bool initSodium();
std::string AesKey();
std::string toHex(const std::vector<unsigned char>& data);
void generateKeyPair(std::vector<unsigned char>& publicKey, std::vector<unsigned char>& privateKey);
std::string encryptMessageSeal(const std::string& message, const std::vector<unsigned char>& recipientPublicKey);
std::string decryptMessageSeal(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& recipientPublicKey, const std::vector<unsigned char>& recipientPrivateKey);
Keys KeyGeneration();
std::vector<unsigned char> fromHex(const std::string& hex);
#endif