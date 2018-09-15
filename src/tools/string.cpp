#include <tools/string.h>
#include <algorithm>

namespace util
{
    std::string GetFileExtension(const std::string& filename)
    {
        return filename.substr(filename.find_last_of(".") + 1);
    }

    bool stricmp(const std::string& str1, const std::string& str2) {
        std::string str1Cpy(str1);
        std::string str2Cpy(str2);
        std::transform(str1Cpy.begin(), str1Cpy.end(), str1Cpy.begin(), ::tolower);
        std::transform(str2Cpy.begin(), str2Cpy.end(), str2Cpy.begin(), ::tolower);
        return (str1Cpy == str2Cpy);
    }

    std::string ReplaceAll(std::string str, const std::string &from, const std::string &to)
    {
        size_t pos = 0;
        while ((pos = str.find(from, pos)) != std::string::npos)
        {
            str.replace(pos, from.length(), to);
            pos += to.length();
        }
        return str;
    }
}
