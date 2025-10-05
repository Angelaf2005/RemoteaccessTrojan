#ifndef PERSIST_H
#define PERSIST_H

#include <string>

bool persistence(const std::wstring& valueName, const std::wstring& path);
bool deletePersistence(const std::wstring& valueName);
bool checkPersistence(const std::wstring& valueName);

#endif