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

#include "FunctionDatabase.h"
#include "FunctionSignature.h"
#include "TokenizeString.h"

#include <algorithm>
#include <iostream>
#include <cstdio>

//Iterator
FunctionDatabase::Iterator::Iterator(const FunctionDatabase& db) :
	m_db(db),
	m_currentIndex(0)
{}

std::shared_ptr<const FunctionSignature> FunctionDatabase::Iterator::get() const
{
	if (m_currentIndex >= m_db.totalItems())
	{
		return std::shared_ptr<FunctionSignature>();
	}

	return m_db.getData(m_currentIndex);
}

std::shared_ptr<const FunctionSignature> FunctionDatabase::Iterator::next()
{
	++m_currentIndex;

	while (!get() && m_currentIndex < m_db.totalItems())
	{
		++m_currentIndex;
	}

	return get();
}

//Database

void FunctionDatabase::addApiFunctionPointerToken(const std::string& apiDelimeter)
{
	m_apiFunctionPointerDelimiters.push_back(apiDelimeter);
}

void FunctionDatabase::addFuncExportToken(const std::string& funcExportDelimiter)
{
	m_funcExportDelimiters.push_back(funcExportDelimiter);
}

void FunctionDatabase::addGLDefineToken(const std::string& glDefineDelimit)
{
	m_glFuncDefineDelimiters.push_back(glDefineDelimit);
}

/// This is a bit of a mess
void FunctionDatabase::classify(std::string const& line)
{
	if (handleAsGLDefine(line))
		return;

	if (handleAsFuncExport(line))
		return;

	//Both api function pointers and types have typedefs
	//If there's no 'typedef' just early out
	if (line.find("typedef") == std::string::npos)
		return;

	if (handleAsApiFunctionPointer(line))
		return;


	//else typedef for an argument type, e.g., typedef unsigned int GLuint;
	std::vector<std::string> tokens;
	tokenizeString(line, Delimiters(" ;", 2), [&tokens](const std::string& in, std::size_t) { tokens.push_back(in); });

	assert(tokens.size() > 1);

	std::string primaryType;

	//skip typedef
	std::size_t i = 1;
	std::size_t end = tokens.size() - 1;
	concatenateTokens(tokens, primaryType, i, end);

	/// Typedefed type
	m_primaryTypes.push_back(primaryType);
	m_likelyTypes.push_back(tokens.back());
}

bool FunctionDatabase::handleAsApiFunctionPointer(std::string const& line)
{
	//typedef void (GLAPIENTRY * PFNGLGETVIDEOI64VNVPROC) (GLuint video_slot, GLenum pname, GLint64EXT* params);

	for (int i = 0; i < m_apiFunctionPointerDelimiters.size(); ++i)
	{
		if (line.find(m_apiFunctionPointerDelimiters[i]) != std::string::npos &&
			line.find("#define") == std::string::npos //don't want that pesky #define
			)
		{
			Delimiters space(" ", 1);
			Delimiters comma(",", 1);
			std::size_t validator = 0;

			std::vector<std::string> tokens;
			std::vector<std::string> functionArguments;

			tokenizeString(line, Delimiters("();", 3), [&tokens](const std::string& in, std::size_t){tokens.push_back(in);});

			if (tokens.size() != 4)
			{
				std::cout << "Skipping delimited APIFUNC due to invalid number of tokens: " << line << std::endl;
				continue;
			}

			//arguments + possible argument names
			tokenizeString(tokens[3], comma, [&functionArguments](const std::string& in, std::size_t){functionArguments.push_back(in);});

			//APITYPE * __POINTERNAME__
			//apitype * __pointertype__
			std::string funcTypedefStr;
			tokenizeString(tokens[1], space, [&funcTypedefStr, &validator](const std::string& in, std::size_t i)
				{
					if (i == 2 ) 
						funcTypedefStr = in;
					validator = i;
				}
			);

			assert(validator == 2);

			std::shared_ptr<FunctionSignature> data = getFromFunctionTypedef(funcTypedefStr, true);
		
			std::string returnType;
			//typedef return type
			tokenizeString(tokens[0], space, [&returnType](const std::string& in, std::size_t i)
				{
					//typedef
					if (i > 1)
					{
						if (i > 2)
							returnType.append(" ");
						returnType.append(in);
					}
				}
			);

			data->m_functionTypedef = funcTypedefStr;
			data->m_returnType = returnType;

			for (int arg = 0; arg < functionArguments.size(); ++arg)
			{
				//need to do exhaustive search?
				//done this way instead of with a hashed set in case spacing conventions are different
				for (int argName = 0; argName < m_likelyTypes.size(); ++argName)
				{
					std::string::size_type p = functionArguments[arg].find(m_likelyTypes[argName]);

					if (p != std::string::npos)
					{
						data->m_argumentTypes.push_back(m_likelyTypes[argName]);

						std::string remainingChars = functionArguments[arg].substr(p + m_likelyTypes[argName].size());

						remainingChars.erase(
							std::remove_if(
							remainingChars.begin(),
							remainingChars.end(),
							[](char c){ return c == ' '; }
						), remainingChars.end());

						//no variable name given
						if (remainingChars.empty())
						{
							static char buf[128];
							sprintf(buf, "undefined%i", i);
							data->m_argumentNames.push_back(buf);
						}
						else
						{
							data->m_argumentNames.push_back(remainingChars);
						}

						break;
					}
				}
			}

			return true;
		}
	}

	return false;
}

