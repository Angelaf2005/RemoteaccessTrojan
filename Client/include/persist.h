#ifndef PERSIST_H
#define PERSIST_H

#include <string>
std::filesystem::path getAppDataPath();
bool fileExist(const std::filesystem::path& file_path);
bool persistence(const std::wstring& valueName, const std::wstring& path);
bool deletePersistence(const std::wstring& valueName);
bool checkPersistence(const std::wstring& valueName);
bool createConfigFile(const std::filesystem::path& file_path);
bool copyExe(const std::filesystem::path& sourceFile, const std::filesystem::path& destFolder);

#endif