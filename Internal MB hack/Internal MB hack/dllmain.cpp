#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "stdafx.h"
#include "Utility.h"
#include "Directx9Hack.h"
#include "FileDumper.h"
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
#pragma intrinsic(_ReturnAddress)

using namespace std;

LPDIRECT3DTEXTURE9 pTx,green,red;
FileDumper* fileDumper;
D3DLOCKED_RECT d3dlr;
Directx9Hack directx9Hack;
D3DVIEWPORT9 vpt;
LPD3DXFONT pFont;
void* g_SelectedAddress = NULL;
UINT iBaseTex;
char strbuff[260];
int number;
bool a=true, color=true, chams=true;
UINT iStride;
int x;
int y;
int g_Index = -1;
int numbers;
D3DXMATRIX* viewmatrix, projmatrix, result;
struct ModelInfo_t
{
	int X, Y, Team;
	char* Name;
};

std::vector<ModelInfo_t>ModelInfo;

typedef HRESULT(WINAPI* tEndScene)(LPDIRECT3DDEVICE9 pDevice);
tEndScene oEndScene;

typedef HRESULT(WINAPI* tDIP)(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT Base, UINT Min, UINT Num, UINT Start, UINT Prim);
tDIP oDIP;

std::vector<void*>                    g_Vector;

struct SD3DVertex {
	float x, y, z, rhw;
	DWORD colour;
};

void Circle(LPDIRECT3DDEVICE9 pDevice, int x, int y, int radius, int points, D3DCOLOR colour) {
	SD3DVertex* pVertex = new SD3DVertex[points + 1];
	for (int i = 0; i <= points; i++) pVertex[i] = { x + radius * cos(D3DX_PI * (i / (points / 2.0f))), y - radius * sin(D3DX_PI * (i / (points / 2.0f))), 0.0f, 1.0f, colour };
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, points, pVertex, sizeof(SD3DVertex));
	delete[] pVertex;
}

void RenderNames(LPDIRECT3DDEVICE9 pDevice)
{
	int numbers = 0;
	if (ModelInfo.size() != NULL)
	{
		for (int i = 0; i < ModelInfo.size(); i++)
		{
			D3DRECT rec;
			rec.x1 = ModelInfo[i].X;
			rec.y1 = ModelInfo[i].Y;
			rec.x2 = ModelInfo[i].X + 5;
			rec.y2 = ModelInfo[i].Y + 5;

			pDevice->Clear(1, &rec, D3DCLEAR_TARGET, D3DCOLOR_ARGB(232, 100, 145, 30), 0, 0);
		}
	}
	else { return; }
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

	RenderNames(pDevice);
	ModelInfo.clear();
//

	if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
		if (g_Index != g_Vector.size() - 1)
		{
			g_Index++;
			g_SelectedAddress = g_Vector[g_Index];
		}
	}
	
	if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
		if (g_Index >= 0)
		{
			g_Index--;
			g_SelectedAddress = g_Vector[g_Index];
			if (g_Index == -1)
				g_SelectedAddress = NULL;
		}
	}


	/*if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
		iStride++;
		printf("%i stride, %i base", iStride, iBaseTex);
	}

if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		iStride--;*/

	return oEndScene(pDevice);
}

bool IsAddressPresent(void* Address)
{
	for (auto it = g_Vector.begin(); it != g_Vector.end(); ++it)
	{
		if (*it == Address)
			return true;
	}
	return false;
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
		if (64 == stride && number == 0)
		{
			pDevice->SetRenderState(D3DRS_ZENABLE, false);

			for (size_t i = 0; i < 10; i++)
			{
				pDevice->SetTexture(i, red);
			}
			D3DVIEWPORT9 Viewport;
			D3DXMATRIX ViewProjectionMatrix, LocalToWorld, WorldToLocal;
			D3DXVECTOR3 Vector3D(0, 0, 0), Vector2D;
			pDevice->GetVertexShaderConstantF(163, ViewProjectionMatrix, 4);
			pDevice->GetVertexShaderConstantF(174, LocalToWorld, 1);
			pDevice->GetViewport(&Viewport);
			/*fileDumper->DumpMatrix(ViewProjectionMatrix, numbers);*/
			D3DXMatrixIdentity(&WorldToLocal);
			D3DXVec3Project(&Vector2D, &Vector3D, &Viewport, &ViewProjectionMatrix, &LocalToWorld, &WorldToLocal);

			cout << ViewProjectionMatrix._11 << endl;
			Sleep(200);
			/*if (Vector2D.z < 1.0f)
			{*/
			ModelInfo_t pModelInfo = { static_cast<int>(Vector2D.x),static_cast<int>(Vector2D.y),0,""};
			ModelInfo.push_back(pModelInfo);

			/*}
			*/
			a = false;
			number = 1;
			pDevice->DrawIndexedPrimitive(Type, Base, Min, Num, Start, Prim);
		}

		if (64 == stride && number == 1)
		{	
			number = 0;
		}

	}

	return oDIP(pDevice, Type, Base, Min, Num, Start, Prim);
}

DWORD WINAPI HookD3D(LPVOID lpParameter)
{
	fileDumper = new FileDumper("Matrix.txt");
	Utility utility;
	DWORD dwDXDevice = utility.FindPattern((DWORD)GetModuleHandle("d3d9.dll"), 0x128000, (PBYTE)"\xC7\x06\x00\x00\x00\x00\x89\x86\x00\x00\x00\x00\x89\x86", "xx????xx????xx");
	DWORD* pdwVTable;
	memcpy(&pdwVTable, (VOID*)(dwDXDevice + 2), 4);

	/*oEndScene = (tEndScene)DetourFunction((PBYTE)pdwVTable[42], (PBYTE)hkEndScene);
	oDIP = (tDIP)DetourFunction((PBYTE)pdwVTable[82], (PBYTE)hkDIP);
*/
	viewmatrix = (D3DXMATRIX*) 0xC7BFB18;
	//projmatrix = *(D3DXMATRIX*) 0xC3EFB18;

	/*fileDumper->DumpMatrix(viewmatrix,1);
	fileDumper->DumpMatrix(projmatrix,2);*/
	float notthis = viewmatrix->_11;
	while (true) {
		if (viewmatrix->_11 > -1 && viewmatrix->_11 < 1 && viewmatrix->_11 != notthis)
			/*fileDumper->DumpMatrix(viewmatrix,1);*/
			std::cout << "\t" << viewmatrix->_11;
		//std::cout << "\t" << viewmatrix->_12 << endl;
	}

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