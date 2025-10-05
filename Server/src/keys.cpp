#include <sodium.h>
#include <iostream>
#include <vector>
#include <string>
#include "Keys.h"


std::string toHex(const std::vector<unsigned char>& data) {
    std::string hex;
    char buf[3];
    for (unsigned char byte : data) {
        snprintf(buf, sizeof(buf), "%02x", byte);
        hex += buf;
    }
    return hex;
}

bool initSodium() {
    return sodium_init() >= 0;
}

void generateKeyPair(std::vector<unsigned char>& publicKey, std::vector<unsigned char>& privateKey) {
    publicKey.resize(crypto_box_PUBLICKEYBYTES);
    privateKey.resize(crypto_box_SECRETKEYBYTES);
    crypto_box_keypair(publicKey.data(), privateKey.data());
}

std::vector<unsigned char> encryptMessage(
    const std::string& message,
    const std::vector<unsigned char>& publicKey,
    const std::vector<unsigned char>& privateKey,
    std::vector<unsigned char>& nonce
) {
    std::vector<unsigned char> plaintext(message.begin(), message.end());
    nonce.resize(crypto_box_NONCEBYTES);
    randombytes_buf(nonce.data(), nonce.size());

    std::vector<unsigned char> ciphertext(plaintext.size() + crypto_box_MACBYTES);
    crypto_box_easy(ciphertext.data(), plaintext.data(), plaintext.size(), nonce.data(), publicKey.data(), privateKey.data());
    return ciphertext;
}

std::string decryptMessage(
    const std::vector<unsigned char>& ciphertext,
    const std::vector<unsigned char>& publicKey,
    const std::vector<unsigned char>& privateKey,
    const std::vector<unsigned char>& nonce
) {
    std::vector<unsigned char> decrypted(ciphertext.size() - crypto_box_MACBYTES);
    if (crypto_box_open_easy(decrypted.data(), ciphertext.data(), ciphertext.size(), nonce.data(), publicKey.data(), privateKey.data()) != 0) {
        return "[ERROR] No se pudo descifrar.";
    }
    return std::string(decrypted.begin(), decrypted.end());
}

Keys* KeyGeneration() {

    std::vector<unsigned char> publicKey, privateKey;
    generateKeyPair(publicKey, privateKey);
    Keys* ServerKeys = new Keys;
    ServerKeys->privateKey = toHex(privateKey);
    ServerKeys->publicKey = toHex(publicKey);

  
    return ServerKeys;
}