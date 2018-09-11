#pragma once

#include <core/core.h>

bool IsAbsolutePath(const std::string &filename);
std::string AbsolutePath(const std::string &filename);
std::string ResolveFilename(const std::string &filename);
std::string DirectoryContaining(const std::string &filename);
void SetSearchDirectory(const std::string& dirname);
