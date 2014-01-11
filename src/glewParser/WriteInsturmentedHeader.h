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

#ifndef OUTPUT_PROCESSING_H
#define OUTPUT_PROCESSING_H

#include <functional>
#include <memory>
#include <regex>
#include <unordered_set>
#include <string>

class FunctionDatabase;
struct FunctionSignature;

//Convert function database to insturmented header
class WriteInsturmentedHeader
{
public:

	//Callback to create pre/post function calls
	typedef std::function<void(std::ostream& stream, std::shared_ptr<const FunctionSignature>)> Callback;

	WriteInsturmentedHeader();

	void write(std::ostream& out, const FunctionDatabase& db);

	//specify the include guard on the header
	void setIncludeGuard(const char* guard);

	/// Do not process function
	void addFunctionToIgnoreList(const char* function);

	//associate regular expression with a set of callbacks
	void registerFunctionRegex(
		const char* regExpression,
		Callback preCb,
		Callback postCb);

	//set default callbacks
	void registerDefaultCallback(
		Callback preCb,
		Callback postCb);

	//add an include, e.g., <gl/glew.h>, <gl/wglew.h>, etc.
	void addInclude(const char* include);

private:

	struct InternalData
	{
		InternalData(const std::regex& reg, Callback pre, Callback post) :
			m_regex(reg),
			m_pre(pre),
			m_post(post)
		{}

		std::regex m_regex;
		Callback m_pre;
		Callback m_post;
	};

	Callback m_defaultPreCb;

	Callback m_defaultPostCb;

	std::vector<InternalData> m_internalData;

	std::unordered_set<std::string> m_ignore;


	std::string m_includeGuard;
	std::vector<std::string> m_includes;

};


#endif
