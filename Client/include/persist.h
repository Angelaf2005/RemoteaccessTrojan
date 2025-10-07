#ifndef PERSIST_H
#define PERSIST_H

#include <string>
#include <filesystem>
std::filesystem::path getAppDataPath();
bool fileExist(const std::filesystem::path& file_path);
std::filesystem::path getExecutablePath();
bool persistence(const std::wstring& valueName, const std::wstring& path);
bool deletePersistence(const std::wstring& valueName);
bool deletePath(const std::filesystem::path& target);
bool directoryExist(const std::filesystem::path& dir_path);
bool checkPersistence(const std::wstring& valueName);
bool createConfigFile(const std::filesystem::path& file_path);
bool copyExe(const std::filesystem::path& sourceFile, const std::filesystem::path& destFolder);
void goodBye();

#endif