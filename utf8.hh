#pragma once
#include <string>

size_t columnLen(const std::string&);
std::string substr(const std::string&, int, int);
void erase(std::string&, int, int);
void removeChar(std::string&, int);
void appendChar(std::string&, int, const char*);
