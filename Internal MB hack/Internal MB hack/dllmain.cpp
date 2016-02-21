#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "stdafx.h"
#include "Utility.h"
#include "Directx9Hack.h"
#include <detours.h>
#include <windows.h>
#include <vector>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <d3dx9.h>
#include <d3d9.h>
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

using namespace std;

LPDIRECT3DTEXTURE9 pTx,green,red;
D3DLOCKED_RECT d3dlr;
Directx9Hack directx9Hack;
D3DVIEWPORT9 vpt;
LPD3DXFONT pFont;
UINT iBaseTex;
char strbuff[260];
int number;
bool a, color, chams;

typedef HRESULT(WINAPI* tEndScene)(LPDIRECT3DDEVICE9 pDevice);
tEndScene oEndScene;

typedef HRESULT(WINAPI* tDIP)(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT Base, UINT Min, UINT Num, UINT Start, UINT Prim);
tDIP oDIP;

void printMatrix(D3DXMATRIX matrix)
{
	if (a) {
		printf("ViewProjectionWorldMatrix \n");
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t t = 0; t < 4; t++)
			{
				cout << "\t" << matrix.m[t][i];
			}
			printf("\n");
		}
		printf("\n");
	}
}

HRESULT WINAPI hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	if (color)
	{
		directx9Hack.GenerateTexture(pDevice, &red, D3DCOLOR_ARGB(255, 255, 0, 0));

		color = false;
	}

	if (GetAsyncKeyState(VK_F1) & 1)
	{
		chams = !chams;
	}

	pDevice->GetViewport(&vpt);

	RECT FRect = { vpt.Width - 250,vpt.Height - 300,
		vpt.Width,vpt.Height };

	if (green == NULL)
		if (pDevice->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT, &green, NULL) == S_OK)
			if (pDevice->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8,
				D3DPOOL_SYSTEMMEM, &pTx, NULL) == S_OK)
				if (pTx->LockRect(0, &d3dlr, 0, D3DLOCK_DONOTWAIT |
					D3DLOCK_NOSYSLOCK) == S_OK)
				{
					for (UINT xy = 0; xy < 8 * 8; xy++)
						((PDWORD)d3dlr.pBits)[xy] = 0xFF00FF00;

					pTx->UnlockRect(0);
					pDevice->UpdateTexture(pTx, green);
					pTx->Release();
				}

	if (pFont == NULL)
		D3DXCreateFontA(pDevice, 16, 0, 700, 0, 0, 1, 0,
			0, DEFAULT_PITCH | FF_DONTCARE, "Calibri", &pFont);

	sprintf(strbuff, "", iBaseTex + 1);

if (pFont)
	pFont->DrawTextA(0, strbuff, -1, &FRect,
		DT_CENTER | DT_NOCLIP, 0xFF00FF00);

if (GetAsyncKeyState(VK_NUMPAD3) & 1)
	iBaseTex++;

if (GetAsyncKeyState(VK_NUMPAD4) & 1)
	if (iBaseTex > 0)
		iBaseTex--;

	return oEndScene(pDevice);
}

HRESULT WINAPI hkDIP(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT Base, UINT Min, UINT Num, UINT Start, UINT Prim)
{
	LPDIRECT3DVERTEXBUFFER9 streamData;
	UINT offset = 0;
	UINT stride = 0;

	if (pDevice->GetStreamSource
		(0, &streamData, &offset, &stride) == S_OK)
		streamData->Release();
	
	if (!chams)
	{
		if (68 == stride && number == 0)
		{
			pDevice->SetRenderState(D3DRS_ZENABLE, false);

			for (size_t i = 0; i < 10; i++)
			{
				pDevice->SetTexture(i, red);
			}

			number = 1;

			D3DXMATRIX viewProjectionMatrix,world;

			pDevice->GetVertexShaderConstantF(158, viewProjectionMatrix, 4);
			pDevice->GetVertexShaderConstantF(177, world, 1);
		
			printMatrix(viewProjectionMatrix);
			printMatrix(viewProjectionMatrix);
			a = false;
			pDevice->DrawIndexedPrimitive(Type, Base, Min, Num, Start, Prim);
		}

		if (68 == stride && number == 1)
		{
			number = 0;
		}
	}

	return oDIP(pDevice, Type, Base, Min, Num, Start, Prim);
}

DWORD WINAPI HookD3D(LPVOID lpParameter)
{
	Utility utility;
	DWORD dwDXDevice = utility.FindPattern((DWORD)GetModuleHandle("d3d9.dll"), 0x128000, (PBYTE)"\xC7\x06\x00\x00\x00\x00\x89\x86\x00\x00\x00\x00\x89\x86", "xx????xx????xx");
	DWORD* pdwVTable;
	memcpy(&pdwVTable, (VOID*)(dwDXDevice + 2), 4);

	oEndScene = (tEndScene)DetourFunction((PBYTE)pdwVTable[42], (PBYTE)hkEndScene);
	oDIP = (tDIP)DetourFunction((PBYTE)pdwVTable[82], (PBYTE)hkDIP);

	return 0;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		AllocConsole();

		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);


		DisableThreadLibraryCalls(GetModuleHandle((LPCSTR)NULL));
		CreateThread((LPSECURITY_ATTRIBUTES)NULL, 0, HookD3D, NULL, 0, 0);

		printf("> Threads Active.\n");
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		Sleep(500);
	}

	return TRUE;
}