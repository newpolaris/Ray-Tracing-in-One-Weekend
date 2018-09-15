#pragma once

#include <core/core.h>
#include <string>
#include <tools/string.h>

#ifdef __GNUG__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
#endif  // __GNUG__

template <typename... Args>
std::string StringPrintf(const std::string &fmt, Args&&... args)
{
    std::string format = util::ReplaceAll(fmt, "%f", "%.9g");
    size_t size = snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...);
    std::string str;
    str.resize(size + 1);
    snprintf(&str[0], size + 1, format.c_str(), std::forward<Args>(args)...);
    str.pop_back();
    return str;
}
