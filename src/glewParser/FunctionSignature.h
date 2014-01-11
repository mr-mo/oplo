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

#ifndef OPLO_FUNCTIONSIGNATURE_H
#define OPLO_FUNCTIONSIGNATURE_H

#include <vector>
#include <string>
#include <cassert>

// Stringified opengl function
struct FunctionSignature
{
	FunctionSignature();

	//The entire function signature is resolved in two parts
	//Resolve order is:
	//	Function typedef
	//	glName -> function pointer
	//  Function typedef -> function name
	//Parsing is done in a single pass and then combined
	void combine(const FunctionSignature& other);

	//gl function, e.g., glTexSubImage2d
	std::string m_glName;

	//glew function typedef, e.g., PFNGLTEXSUBIMAGE2D
	std::string m_functionTypedef;

	//glew function pointer, e.g., __glewTexSubImage2D
	std::string m_functionPointer;

	//return type for the function
	std::string m_returnType;

	//argument types
	std::vector<std::string> m_argumentTypes;

	//argument names, if unnamed, will be called Undefined_{i}
	std::vector<std::string> m_argumentNames;

	//For internal use, used to order the functions according to order in glew header
	std::size_t m_index;

	//assign utility for combine()

private:

	template<typename T>
	void assignWithTest(T& var, const T& otherVar)
	{
		if (var.empty())
		{
			var = otherVar;
		}
		else if (!otherVar.empty())
		{
			assert(var == otherVar);
		}
	}
};

#endif

