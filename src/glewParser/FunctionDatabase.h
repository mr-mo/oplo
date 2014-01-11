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

#ifndef OPLO_FUNCTIONDATABASE_H
#define OPLO_FUNCTIONDATABASE_H

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

// 
struct FunctionSignature;

/// Parse lines to create a database of all known gl function signatures
/// Data will be stored in order of occurance
/// Parsing routines are pretty rigid, but sufficient for glew headers
class FunctionDatabase
{
public:

	/// Iterate over database
	struct Iterator
	{
		Iterator(const FunctionDatabase& db);

		std::shared_ptr<const FunctionSignature> next();
		std::shared_ptr<const FunctionSignature> get() const;
	private:

		Iterator& operator=(const Iterator& rhs);
		Iterator(const Iterator& rhs);

		std::size_t m_currentIndex;
		const FunctionDatabase& m_db;
	};

	FunctionDatabase()
	{}

	//Turn a line into a function signature
	void classify(std::string const& line);

	// Token to decide on what's a function pointer typedef
	// E.g., GLAPIENTRY, WGLAPIENTRY, GLXAPIENTRY, etc.
	void addApiFunctionPointerToken(const std::string& apiDelimiter);

	// Token to decide what's an exported function
	// e.g., GLEW_FUN_EXPORT, WGLEW_FUN_EXPORT, etc. 
	void addFuncExportToken(const std::string& funcExportDelimiter);

	// Token to figure out if a #define is a gl function, e.g., GLEW_GET_FUNC
	void addGLDefineToken(const std::string& glDefineDelimiter);

private:

	bool handleAsApiFunctionPointer(std::string const& line);

	bool handleAsFuncExport(std::string const& line);

	bool handleAsGLDefine(std::string const& line);

	/// Get (and possibly allocate if not found) the function signature from the typedef
	std::shared_ptr<FunctionSignature> getFromFunctionTypedef(std::string const& funcTypedef, bool allocate);

	/// Get (and possibly allocate if not found) the function signature from the __glew function pointer
	std::shared_ptr<FunctionSignature> getFromFunctionPointer(std::string const& funcPointer, bool allocate);

	/// Get (and possibly allocate if not found) from either
	std::shared_ptr<FunctionSignature> getFromEither(std::string const& funcTypedef, std::string const& funcPointer, bool allocate);

	//result is token[0] + ' ' + ... + ' ' + token[n]
	void concatenateTokens(
		std::vector<std::string> const& tokens,
		std::string& result,
		std::size_t begin,
		std::size_t end);

private:

	std::size_t totalItems() const;

	std::shared_ptr<const FunctionSignature> getData(std::size_t) const;

private:

	//Database
	typedef std::unordered_map< std::string, std::shared_ptr<FunctionSignature> > ParamToSigMap;

	ParamToSigMap m_typedefToSig;
	ParamToSigMap m_pointerToSig;

	std::vector< int > m_spareIndices;
	std::vector < std::shared_ptr<FunctionSignature> >	 m_data;

	//Line classifiers
	std::vector<std::string> m_apiFunctionPointerDelimiters;
	std::vector<std::string> m_funcExportDelimiters;
	std::vector<std::string> m_glFuncDefineDelimiters;

	//Known types for parsing function signatures
	std::vector<std::string> m_primaryTypes; //Basic types, e.g., int, HDC, whatever
	std::vector<std::string> m_likelyTypes; //Likely types to be a type for a function argument, GLint, etc.
};

#endif

