#include <windows.h>
#include <iostream>
#include <string>

// MODULE NOT TESTED

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