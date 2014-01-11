/*
The MIT License(MIT)

Copyright(c) 2014 Erik Scorelle

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef OPLO_TOKENIZE_STRING_H
#define OPLO_TOKENIZE_STRING_H

#include <string>
#include <functional>

//Bitfield for chars
class Delimiters
{
public:

	Delimiters();

	Delimiters(const char* delimiterList, int delimiterCount);

	void addToken(char token);

	void removeToken(char token);

	bool hasToken(char token) const;

private:
	unsigned long long m_tokens[4];
};

struct TokenizationReturn
{
	enum ReturnValue
	{
		CONTINUE,
		EARLY_EXIT
	};
};

//O(N) complexity on input
//void tokenizeString(std::string const& input, Delimiters const& tokens, std::function<TokenizationReturn::ReturnValue(const std::string& input, std::size_t tokenNumber)> tokenizationFunc);
void tokenizeString(std::string const& input, Delimiters const& tokens, std::function<void(const std::string& input, std::size_t tokenNumber)> tokenizationFunc);

#endif
