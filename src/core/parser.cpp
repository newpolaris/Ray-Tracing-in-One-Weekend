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
#ifdef HAVE_MMAP
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#elif defined(IS_WINDOW)
#include <windows.h>  // Windows file mapping API
#endif
#include <core/error.h>
#include <core/api.h>
#include <core/fileutil.h>
#include <core/spectrum.h>
#include <core/paramset.h>
#include <tools/MemoryArena.h>
#include <algorithm>

Loc *parserLoc = nullptr;

static std::string toString(string_view s) {
    return std::string(s.data(), s.size());
}

static char decodeEscaped(int ch) {
    switch (ch) {
    case EOF:
        Error("premature EOF after character escape '\\'");
        exit(1);
    case 'b':
        return '\b';
    case 'f':
        return '\f';
    case 'n':
        return '\n';
    case 'r':
        return '\r';
    case 't':
        return '\t';
    case '\\':
        return '\\';
    case '\'':
        return '\'';
    case '\"':
        return '\"';
    default:
        Error("unexpected escaped character \"%c\"", ch);
        exit(1);
    }
    return 0;  // NOTREACHED
}

inline bool isQuotedString(string_view str) {
    return str.size() >= 2 && str[0] == '"' && str.back() == '"';
}

static double parseNumber(string_view str) {
    // Fast path for a single digit
    if (str.size() == 1) {
    }

    double val;
    return val;
}

static string_view dequoteString(string_view str) {
    if (!isQuotedString(str)) {
        Error("\"%s\": expected quoted string", toString(str).c_str());
        exit(1);
    }

    str.remove_prefix(1);
    str.remove_suffix(1);
    return str;
}

struct ParamListItem {
    std::string name;
    double *doubleValues = nullptr;
    const char **stringValues = nullptr;
    size_t size = 0;
    bool isString = false;
};

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

static bool lookupType(const std::string &decl, int *type, std::string &sname) {
    *type = 0;
    // Skip leading space
    auto skipSpace = [&decl](std::string::const_iterator iter) {
        while (iter != decl.end() && (*iter == ' ' || *iter == '\t')) ++iter;
        return iter;
    };
    // Skip to the next whitespace character (or the end of the string).
    auto skipToSpace = [&decl](std::string::const_iterator iter) {
        while (iter != decl.end() && *iter != ' ' && *iter != '\t') ++iter;
        return iter;
    };

    auto typeBegin = skipSpace(decl.begin());
    if (typeBegin == decl.end()) {
        Error("Parameter \"%s\" doesn't have a type declaration?!", decl.c_str());
        return false;
    }

    // Find end of type declaration
    auto typeEnd = skipToSpace(typeBegin);

    string_view typeStr(&(*typeBegin), size_t(typeEnd - typeBegin));
    if (typeStr == "float")
        *type = PARAM_TYPE_FLOAT;
    else {
        return false;
    }

    auto nameBegin = skipSpace(typeEnd);
    if (nameBegin == decl.end()) {
        Error("Unable to find parameter name from \"%s\"", decl.c_str());
        return false;
    }
    auto nameEnd = skipToSpace(nameBegin);
    sname = std::string(nameBegin, nameEnd);

    return true;
}

static void AddParam(ParamSet &ps, const ParamListItem &item,
                     SpectrumType SpectrumType) {
    int type;
    std::string name;
    if (lookupType(item.name, &type, name)) {

    }
}

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
#elif defined(IS_WINDOW)
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

#if defined(HAVE_MMAP) || defined(IS_WINDOW)
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
#if 0
    if (unmapPtr && unmapLength > 0)
        if (munmap(unmapPtr, unmapLength) != 0)
            errorCallback("");
#endif
}

string_view Tokenizer::Next() {
	while (true) 
	{
		const char* tokenStart = pos;
		int ch = getChar();
		if (ch == EOF)
			return {};
		else if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r')
		{
			// nothing
		}
		else if (ch == '"')
		{ 
			bool haveEscaped = false;
			while ((ch = getChar()) != '"') {
				if (ch == EOF) {
					errorCallback("premature EOF");
					return {};
				} else if (ch == '\n') {
					errorCallback("unterminated string");
					return {};
				} else if (ch == '\\') {
					haveEscaped = true;
					// Grab the next character
					if ((ch = getChar()) == EOF) {
						errorCallback("premature EOF");
						return {};
					}
				}
			}

			if (!haveEscaped)
				return {tokenStart, size_t(pos - tokenStart)};
			else {
				sEscaped.clear();
				for (const char *p = tokenStart; p < pos; ++p) {
					if (*p != '\\')
						sEscaped.push_back(*p);
					else {
						++p;
						CHECK_LT(p, pos);
						sEscaped.push_back(decodeEscaped(*p));
					}
				}
				return {sEscaped.data(), sEscaped.size()};
			}
		}
		else if (ch == '[' || ch == ']') 
		{
			return {tokenStart, size_t(1)};
		}
		else if (ch == '#')
		{
			// comment: scan to EOL (or EOF)
			while ((ch = getChar()) != EOF)
			{
				if (ch == '\n' || ch == '\r')
				{
					ungetChar();
					break;
				}
			}
			return {tokenStart, size_t(pos - tokenStart)};
		}
		else
		{
			// Regular statement or numeric token; scan until we hit a
			// space, opening quote, or bracket.
			while ((ch = getChar()) != EOF) {
				if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r' ||
						ch == '"' || ch == '[' || ch == ']') {
					ungetChar();
					break;
				}
			}
			return {tokenStart, size_t(pos - tokenStart)};
		}
	}
	return {};
}

