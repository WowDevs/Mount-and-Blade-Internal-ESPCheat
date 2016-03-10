#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "stdafx.h"
#include "Utility.h"
#include "Directx9Hack.h"
#include "Player.h"
#include "MainPlayer.h"
#include <detours.h>
#include <windows.h>
#include <vector>
#include <iostream>
#include <d3dx9.h>
#include <d3d9.h>
#include <math.h>
#include <string>   
#include <iomanip> 
#include <sstream>
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

LPDIRECT3DTEXTURE9 red;
D3DLOCKED_RECT d3dlr;
Directx9Hack directx9Hack;
LPD3DXFONT pFont;
int number;
bool color = true, chams = true;
DWORD playerAddy;
DWORD playerRetrn;
MainPlayer mainPlayer;
LPDIRECT3DVERTEXBUFFER9 pStreamData;
std::vector<Player*> cPlayerBase;
DWORD playerPointer;
D3DVIEWPORT9 Viewport;
bool trigger = false;
bool secondcheck=true;
LPD3DXFONT m_font;
int array[4][4] = { {0,4,8,12},{16,20,24,28},{32,36,40,44},{48,52,56,60} };
DWORD viewMatrixAddress = 0x00A63470; 
DWORD viewMatrixAddress2 = 0xDD9A4C;
DWORD testreturn;
int inumber=0;
int sum = -1;
float offsets[3]{ 100,100,1 };
D3DXVECTOR3* goldenKey = new D3DXVECTOR3(offsets);

D3DXMATRIX* newViewMatrix = new D3DXMATRIX();
typedef HRESULT(WINAPI* tEndScene)(LPDIRECT3DDEVICE9 pDevice);
tEndScene oEndScene;

typedef HRESULT(WINAPI* tDIP)(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT Base, UINT Min, UINT Num, UINT Start, UINT Prim);
tDIP oDIP;

typedef HRESULT(WINAPI* tD3DXMatrixMultiply)(_Inout_ D3DXMATRIX *pOut, _In_ D3DXMATRIX *pM1, _In_ D3DXMATRIX *pM2);
tD3DXMatrixMultiply oD3DXMatrixMultiply;

typedef HRESULT(WINAPI* tPresent)(const RECT *pSourceRect,const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion);
tPresent oPresent;

typedef DWORD(__thiscall* HookWorldToScreen)(DWORD this_ptr, DWORD arg1, D3DXVECTOR3* arg2, DWORD arg3);

void printMatrix(D3DXMATRIX* matrix)
{
	std::cout << std::endl;
	for (size_t t = 0; t < 4; t++)
	{
		for (size_t i = 0; i < 4; i++)
		{
			std::cout << "	" << std::fixed << matrix->m[t][i];
		}

		std::cout << std::endl;
	}
}

bool ContainsPlayer(Player* player)
{
	for (size_t i = 0; i < cPlayerBase.size(); i++)
	{
		if (cPlayerBase[i]->address == player->address)
			return true;
	}
	return false;
}

void SavingPlayer(UINT32 playerPointer)
{
	Player* player = new Player(playerPointer);

	if (!ContainsPlayer(player) && player->active == 1 && player->health > 0 && playerPointer != (UINT32)mainPlayer.pointer)
	{
		cPlayerBase.push_back(player);
	}
}

void CheckDataStorage()
{
	for (int i = 0; i < cPlayerBase.size(); i++)
	{
		cPlayerBase[i]->checkDataPlayer();

		if (cPlayerBase[i]->active == 0 || cPlayerBase[i]->health <= 0)
		{
			cPlayerBase.erase(cPlayerBase.begin() + i);
		}
	}

}

__declspec (naked) void ourFunc()
{
	__asm
	{
		sub esp, 8
		fst dword ptr[esi + 0x6000]
			mov playerPointer, esi
			PUSHAD
	}

	SavingPlayer(playerPointer);
	/*trigger = false;*/

	__asm {
		POPAD
			jmp  playerRetrn
	}
}

