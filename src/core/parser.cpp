/*
    pbrt source code is Copyright(c) 1998-2016
                        Matt Pharr, Greg Humphreys, and Wenzel Jakob.
    This file is part of pbrt.
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:
    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "parser.h"
#include <vector>
#include <iostream>
#ifdef HAVE_MMAP
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>  // Windows file mapping API
#endif
#include <core/fileutil.h>
#include <glog/logging.h>

Loc *parserLoc = nullptr;

std::unique_ptr<Tokenizer> Tokenizer::CreateFromFile(
		const std::string& filename,
		std::function<void(const char*)> errorCallback)
{
	if (filename == "-")
	{
		std::string str;
		int ch;
		while ((ch = getchar()) != EOF) str.push_back((char)ch);
		// std::make_unique...
		return std::unique_ptr<Tokenizer>(
				new Tokenizer(std::move(str), std::move(errorCallback)));
	}

#ifdef HAVE_MMAP
	int fd = open(filename.c_str(), O_RDONLY);
	if (fd == -1)
	{
		errorCallback("");
		return nullptr;
	}

	struct stat stat;
	if (fstat(fd, &stat) != 0)
	{
		errorCallback("");
		return nullptr;
	}

	size_t len = stat.st_size;
	void *ptr = mmap(0, len, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
	if (close(fd) != 0)
	{
		errorCallback("");
		return nullptr;
	}

    // return std::make_unique<Tokenizer>(ptr, len);
    return std::unique_ptr<Tokenizer>(
        new Tokenizer(ptr, len, filename, std::move(errorCallback)));
#else
#endif
}

Tokenizer::Tokenizer(std::string str,
                     std::function<void(const char *)> errorCallback)
    : loc("<stdin>"),
      errorCallback(std::move(errorCallback)),
      contents(std::move(str)) {
    pos = contents.data();
    end = pos + contents.size();
    // tokenizerMemory += contents.size();
}

#if defined(HAVE_MMAP) || defined(_WIN32)
Tokenizer::Tokenizer(void *ptr, size_t len, std::string filename,
                     std::function<void(const char *)> errorCallback)
    : loc(filename),
      errorCallback(std::move(errorCallback)),
      unmapPtr(ptr),
      unmapLength(len) {
    pos = (const char *)ptr;
    end = pos + len;
}
#endif

Tokenizer::~Tokenizer() {
    if (unmapPtr && unmapLength > 0)
        if (munmap(unmapPtr, unmapLength) != 0)
            errorCallback("");
}

string_view Tokenizer::Next() {
	return {};
}

constexpr int TokenOptional = 0;
constexpr int TokenRequired = 1;

enum
{
    PARAM_TYPE_INT,
    PARAM_TYPE_BOOL,
    PARAM_TYPE_FLOAT,
    PARAM_TYPE_POINT2,
    PARAM_TYPE_VECTOR2,
    PARAM_TYPE_POINT3,
    PARAM_TYPE_VECTOR3,
    PARAM_TYPE_NORMAL,
    PARAM_TYPE_RGB,
    PARAM_TYPE_XYZ,
    PARAM_TYPE_BLACKBODY,
    PARAM_TYPE_SPECTRUM,
    PARAM_TYPE_STRING,
    PARAM_TYPE_TEXTURE
};

// Parseing Global Interface
static void parse(std::unique_ptr<Tokenizer> t)
{
	std::vector<std::unique_ptr<Tokenizer>> fileStack;
	fileStack.push_back(std::move(t));
	parserLoc = &fileStack.back()->loc;

	bool ungetTokenSet = false;
	std::string ungetTokenValue;

    // nextToken is a little helper function that handles the file stack,
    // returning the next token from the current file until reaching EOF,
    // at which point it switches to the next file (if any).
    std::function<string_view(int)> nextToken;
    nextToken = [&](int flags) -> string_view {
        if (ungetTokenSet) {
            ungetTokenSet = false;
            return string_view(ungetTokenValue.data(), ungetTokenValue.size());
        }

        if (fileStack.empty()) {
            if (flags & TokenRequired) {
                printf("premature EOF");
                exit(1);
            }
            parserLoc = nullptr;
            return {};
        }

        string_view tok = fileStack.back()->Next();

		if (tok.empty()) 
		{
			// We've reached EOF in the current file. Anything more to parse?
			fileStack.pop_back();
			if (!fileStack.empty()) parserLoc = &fileStack.back()->loc;
			return nextToken(flags);
		}
		else if (tok == "Include")
		{
			return tok; // temp
		}
		else
		{
			// Regular token; success.
			return tok;
		}
	};
	auto ungetToken = [&](string_view s) {
		CHECK(!ungetTokenSet);
		ungetTokenValue = std::string(s.data(), s.size());
		ungetTokenSet = true;
	};

    // MemoryArena arena;

    auto syntaxError = [&](string_view tok) {
        // Error("Unexpected token: %s", toString(tok).c_str());
        exit(1);
    };

	while (true)
	{
	}
}

std::unique_ptr<Tokenizer> Tokenizer::CreateFromString(
    std::string str, std::function<void(const char *)> errorCallback) 
{
    // return std::make_unique<Tokenizer>(std::move(str));
    return std::unique_ptr<Tokenizer>(
        new Tokenizer(std::move(str), std::move(errorCallback)));
}

void parseFile(std::string filename)
{
	if (filename != "-") SetSearchDirectory(DirectoryContaining(filename));

	auto tokError = [](const char* msg) { 
		std::cerr << msg << std::endl;
		exit(1);
	};
	std::unique_ptr<Tokenizer> t = 
		Tokenizer::CreateFromFile(filename, tokError);
	if (!t) return;
	parse(std::move(t));
}
