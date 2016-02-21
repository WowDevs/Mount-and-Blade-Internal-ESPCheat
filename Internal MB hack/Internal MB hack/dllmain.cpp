#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "stdafx.h"
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

LPDIRECT3DVERTEXBUFFER9 pStreamData;
vector<DWORD> BASETEX;
LPDIRECT3DBASETEXTURE9 BTEX = NULL;
D3DVIEWPORT9 viewPort;
D3DVIEWPORT9 Vpt;
LPDIRECT3DTEXTURE9 pTx = NULL;
D3DLOCKED_RECT d3dlr;
LPDIRECT3DTEXTURE9 Green = NULL;
LPD3DXFONT pFont = NULL;
UINT iStride = 0;
UINT iBaseTex = 0;
int number = 0;
char strbuff[260];
bool bTex = false, bCham = false;
LPDIRECT3DTEXTURE9 cFront, cBack;
DWORD* pdwVTable;
bool color = true;
bool chams = false;
bool test = false;

struct ModelInfo_t
{
	int X, Y, Team;
	char* Name;
};

std::vector<ModelInfo_t>ModelInfo;

void AddModel(IDirect3DDevice9* Device, char* oName, int oTeam)
{
	D3DVIEWPORT9 Viewport;
	D3DXMATRIX ViewProjectionMatrix, LocalToWorld, WorldToLocal;
	D3DXVECTOR3 Vector3D(0, 0, 0), Vector2D;
	Device->GetVertexShaderConstantF(7, ViewProjectionMatrix, 1);//change this to match your game matrix
	Device->GetVertexShaderConstantF(8, LocalToWorld, 1);//change this to match your game matrix
	Device->GetViewport(&Viewport);
	D3DXMatrixIdentity(&WorldToLocal);
	D3DXVec3Project(&Vector2D, &Vector3D, &Viewport, &ViewProjectionMatrix, &LocalToWorld, &WorldToLocal);
	if (Vector2D.z < 1.0f)
	{
		ModelInfo_t pModelInfo = { static_cast<int>(Vector2D.x),static_cast<int>(Vector2D.y),oTeam,oName };
		ModelInfo.push_back(pModelInfo);
	}
}

HRESULT GenerateTexture(LPDIRECT3DDEVICE9 pDevice, IDirect3DTexture9 **ppD3Dtex, DWORD colour32)
{
	if (FAILED(pDevice->CreateTexture(8, 8, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, ppD3Dtex, NULL)))
		return E_FAIL;

	WORD colour16 = ((WORD)((colour32 >> 28) & 0xF) << 12)
		| (WORD)(((colour32 >> 20) & 0xF) << 8)
		| (WORD)(((colour32 >> 12) & 0xF) << 4)
		| (WORD)(((colour32 >> 4) & 0xF) << 0);

	D3DLOCKED_RECT d3dlr;
	(*ppD3Dtex)->LockRect(0, &d3dlr, 0, 0);
	WORD *pDst16 = (WORD*)d3dlr.pBits;

	for (int xy = 0; xy < 8 * 8; xy++)
		*pDst16++ = colour16;

	(*ppD3Dtex)->UnlockRect(0);

	return S_OK;
}

void doDisassembleShader(LPDIRECT3DDEVICE9 pDevice, char* FileName)
{
	ofstream oLogFile(FileName, ios::trunc);

	if (!oLogFile.is_open())
		return;

	IDirect3DVertexShader9* pShader;

	pDevice->GetVertexShader(&pShader);

	UINT pSizeOfData;

	pShader->GetFunction(NULL, &pSizeOfData);

	BYTE* pData = new BYTE[pSizeOfData];

	pShader->GetFunction(pData, &pSizeOfData);

	LPD3DXBUFFER bOut;

	D3DXDisassembleShader(reinterpret_cast<DWORD*>(pData), (BOOL)NULL, (LPCSTR)NULL, &bOut);

	oLogFile << static_cast<char*>(bOut->GetBufferPointer()) << std::endl;
	oLogFile.close();

	delete[] pData;

	pShader->Release();
}

typedef HRESULT(WINAPI* tEndScene)(LPDIRECT3DDEVICE9 pDevice);
tEndScene oEndScene;

typedef HRESULT(WINAPI* tDIP)(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT Base, UINT Min, UINT Num, UINT Start, UINT Prim);
tDIP oDIP;

typedef HRESULT(WINAPI* tReset)(DWORD RenderTargetIndex, IDirect3DSurface9 *pRenderTarget);
tReset oReset;