bool FunctionDatabase::handleAsFuncExport(std::string const& line)
{
	//GLEW_FUN_EXPORT PFNGLGETVIDEOI64VNVPROC __glewGetVideoi64vNV;
	for (int i = 0; i < m_funcExportDelimiters.size(); ++i)
	{
		if (
			line.find(m_funcExportDelimiters[i]) != std::string::npos &&
			line.find("#define") == std::string::npos //don't want that pesky #define
			)
		{
			std::string funcTypedef;
			std::string funcPointer;
			std::size_t validationCount;

			tokenizeString(line, Delimiters(" ;", 2), 
				[&funcTypedef, &funcPointer, &validationCount](const std::string& in, std::size_t i)
				{
					validationCount = i;
					switch (i)
					{
					case 1:
						funcTypedef = in;
					case 2:
						funcPointer = in;
					}
				}
				);


			if (validationCount != 2)
			{
				std::cout << "Skipping delimited FUNCEXPORT due to invalid number of tokens: " << line << std::endl;
				continue;
			}

			std::shared_ptr<FunctionSignature> data = getFromEither(funcTypedef, funcPointer, true);

			data->m_functionTypedef = funcTypedef;
			data->m_functionPointer = funcPointer;

			return true;
		}
	}

	return false;
}

bool FunctionDatabase::handleAsGLDefine(std::string const& line)
{
	//#define glPresentFrameKeyedNV GLEW_GET_FUN(__glewPresentFrameKeyedNV)
	for (int i = 0; i < m_glFuncDefineDelimiters.size(); ++i)
	{
		if ( line.find(m_glFuncDefineDelimiters[i]) != std::string::npos && line.find("#define") != std::string::npos )
		{
			std::string glName;
			std::string funcPointer;
			std::size_t validationCount;

			tokenizeString(line, Delimiters(" ();", 4),
				[&glName, &funcPointer, &validationCount](const std::string& in, std::size_t i)
				{
					validationCount = i;
					switch (i)
					{
					case 1:
						glName = in;
					case 3:
						funcPointer = in;
					}
				}
			);


			if (validationCount != 3)
			{
				std::cout << "Skipping delimited GL_DEFINE due to invalid number of tokens: " << line << std::endl;
				continue;
			}

			//func define's define
			if (glName == m_glFuncDefineDelimiters[i])
			{
				return false;
			}

			std::shared_ptr<FunctionSignature> data = getFromFunctionPointer(funcPointer, true);
			data->m_glName = glName;
			data->m_functionPointer = funcPointer;

			return true;
		}
	}

	return false;
}

