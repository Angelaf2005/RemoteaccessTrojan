#ifndef AES_CBC_UTILS_H
#define AES_CBC_UTILS_H

#include <string>
#include <vector>

// Convierte un vector de bytes a un string hexadecimal
std::string BytesToHex(const std::vector<unsigned char>& data);

// Convierte un string hexadecimal a un vector de bytes
std::vector<unsigned char> HexToBytes(const std::string& hex);

// Encripta un mensaje usando AES-128-CBC con una clave de 16 bytes
// Devuelve un string hexadecimal que contiene IV + ciphertext
std::string encryptAES(const std::vector<unsigned char>& key, const std::string& msg);

// Desencripta un string hexadecimal que contiene IV + ciphertext usando AES-128-CBC
// Devuelve el mensaje original como string
std::string decryptAES(const std::vector<unsigned char>& key, const std::string& hexCipher);

#endif // AES_CBC_UTILS_H