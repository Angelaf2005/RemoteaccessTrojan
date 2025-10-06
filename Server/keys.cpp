#include <sodium.h>
#include <iostream>
#include <vector>
#include <string>
#include "include/keys.h"


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

std::vector<unsigned char> fromHex(const std::string& hex) {
    std::vector<unsigned char> bin(hex.size() / 2);
    size_t bin_len = 0;

    if (sodium_hex2bin(
            bin.data(), bin.size(),
            hex.c_str(), hex.size(),
            nullptr, &bin_len, nullptr) != 0) {
        throw std::runtime_error("Hex inválido o malformado.");
    }

    bin.resize(bin_len); // Ajusta al tamaño real
    return bin;
}






std::string AesKey(){
    unsigned char aesKey[16];
    randombytes_buf(aesKey, sizeof(aesKey));
    char hex[33]; 
    sodium_bin2hex(hex, sizeof(hex), aesKey, sizeof(aesKey));
    return std::string(hex); 
}


std::string encryptMessageSeal(
    const std::string& message,
    const std::vector<unsigned char>& recipientPublicKey
) {
    std::vector<unsigned char> plaintext(message.begin(), message.end());
    std::vector<unsigned char> ciphertext(plaintext.size() + crypto_box_SEALBYTES);

    crypto_box_seal(ciphertext.data(), plaintext.data(), plaintext.size(), recipientPublicKey.data());
    return toHex(ciphertext);
}


std::string decryptMessageSeal(
    const std::vector<unsigned char>& ciphertext,
    const std::vector<unsigned char>& recipientPublicKey,
    const std::vector<unsigned char>& recipientPrivateKey
) {
    std::vector<unsigned char> decrypted(ciphertext.size() - crypto_box_SEALBYTES);

    if (crypto_box_seal_open(decrypted.data(), ciphertext.data(), ciphertext.size(),
        recipientPublicKey.data(), recipientPrivateKey.data()) != 0) {
        return "[ERROR] No se pudo descifrar.";
    }

    return std::string(decrypted.begin(), decrypted.end());
}


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

    std::cout << toHex(publicKey) << std::endl;
    
    return keys;
}