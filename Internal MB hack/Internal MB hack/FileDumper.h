#pragma once

#include <fstream>
#include <exception>
#include <d3dx9.h>
#include <d3d9.h>
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

class FileDumper
{
public:
	FileDumper(char* file);
	void DumpMatrix(D3DXMATRIX* matrix,int numbers);
	~FileDumper();
};

