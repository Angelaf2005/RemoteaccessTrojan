#include <windows.h>
#include <iostream>
#include <string>
//Desactivar 
//Variables importantes
std::string g_exeDir;     // carpeta donde está el ejecutable (p. Descargas\RemoteaccessTrojan)
std::string g_folderName; // nombre de la carpeta 
std::string g_appdata;    // valor de %APPDATA% (p. ej. "C:\\Users\\User\\AppData\\Roaming")
std::string g_dst;        // carpeta destino final (p. ej. "%APPDATA%\\RemoteaccessTrojan")

// Función que copia recursivamente una carpeta 
bool copyDirectory(const std::string& src, const std::string& dst) {
    WIN32_FIND_DATAA ffd;
    std::string searchPath = src + "\\*";
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &ffd);

    if (hFind == INVALID_HANDLE_VALUE) return false;

    // Crear carpeta destino si no existe
    CreateDirectoryA(dst.c_str(), NULL);

    do {
        std::string name = ffd.cFileName;

        if (name == "." || name == "..") continue;

        std::string srcPath = src + "\\" + name;
        std::string dstPath = dst + "\\" + name;

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Si es carpeta: recursión
            copyDirectory(srcPath, dstPath);
        }
        else {
            // Si es archivo: lo copia
            CopyFileA(srcPath.c_str(), dstPath.c_str(), FALSE);
        }
    } while (FindNextFileA(hFind, &ffd) != 0);

    FindClose(hFind);
    return true;
}

// Inicializa las variables globales
bool initPaths() {
    // Obtener ruta del exe
    char exePath[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, exePath, MAX_PATH);
    if (len == 0) {
        std::cerr << "Error: no se pudo obtener la ruta del exe.\n";
        return false;
    }

    // Obtener carpeta donde está el exe (C:\... \RemoteAccessTrojan)
    std::string exeDir(exePath);
    size_t pos = exeDir.find_last_of("\\/");
    if (pos == std::string::npos) {
        std::cerr << "Error: ruta del exe inválida.\n";
        return false;
    }
    exeDir = exeDir.substr(0, pos);

    // Nombre de la carpeta
    pos = exeDir.find_last_of("\\/");
    if (pos == std::string::npos) {
        // Si no hay separador, entonces exeDir mismo es el nombre
        g_folderName = exeDir;
    }
    else {
        g_folderName = exeDir.substr(pos + 1);
    }

    // Obtener %APPDATA%
    const char* appdata_env = std::getenv("APPDATA");
    if (!appdata_env) {
        std::cerr << "Error: APPDATA no encontrada.\n";
        return false;
    }

    // Guardar en variables globales
    g_exeDir = exeDir;
    g_appdata = std::string(appdata_env);
    g_dst = g_appdata + "\\" + g_folderName;

    return true;
}

int main() {
    // Inicializamos rutas globales
    if (!initPaths()) return 1;

    // Mostrar rutas
    std::cout << "Carpeta origen: " << g_exeDir << "\n";
    std::cout << "APPDATA: " << g_appdata << "\n";
    std::cout << "Destino: " << g_dst << "\n";

    // Copiar carpeta completa usando las variables globales
    if (copyDirectory(g_exeDir, g_dst)) {
        std::cout << "Carpeta copiada a: " << g_dst << "\n";
    }
    else {
        std::cerr << "Error al copiar la carpeta\n";
        return 1;
    }

    return 0;
}
