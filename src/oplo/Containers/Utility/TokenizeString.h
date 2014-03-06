#ifndef OPLO_TOKENIZE_STRING_H
#define OPLO_TOKENIZE_STRING_H

#include <string>
#include <functional>

namespace oplo
{

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

	//O(N) complexity on input
	void TokenizeString(std::string const& input, Delimiters const& tokens, std::function<void(const std::string& input, std::size_t tokenNumber)> tokenizationFunc);
}

#endif
