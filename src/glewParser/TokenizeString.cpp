/*The MIT License(MIT)

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
THE SOFTWARE.*/

#include "TokenizeString.h"

//Delimiters
Delimiters::Delimiters()
{
	m_tokens[0] = 0;
	m_tokens[1] = 0;
	m_tokens[2] = 0;
	m_tokens[3] = 0;
}

Delimiters::Delimiters(const char* delimiterList, int delimiterCount)
{
	m_tokens[0] = 0;
	m_tokens[1] = 0;
	m_tokens[2] = 0;
	m_tokens[3] = 0;

	for (int i = 0; i < delimiterCount; ++i)
	{
		addToken(delimiterList[i]);
	}
}


void Delimiters::addToken(char token)
{
	//divide by 64
	int offset = token >> 6;
	int offsetedChar = token - (offset << 6);
	m_tokens[offset] |= (1ll << offsetedChar);
}

void Delimiters::removeToken(char token)
{
	//divide by 64
	int offset = token >> 6;
	int offsetedChar = token - (offset << 6);
	m_tokens[offset] &= ~(1ll << offsetedChar);
}

bool Delimiters::hasToken(char token) const
{
	int offset = token >> 6;
	int offsetedChar = token - (offset << 6);
	return (m_tokens[offset] & (1ll << offsetedChar)) != 0;
}

void tokenizeString(std::string const& input, Delimiters const& tokens, std::function<void(const std::string& input, std::size_t tokenNumber)> func)
{
	std::string tmp;
	std::size_t tokenCount = 0;

	for (std::size_t i = 0; i < input.size(); ++i)
	{
		if (tokens.hasToken(input[i]))
		{
			if (!tmp.empty())
			{
				func(tmp, tokenCount);
				++tokenCount;
			}

			tmp.clear();
		}
		else
		{
			tmp.push_back(input[i]);
		}
	}

	if (!tmp.empty())
	{
		func(tmp, tokenCount);
	}
}

//void tokenizeString(std::string const& input, Delimiters const& tokens, std::function<TokenizationReturn::ReturnValue(const std::string& input, std::size_t tokenNumber)> func)
//{
//	std::string tmp;
//	std::size_t tokenCount = 0;
//
//	for (std::size_t i = 0; i < input.size(); ++i)
//	{
//		if (tokens.hasToken(input[i]))
//		{
//			if (!tmp.empty())
//			{
//				if (func(tmp, tokenCount) == TokenizationReturn::EARLY_EXIT)
//				{
//					return;
//				}
//
//				++tokenCount;
//			}
//
//			tmp.clear();
//		}
//		else
//		{
//			tmp.push_back(input[i]);
//		}
//	}
//
//	if (!tmp.empty())
//	{
//		func(tmp, tokenCount);
//	}
//}

