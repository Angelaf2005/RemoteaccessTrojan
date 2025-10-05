#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <openssl/evp.h> 
#include <openssl/rand.h> // Para generar IV aleatorios, la sal

using namespace std;
//ENCRIPTAR AES 128 Cipher Block Chaining
// Convierte un vector de bytes a un string hexadecimal (para mostrar el cifrado)
string BytesToHex(const vector<unsigned char>& data) {
    stringstream ss;
    for (auto b : data)
        ss << hex << setw(2) << setfill('0') << (int)b;
    return ss.str();
}

// Convierte un string hexadecimal a un vector de bytes (para descifrar)
vector<unsigned char> HexToBytes(const string& hex) {
    vector<unsigned char> bytes;
    for (size_t i = 0; i < hex.size(); i += 2)
        bytes.push_back((unsigned char)strtol(hex.substr(i, 2).c_str(), nullptr, 16));
    return bytes;
}

// Recibe la clave (16 bytes) y el mensaje, devuelve IV+cipher en hex
string encryptAES(const vector<unsigned char>& key, const string& msg) {
    vector<unsigned char> iv(16); // IV de 16 bytes
    RAND_bytes(iv.data(), iv.size()); // Generamos IV aleatorio
    std::cout << iv << std::endl; 
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new(); // Creamos contexto de cifrado
    EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key.data(), iv.data()); // Inicializamos AES-128-CBC

    vector<unsigned char> cipher(msg.size() + 16); // Reservamos espacio para el cifrado
    int len, clen;

    // Ciframos los datos en bloques
    EVP_EncryptUpdate(ctx, cipher.data(), &len, (unsigned char*)msg.data(), msg.size());
    clen = len;

    // Finalizamos cifrado, aplicando padding
    EVP_EncryptFinal_ex(ctx, cipher.data() + len, &len);
    clen += len;

    cipher.resize(clen); // Ajustamos tamaño al real
    EVP_CIPHER_CTX_free(ctx); // Liberamos memoria

    // Concatenamos IV + ciphertext para poder descifrar luego
    vector<unsigned char> out;
    out.insert(out.end(), iv.begin(), iv.end());
    out.insert(out.end(), cipher.begin(), cipher.end());

    return BytesToHex(out); // Retornamos como string hexadecimal
}

// Recibe clave y string hex de IV+cipher, devuelve el mensaje original
string decryptAES(const vector<unsigned char>& key, const string& hexCipher) {
    auto data = HexToBytes(hexCipher); // Convertimos hex a bytes

    // Separamos IV (primeros 16 bytes) y ciphertext (resto)
    vector<unsigned char> iv(data.begin(), data.begin() + 16);
    vector<unsigned char> cipher(data.begin() + 16, data.end());

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new(); // Creamos contexto de descifrado
    EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key.data(), iv.data()); // Inicializamos AES-128-CBC

    vector<unsigned char> plain(cipher.size() + 16); // Reservamos espacio
    int len, plen;

    EVP_DecryptUpdate(ctx, plain.data(), &len, cipher.data(), cipher.size()); // Desciframos bloques
    plen = len;

    // Finalizamos y verificamos padding
    EVP_DecryptFinal_ex(ctx, plain.data() + len, &len);
    plen += len;

    plain.resize(plen); // Ajustamos al tamaño real
    EVP_CIPHER_CTX_free(ctx); // Liberamos contexto

    return string(plain.begin(), plain.end()); // Devolvemos mensaje original
}
/*
int main() {
    string claveInput, mensaje;
    int opcion;

    cout << "Ingrese clave (16 caracteres): ";
    getline(cin, claveInput); // Pedimos la clave al usuario

    vector<unsigned char> key(claveInput.begin(), claveInput.end()); // Convertimos string -> vector de bytes

    if (key.size() != 16) { // Validamos tamaño
        cout << "Clave debe ser 16 bytes" << endl;
        return 1;
    }

    cout << "1. Encriptar\n2. Desencriptar\n> ";
    cin >> opcion;
    cin.ignore(); // Limpiamos buffer de entrada

    if (opcion == 1) {
        cout << "Mensaje a encriptar: ";
        getline(cin, mensaje);
        cout << "Cifrado HEX: " << encryptAES(key, mensaje) << endl; // Llamamos a función encrypt
    }
    else if (opcion == 2) {
        cout << "Texto cifrado HEX: ";
        getline(cin, mensaje);
        cout << "Descifrado: " << decryptAES(key, mensaje) << endl; // Llamamos a función decrypt
    }
    else cout << "Opción inválida\n";

    return 0;
}
*/
