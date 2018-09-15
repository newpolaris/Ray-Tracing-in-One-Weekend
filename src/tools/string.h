#pragma once

#include <string>

namespace util
{
    std::string GetFileExtension(const std::string& filename);
    std::string ReplaceAll(std::string str, const std::string &from, const std::string &to);
    bool stricmp(const std::string& str1, const std::string& str2);
}
