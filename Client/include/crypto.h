#ifndef CRYPTO_H
#define CRYPTO_H

string encryptAES(const vector<unsigned char>& key, const string& msg);
string decryptAES(const vector<unsigned char>& key, const string& hexCipher);

#endif