template <typename Next, typename Unget>
ParamSet parseParams(Next nextToken, Unget ungetToken, MemoryArena &arena,
                     SpectrumType spectrumType) {
    ParamSet ps;
    while (true) {
        string_view decl = nextToken(TokenOptional);
        if (decl.empty()) return ps;

        if (!isQuotedString(decl)) {
            ungetToken(decl);
            return ps;
        }

        ParamListItem item;
        item.name = toString(dequoteString(decl));
        size_t nAlloc = 0;

        auto addVal = [&](string_view val) {
            if (isQuotedString(val)) {
                if (item.doubleValues) {
                    exit(1);
                }

                if (item.size == nAlloc) {
                    nAlloc = std::max<size_t>(2 * item.size, 4);
                    const char **newData = arena.allocate<const char*>(nAlloc);
                    std::copy(item.stringValues, item.stringValues + item.size,
                              newData);
                    item.stringValues = newData;
                }

                val = dequoteString(val);
                char *buf = arena.allocate<char>(val.size() + 1);
                memcpy(buf, val.data(), val.size());
                buf[val.size()] = '\0';
                item.stringValues[item.size++] = buf;
            } else {
                if (item.stringValues) {
                    exit(1);
                }

                if (item.size == nAlloc) {
                    nAlloc = std::max<size_t>(2 * item.size, 4);
                    double *newData = arena.allocate<double>(nAlloc);
                    std::copy(item.doubleValues, item.doubleValues + item.size,
                              newData);
                    item.doubleValues = newData;
                }
                item.doubleValues[item.size++] = parseNumber(val);
            }
        };
        string_view val = nextToken(TokenRequired);

        if (val == "[") {
            while (true) {
                val = nextToken(TokenRequired);
                if (val == "]") break;
                addVal(val);
            }
        } else {
            addVal(val);
        }

        AddParam(ps, item, spectrumType);
        arena.clear();
    }

    return ps;
}

extern int catIndentCount;

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
		else if (tok[0] == '#')
		{
            // Swallow comments, unless --cat or --toply was given, in
            // which case they're printed to stdout.
            if (PbrtOptions.cat || PbrtOptions.toPly)
                printf("%*s%s\n", catIndentCount, "", toString(tok).c_str());
            return nextToken(flags);
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

    MemoryArena arena;

    // Helper function for pbrt API entrypoints that take a single string
    // parameter and a ParamSet (e.g. pbrtShape()).
    auto basicParamListEntrypoint = [&](
        SpectrumType spectrumType,
        std::function<void(const std::string &n, ParamSet p)> apiFunc)
    {
        string_view token = nextToken(TokenRequired);
        string_view dequoted = dequoteString(token);
        std::string n = toString(dequoted);
        ParamSet params =
            parseParams(nextToken, ungetToken, arena, spectrumType);
        apiFunc(n, std::move(params));
    };


    auto syntaxError = [&](string_view tok) {
        Error("Unexpected token: %s", toString(tok).c_str());
        exit(1);
    };

	while (true)
	{
		string_view tok = nextToken(TokenOptional);
		if (tok.empty()) break;

		switch (tok[0]) {
		case 'A':
			if (tok == "AttributeBegin")
				;
			else if (tok == "Accelerator")
				basicParamListEntrypoint(SpectrumType::Reflectance,
										 pbrtAccelerator);
            break;
        case 'I':
            if (tok == "Integrator")
                ;
            else if (tok == "Include") {
                // Switch to the given file.
                std::string filename =
                    toString(dequoteString(nextToken(TokenRequired)));
                if (PbrtOptions.cat || PbrtOptions.toPly)
                    printf("%*sInclude \"%s\"\n", catIndentCount, "", filename.c_str());
                else {
                    filename = AbsolutePath(ResolveFilename(filename));
                    auto tokError = [](const char *msg) { Error("%s", msg); };
                    std::unique_ptr<Tokenizer> tinc =
                        Tokenizer::CreateFromFile(filename, tokError);
                    if (tinc) {
                        fileStack.push_back(std::move(tinc));
                        parserLoc = &fileStack.back()->loc;
                    }
                }
            }
            break;
		}
	}
}

std::unique_ptr<Tokenizer> Tokenizer::CreateFromString(
    std::string str, std::function<void(const char *)> errorCallback) 
{
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