void RenderNames(LPDIRECT3DDEVICE9 pDevice)
{
	if (cPlayerBase.size() != NULL)
	{
		for (int i = 0; i < cPlayerBase.size(); i++)
		{
			if (cPlayerBase[i]->vec2Dpoint[2] < 1 && mainPlayer.team != cPlayerBase[i]->team) {
				RECT rec;
				rec.right = cPlayerBase[i]->vec2Dpoint[0];
				rec.bottom = cPlayerBase[i]->vec2Dpoint[1];
				rec.left = rec.right - 20;
				rec.top = rec.bottom  - 20;

				//std::cout << " Size " << cPlayerBase[i]->team << "\n";
				D3DCOLOR fontColor = D3DCOLOR_ARGB(255, 0, 0, 255);
				std::stringstream stream;
				stream << cPlayerBase[i]->health;

				m_font->DrawText(NULL, stream.str().c_str(), -1, &rec, 0, fontColor);
				/*Sleep(1000);*/
			}
		}
	}
}

HRESULT WINAPI hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	pDevice->GetViewport(&Viewport);

	/*if (!secondcheck) {
		secondcheck = true;
	}

	if (secondcheck) {
		trigger = false;
		secondcheck = false;
	}*/

	//if (color)
	//{
	//	directx9Hack.GenerateTexture(pDevice, &red, D3DCOLOR_ARGB(255, 255, 0, 0));
	//	color = false;
	//}
	//
	//	RenderNames(pDevice);
	//	ModelInfo.clear();
	////
	//
	//	if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
	//		if (g_Index != g_Vector.size() - 1)
	//		{
	//			g_Index++;
	//			g_SelectedAddress = g_Vector[g_Index];
	//		}
	//	}
	//	
	//	if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
	//		if (g_Index >= 0)
	//		{
	//			g_Index--;
	//			g_SelectedAddress = g_Vector[g_Index];
	//			if (g_Index == -1)
	//				g_SelectedAddress = NULL;
	//		}
	//	}
	//
	//
	//	/*if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
	//		iStride++;
	//		printf("%i stride, %i base", iStride, iBaseTex);
	//	}
	//
	//if (GetAsyncKeyState(VK_NUMPAD2) & 1)
	//		iStride--;*/

	D3DXCreateFont(pDevice, 15, 0, FW_DONTCARE, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_font);
	mainPlayer.CheckPlayer();
	CheckDataStorage();
	RenderNames(pDevice);
	m_font->Release();
	return oEndScene(pDevice);
}

D3DXMATRIX* viewMatrix(D3DXMATRIX *pM1)
{
	newViewMatrix->_11 = *(float*)viewMatrixAddress2;
	newViewMatrix->_12 = *(float*)(viewMatrixAddress2+0x10);
	newViewMatrix->_13 = *(float*)(viewMatrixAddress2 + 0x20);
	newViewMatrix->_14 = 0;

	newViewMatrix->_21 = *(float*)(viewMatrixAddress2 + 0x4);
	newViewMatrix->_22 = *(float*)(viewMatrixAddress2 + 0x14);
	newViewMatrix->_23 = *(float*)(viewMatrixAddress2 + 0x24);
	newViewMatrix->_24 = 0;

	newViewMatrix->_31 = 0;
	newViewMatrix->_32 = *(float*)(viewMatrixAddress2 + 0x18);
	newViewMatrix->_33 = *(float*)(viewMatrixAddress2 + 0x28);
	newViewMatrix->_34 = 0;

	newViewMatrix->_41 = pM1->_41;
	newViewMatrix->_42 = pM1->_42;
	newViewMatrix->_43 = pM1->_43;
	newViewMatrix->_44 = 1.0f;
	printf("Game matrix ");
	printMatrix(pM1);
	printf("My matrix ");
	printMatrix(newViewMatrix);
	
	return pM1;
}

