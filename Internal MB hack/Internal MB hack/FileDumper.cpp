#include "stdafx.h"
#include "FileDumper.h"
#include <iostream>

std::ofstream* oLogFile;
char myCharPointer[25];
char myCharPointer2[100];

FileDumper::FileDumper(char* file)
{
	oLogFile = new std::ofstream(file, std::ios::app);
}


void FileDumper::DumpMatrix(D3DXMATRIX* matrix, int numbers)
{
	/*sprintf_s(myCharPointer2, "%i ViewProjectionWorldMatrix \n", numbers);*/
	//oLogFile->write(myCharPointer2,100);

	try {
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t t = 0; t < 4; t++)
			{
				std::cout << "\t" << matrix->m[t][i];
			}

			std::cout << "\n";
		}
		Sleep(150);
	}
	catch (int e)
	{

	}
}
FileDumper::~FileDumper()
{
}