void FunctionDatabase::concatenateTokens(
	std::vector<std::string> const& tokens,
	std::string& result,
	std::size_t begin,
	std::size_t end)
{
	std::size_t i = begin;

	//source type
	for (; i < end; ++i)
	{
		if (i != begin)
		{
			result.push_back(' ');
		}

		result.append(tokens[i]);
	}
}



std::shared_ptr<FunctionSignature> FunctionDatabase::getFromFunctionTypedef(std::string const& funcTypedef, bool allocate)
{
	ParamToSigMap::iterator itr = m_typedefToSig.find(funcTypedef);

	if (itr == m_typedefToSig.end())
	{
		if (allocate)
		{
			std::shared_ptr<FunctionSignature> sig(new FunctionSignature);
			sig->m_functionTypedef = funcTypedef;
			sig->m_index = m_data.size();
			m_data.push_back(sig);
			m_typedefToSig.insert(std::make_pair(funcTypedef, sig));
			return sig;
		}

		return std::shared_ptr<FunctionSignature>();
	}

	return itr->second;
}

std::shared_ptr<FunctionSignature> FunctionDatabase::getFromFunctionPointer(std::string const& funcPointer, bool allocate)
{
	ParamToSigMap::iterator itr = m_pointerToSig.find(funcPointer);

	if (itr == m_pointerToSig.end())
	{
		if (allocate)
		{
			std::shared_ptr<FunctionSignature> sig(new FunctionSignature);
			sig->m_functionPointer = funcPointer;
			sig->m_index = m_data.size();
			m_pointerToSig.insert(std::make_pair(funcPointer, sig));
			m_data.push_back(sig);
			return sig;
		}

		return std::shared_ptr<FunctionSignature>();
	}

	return itr->second;
}

std::shared_ptr<FunctionSignature> FunctionDatabase::getFromEither(std::string const& funcTypedef, std::string const& funcPointer, bool allocate)
{
	std::shared_ptr<FunctionSignature> sigTypedef = getFromFunctionTypedef(funcTypedef, false);
	std::shared_ptr<FunctionSignature> sigPointer = getFromFunctionPointer(funcPointer, false);

	//If both are resolved, make sure they resolve to the same pointer
	//If not, combine the disjointed signatures into the minimum index
	if (sigTypedef && sigPointer)
	{
		if (sigTypedef->m_index != sigPointer->m_index)
		{
			std::size_t minIndex = sigTypedef->m_index < sigPointer->m_index ? sigTypedef->m_index : sigPointer->m_index;
			std::size_t maxIndex = sigTypedef->m_index > sigPointer->m_index ? sigTypedef->m_index : sigPointer->m_index;

			m_data[minIndex]->combine(*m_data[maxIndex]);
			m_data[maxIndex] = 0;
		}

		return sigTypedef;
	}

	if (sigTypedef)
	{
		sigTypedef->m_functionPointer = funcPointer;
		m_pointerToSig[funcPointer] = sigTypedef;
		return sigTypedef;
	}

	if (sigPointer)
	{
		sigPointer->m_functionTypedef = funcTypedef;
		m_typedefToSig[funcTypedef] = sigPointer;
		return sigPointer;
	}

	if (allocate)
	{
		sigTypedef.reset(new FunctionSignature);
		sigTypedef->m_functionPointer = funcPointer;
		sigTypedef->m_functionTypedef = funcTypedef;
		sigTypedef->m_index = m_data.size();

		m_pointerToSig[funcPointer] = sigTypedef;
		m_typedefToSig[funcTypedef] = sigTypedef;

		m_data.push_back(sigTypedef);
	}

	return sigTypedef;
}

std::size_t FunctionDatabase::totalItems() const
{
	return m_data.size();
}

std::shared_ptr<const FunctionSignature> FunctionDatabase::getData(std::size_t i) const
{
	return m_data[i];
}