HRESULT WINAPI hkD3DXMatrixMultiply(_Inout_ D3DXMATRIX *pOut, _In_ D3DXMATRIX *pM1, _In_ D3DXMATRIX *pM2)
{
	
if (!trigger && GetAsyncKeyState(VK_NUMPAD1) & 1)
	{

	D3DXVECTOR3 vector2;
	D3DXMATRIX WorldToLocal;
	D3DXMatrixIdentity(&WorldToLocal);

		for (int i = 0; i < cPlayerBase.size(); i++)
		{
			D3DXVECTOR3 vector3(cPlayerBase[i]->vec[0], cPlayerBase[i]->vec[1], cPlayerBase[i]->vec[2]);

			D3DXVec3Project(&vector2, &vector3, &Viewport, pM2, viewMatrix(pM1), &WorldToLocal);
			cPlayerBase[i]->vec2Dpoint[0] = vector2.x;
			cPlayerBase[i]->vec2Dpoint[1] = vector2.y;
			cPlayerBase[i]->vec2Dpoint[2] = vector2.z;

			std::cout << "	X on screen " << vector2.x << " Y on screen "  << vector2.y << std::endl;

			if (i == cPlayerBase.size() - 1)
				trigger = true;
		}
	}

	return oD3DXMatrixMultiply(pOut, pM1, pM2);
}

//HRESULT WINAPI hkDIP(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT Base, UINT Min, UINT Num, UINT Start, UINT Prim)
//{
//	LPDIRECT3DVERTEXBUFFER9 streamData;
//	UINT offset = 0;
//	UINT stride = 0;
//
//	if (pDevice->GetStreamSource
//		(0, &streamData, &offset, &stride) == S_OK)
//		streamData->Release();
//
//	if (!chams)
//	{
//		if (64 == stride && number == 0)
//		{
//			pDevice->SetRenderState(D3DRS_ZENABLE, false);
//
//			for (size_t i = 0; i < 10; i++)
//			{
//				pDevice->SetTexture(i, red);
//			}
//
//			number = 1;
//			pDevice->DrawIndexedPrimitive(Type, Base, Min, Num, Start, Prim);
//		}
//
//		if (64 == stride && number == 1)
//		{
//			number = 0;
//		}
//	}
//	return oDIP(pDevice, Type, Base, Min, Num, Start, Prim);
//}

HRESULT WINAPI hkPresent( const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
	trigger = false;
	
	/*HookWorldToScreen hookWorldToScreen = reinterpret_cast<HookWorldToScreen>(0x0434CA0);
	DWORD a = hookWorldToScreen(0x8E3120, 0xA633F0, goldenKey,0x0);
	std::cout << " " << std::hex << a;
*/
	return oPresent(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

void JmpPatch(void *pDest, void *pSrc, int nNops = 0)
{
	DWORD OldProt;

	VirtualProtect(pSrc, 5 + nNops, PAGE_EXECUTE_READWRITE, &OldProt);

	*(char*)pSrc = (char)0xE9;
	*(DWORD*)((DWORD)pSrc + 1) = (DWORD)pDest - (DWORD)pSrc - 5;

	for (int i = 0; i < nNops; ++i) { *(BYTE*)((DWORD)pSrc + 5 + i) = 0x90; }
	VirtualProtect(pSrc, 5 + nNops, OldProt, &OldProt);
}

DWORD WINAPI HookD3D(LPVOID lpParameter)
{
	Utility utility;
	DWORD dwDXDevice = utility.FindPattern((DWORD)GetModuleHandle("d3d9.dll"), 0x128000, (PBYTE)"\xC7\x06\x00\x00\x00\x00\x89\x86\x00\x00\x00\x00\x89\x86", "xx????xx????xx");
	DWORD* pdwVTable;
	memcpy(&pdwVTable, (VOID*)(dwDXDevice + 2), 4);

	playerAddy = 0x49E451;
	playerRetrn = 0x49E451 + 9;

	JmpPatch((PVOID)ourFunc, (PVOID)playerAddy);
	oEndScene = (tEndScene)DetourFunction((PBYTE)pdwVTable[42], (PBYTE)hkEndScene);
	oPresent = (tPresent)DetourFunction((PBYTE)pdwVTable[17], (PBYTE)hkPresent);

	DWORD D3DXMatrixMultiply = (DWORD)GetProcAddress(GetModuleHandleA("d3dx9_42.dll"), "D3DXMatrixMultiply");
	oD3DXMatrixMultiply = (tD3DXMatrixMultiply)DetourFunction((PBYTE)D3DXMatrixMultiply, (PBYTE)hkD3DXMatrixMultiply);

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

		printf("> Threads Active.\n\n");
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		Sleep(500);
	}

	return TRUE;
}