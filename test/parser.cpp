// Use code from pbrt 'parser.cpp'

#include <vector>
#include <string>
#include <gtest/gtest.h>
#include <core/parser.h>

static std::vector<std::string> extract(Tokenizer *t)
{
	std::vector<std::string> tokens;
	while (true)
	{
		string_view s = t->Next();
		if (s.empty())
			return tokens;
		tokens.push_back(std::string(s.data(), s.size()));
	}
}

void checkTokens(Tokenizer* t, std::initializer_list<std::string> expected)
{
	std::vector<std::string> tokens = extract(t);
	auto iter = expected.begin();
	for (const std::string& s : tokens)
	{
		EXPECT_TRUE(iter != expected.end());
		EXPECT_EQ(*iter, s);
		++iter;
	}
	EXPECT_TRUE(iter == expected.end());
}

TEST(Parser, TokenizerBasics)
{
	std::vector<std::string> errors;
	auto err = [&](const char* err) { errors.push_back(err); };

	{
		auto t = Tokenizer::CreateFromString("Shape \"sphere\" \"float radius\" [1]", err);
		ASSERT_TRUE(t.get() != nullptr);
		checkTokens(t.get(), {"Shape", "\"sphere\"", "\"float radius\"", "[", "1", "]"});
	}
}
