#pragma once
#include "stdafx.h"
#include <fstream>
#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")

class Directx9Hack
{
public:
	void GenerateTexture(LPDIRECT3DDEVICE9 pDevice, IDirect3DTexture9 **ppD3Dtex, DWORD colour32);
	void doDisassembleShader(LPDIRECT3DDEVICE9 pDevice, char* FileName);
};