HRESULT WINAPI hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	//if (color)
	//{
	//	GenerateTexture(pDevice, &cBack, D3DCOLOR_ARGB(255, 255, 0, 0));
	//	GenerateTexture(pDevice, &cFront, D3DCOLOR_ARGB(0, 255, 255, 0));
	//	color = false;
	//}

	//if (GetAsyncKeyState(VK_F1) & 1) // if we click f1
	//{
	//	chams = !chams;
	//}

	//pDevice->GetViewport(&Vpt);

	//RECT FRect = { Vpt.Width - 250,Vpt.Height - 300,
	//	Vpt.Width,Vpt.Height };

	//if (Green == NULL)
	//	if (pDevice->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8,
	//		D3DPOOL_DEFAULT, &Green, NULL) == S_OK)
	//		if (pDevice->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8,
	//			D3DPOOL_SYSTEMMEM, &pTx, NULL) == S_OK)
	//			if (pTx->LockRect(0, &d3dlr, 0, D3DLOCK_DONOTWAIT |
	//				D3DLOCK_NOSYSLOCK) == S_OK)
	//			{
	//				for (UINT xy = 0; xy < 8 * 8; xy++)
	//					((PDWORD)d3dlr.pBits)[xy] = 0xFF00FF00;

	//				pTx->UnlockRect(0);
	//				pDevice->UpdateTexture(pTx, Green);
	//				pTx->Release();
	//			}

	//if (pFont == NULL)
	//	D3DXCreateFontA(pDevice, 16, 0, 700, 0, 0, 1, 0,
	//		0, DEFAULT_PITCH | FF_DONTCARE, "Calibri", &pFont);

	//sprintf(strbuff, "Num of Textures: %i\nStride: %i\nBase Tex Num: %i\n\nSTRIDE LOGGER V1\n\n" \
		//	"Log Enable: %i\n\nNUM1: Stride++\nNUM2: Stride--\nNUM3: BaseTexNum++" \
	//	"\nNUM4: BaseTexNum--\nNUM0: Log On/Off", \
	//	BASETEX.size(), iStride, iBaseTex + 1);

//if (pFont)
//	pFont->DrawTextA(0, strbuff, -1, &FRect,
//		DT_CENTER | DT_NOCLIP, 0xFF00FF00);

//if (GetAsyncKeyState(VK_NUMPAD1) & 1)
//{
//	iStride++; BASETEX.clear(); iBaseTex = 0;
//}

//if (GetAsyncKeyState(VK_NUMPAD2) & 1)
//	if (iStride > 0)
//	{
//		iStride--; BASETEX.clear(); iBaseTex = 0;
//	};

//if (GetAsyncKeyState(VK_NUMPAD3) & 1)
//	if (iBaseTex < BASETEX.size() - 1)iBaseTex++;

//if (GetAsyncKeyState(VK_NUMPAD4) & 1)
//	if (iBaseTex > 0)
//		iBaseTex--;
	printf("hooked");

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
	__asm
	{
		int 3
	}
	if (chams)
	{
		if (iStride == stride && number == 0)
		{
			pDevice->SetRenderState(D3DRS_ZENABLE, false);

			for (size_t i = 0; i < 10; i++)
			{
				pDevice->SetTexture(i, cBack);
			}

			number = 1;

			doDisassembleShader(pDevice, "shader.txt");
			D3DXMATRIX projection, view, world, ID;

			pDevice->DrawIndexedPrimitive(Type, Base, Min, Num, Start, Prim);
		}

		if (iStride == stride && number == 1)
		{
			number = 0;

			///*for (size_t i = 0; i < 10; i++)
			//{
			//	pDevice->SetTexture(i, cFront);
			//}*/
			//
			///*pDevice->SetRenderState(D3DRS_ZENABLE, true);*/
			/////*pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_NEVER);*/
			//pDevice->DrawIndexedPrimitive(Type, Base, Min, Num, Start, Prim);
			//
		}
	}

	return oDIP(pDevice, Type, Base, Min, Num, Start, Prim);
}

HRESULT WINAPI hkReset(DWORD RenderTargetIndex, IDirect3DSurface9 *pRenderTarget)
{


	return hkReset(RenderTargetIndex, pRenderTarget);
}

bool bCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask == 'x' && *pData != *bMask)
			return false;
	return (*szMask) == NULL;
}

DWORD FindPattern(DWORD dValor, DWORD dLer, BYTE *bMaskara, char * szMaskara)
{
	for (DWORD i = 0; i < dLer; i++)
		if (bCompare((PBYTE)(dValor + i), bMaskara, szMaskara))
			return (DWORD)(dValor + i);
	return false;
}

DWORD WINAPI HookD3D(LPVOID lpParameter)
{
	DWORD dwDXDevice = FindPattern((DWORD)GetModuleHandle("d3d9.dll"), 0x128000, (PBYTE)"\xC7\x06\x00\x00\x00\x00\x89\x86\x00\x00\x00\x00\x89\x86", "xx????xx????xx");
	memcpy(&pdwVTable, (VOID*)(dwDXDevice + 2), 4);
	cout << pdwVTable[82];
	/*oEndScene = (tEndScene)DetourFunction((PBYTE)pdwVTable[42], (PBYTE)hkEndScene);*/
	/*oDIP = (tDIP)DetourFunction((PBYTE)pdwVTable[82], (PBYTE)hkDIP);*/
	/*oReset = (tReset)DetourFunction((PBYTE)pdwVTable[17], (PBYTE)hkReset);*/

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