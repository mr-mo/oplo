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

#include "WriteInsturmentedHeader.h"
#include "FunctionDatabase.h"
#include "FunctionSignature.h"

WriteInsturmentedHeader::WriteInsturmentedHeader() :
	m_defaultPreCb([](std::ostream&, std::shared_ptr<const FunctionSignature>){}),
	m_defaultPostCb([](std::ostream&, std::shared_ptr<const FunctionSignature>){})
{}

void WriteInsturmentedHeader::write(std::ostream& out, const FunctionDatabase& db)
{
	out << "#ifndef " << m_includeGuard << "\n";
	out << "#define " << m_includeGuard << "\n\n";

	for (int i = 0; i < m_includes.size(); ++i)
	{
		out << "#include " << m_includes[i] << "\n";
	}

	FunctionDatabase::Iterator itr(db);

	std::shared_ptr<const FunctionSignature> data = itr.get();

	while (data)
	{
		if (m_ignore.find(data->m_glName) == m_ignore.end())
		{
			out << "#ifdef " << data->m_glName << "\n";
			out << "#undef " << data->m_glName << "\n";

			out << "inline " <<
				data->m_returnType << " " << data->m_glName << "(";

			if (!data->m_argumentTypes.empty())
			{
				for (int i = 0; i < data->m_argumentTypes.size() - 1; ++i)
				{
					out << data->m_argumentTypes[i] << " " << data->m_argumentNames[i] << ", ";
				}

				if (!data->m_argumentTypes.empty())
				{
					out << data->m_argumentTypes.back() << " " << data->m_argumentNames.back();
				}
			}

			out << ")\n";

			out << "{\n";

			int regIdx = -1;

			for (int reg = 0; reg < m_internalData.size(); ++reg)
			{
				if (std::regex_match(data->m_glName.c_str(), m_internalData[reg].m_regex))
				{
					regIdx = reg;
					break;
				}
			}

			if (regIdx >= 0)
			{
				m_internalData[regIdx].m_pre(out, data);
			}
			else
			{
				m_defaultPreCb(out, data);
			}

			out << data->m_functionPointer << "(";

			if (!data->m_argumentTypes.empty())
			{
				for (int i = 0; i < data->m_argumentTypes.size() - 1; ++i)
				{
					out << data->m_argumentNames[i] << ", ";
				}

				if (!data->m_argumentTypes.empty())
				{
					out << data->m_argumentNames.back();
				}
			}

			out << ");\n";

			if (regIdx >= 0)
			{
				m_internalData[regIdx].m_post(out, data);
			}
			else
			{
				m_defaultPostCb(out, data);
			}

			out << "}\n\n";

		}

		data = itr.next();
	}

	out << "#endif\n";
}

void WriteInsturmentedHeader::setIncludeGuard(const char* guard)
{
	m_includeGuard = guard;
}

/// Do not process function
void WriteInsturmentedHeader::addFunctionToIgnoreList(const char* function)
{
	m_ignore.insert(function);
}

void WriteInsturmentedHeader::registerFunctionRegex(
	const char* regExpression,
	Callback preCb,
	Callback postCb)
{
	m_internalData.push_back(InternalData(std::regex(regExpression), preCb, postCb));
}

void WriteInsturmentedHeader::registerDefaultCallback(
	Callback preCb,
	Callback postCb)
{
	m_defaultPreCb = preCb;
	m_defaultPostCb = postCb;
}

void WriteInsturmentedHeader::addInclude(const char* include)
{
	m_includes.push_back(include);
}