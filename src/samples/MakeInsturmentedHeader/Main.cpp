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

#include <iostream>
#include <fstream>

#include <glewParser/WriteInsturmentedHeader.h>
#include <glewParser/FunctionDatabase.h>

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage:\n" << std::endl;
		std::cout << "\tMakeInsturmentedHeader [options] inputHeader" << std::endl;
		std::cout << "\n\n" << std::endl;
		std::cout << "\t\t [options]\n" << std::endl;
		std::cout << "\t\t -w add wglew defines\n" << std::endl;
		std::cout << "\t\t -o [outputHeader] adds the filename for the output, default is instGlew.h\n" << std::endl;
		return -1;
	}

	FunctionDatabase db;
	db.addApiFunctionPointerToken("GLAPIENTRY");
	db.addGLDefineToken("GLEW_GET_FUN");
	db.addFuncExportToken("GLEW_FUN_EXPORT");

	WriteInsturmentedHeader writer;
	writer.setIncludeGuard("OPLO_INSTURMENTED_GLEW_H");
	writer.addInclude("<gl/glew.h>");

	std::string outputFile("initGlew.h");
	std::string inputFile;

	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-w") == 0)
		{
			writer.addInclude("<gl/wglew.h>");
			db.addApiFunctionPointerToken("WINAPI");
			db.addGLDefineToken("WGLEW_GET_FUN");
			db.addFuncExportToken("WGLEW_FUN_EXPORT");
		}
		else if (strcmp(argv[i], "-o") == 0)
		{
			outputFile = argv[++i];
		}
		else
		{
			inputFile = argv[i];
		}
	}

	std::cout << "Input: " << inputFile << std::endl;
	std::cout << "Output: " << outputFile << std::endl;

	std::ifstream in(inputFile);
	std::ofstream out(outputFile);

	if (!in.is_open())
	{
		std::cout << "Bad input file: " << inputFile << std::endl;
		return -1;
	}

	if (!out.is_open())
	{
		std::cout << "Bad output file: " << outputFile << std::endl;
		return -1;
	}

	do
	{
		std::string line;
		std::getline(in, line);
		db.classify(line);
	} while (!in.eof());

	writer.write(out, db);

	return - 1;
}