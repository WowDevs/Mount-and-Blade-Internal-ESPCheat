#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "stdafx.h"
#include "Utility.h"
#include "Directx9Hack.h"
#include "FileDumper.h"
#include "Player.h"
#include "MainPlayer.h"
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
int number;
bool a=true, color=true, chams=true,c=true;
UINT iStride;
int g_Index = -1;
int numbers;
DWORD playerAddy;
DWORD playerRetrn;
MainPlayer mainPlayer;
LPDIRECT3DVERTEXBUFFER9 pStreamData;
vector<Player*> cPlayerBase;
DWORD playerPointer;
D3DXMATRIX* viewmatrix;
D3DXMATRIX* projmatrix;
int numbert = 0;
struct ModelInfo_t
{
	int X, Y, Team;
	char* Name;
};

std::vector<ModelInfo_t> ModelInfo;

typedef HRESULT(WINAPI* tEndScene)(LPDIRECT3DDEVICE9 pDevice);
tEndScene oEndScene;

typedef HRESULT(WINAPI* tDIP)(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT Base, UINT Min, UINT Num, UINT Start, UINT Prim);
tDIP oDIP;

typedef HRESULT(WINAPI* tD3DXMatrixMultiply)( _Inout_ D3DXMATRIX *pOut, _In_ D3DXMATRIX *pM1, _In_ D3DXMATRIX *pM2);
tD3DXMatrixMultiply oD3DXMatrixMultiply;

std::vector<void*> g_Vector;
void printmatrix(char* name,D3DXMATRIX* matrix) 
{
	std::cout << "This is the " << name << "call\n";
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t t = 0; t < 4; t++)
		{
			std::cout << "\t" << matrix->m[t][i];
		}

		std::cout << "\n";
	}
	std::cout << "\n";
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

	if (!ContainsPlayer(player) && player->active == 1 && player->health > 0)
	{
		cPlayerBase.push_back(player);
	}
}

void CheckDataStorage()
{
	for (size_t i = 0; i < cPlayerBase.size(); i++)
	{
		cPlayerBase[i]->checkDataPlayer();
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

	__asm {
		POPAD
			jmp  playerRetrn
	}
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

	mainPlayer.CheckPlayer();
	CheckDataStorage();

	for (int i = 0; i < cPlayerBase.size(); i++)
	{

	D3DVIEWPORT9 Viewport;
	D3DXMATRIX WorldToLocal;
	D3DXVECTOR3 Vector3D(cPlayerBase[i]->vec[0], cPlayerBase[i]->vec[1], cPlayerBase[i]->vec[2]), Vector2D;
	pDevice->GetViewport(&Viewport);
	D3DXMatrixIdentity(&WorldToLocal);

	D3DXVec3Project(&Vector2D, &Vector3D, &Viewport, viewmatrix,projmatrix, &WorldToLocal);
	D3DRECT rec;
	rec.x1 = Vector2D.x;
	rec.y1 = Vector2D.y;
	rec.x2 = Vector2D.x + 5;
	rec.y2 = Vector2D.y+5;

	cout << " X " << cPlayerBase[i]->vec[0] << " Y " << cPlayerBase[i]->vec[1] << " Z " << cPlayerBase[i]->vec[2] << endl;

	pDevice->Clear(1, &rec, D3DCLEAR_TARGET, D3DCOLOR_ARGB(232, 100, 145, 30), 0, 0);
	}
	

	return oEndScene(pDevice);
}

HRESULT WINAPI hkD3DXMatrixMultiply(_Inout_ D3DXMATRIX *pOut, _In_ D3DXMATRIX *pM1, _In_ D3DXMATRIX *pM2)
{
	numbert++;
	//std::cout << "This is the " << numbert << "call\n";
	//for (size_t i = 0; i < 4; i++)
	//{
	//	for (size_t t = 0; t < 4; t++)
	//	{
	//		std::cout << "\t" << pOut->m[t][i];
	//	}

	//	std::cout << "\n";
	//}
	//std::cout << "\n";
	//if (numbert == 2) 
	//{
	//	numbert = 0;
	//}
	switch (numbert) {
	case 1:
		std::cout << "First time called\n\n";
		printmatrix("the first argument (projviewmatrix?)", pOut);
		printmatrix("the second argument (viewmatrix?)", pM1);
		printmatrix("the thirde argument (projectionmatrix?)", pM2);
		break;
	case 2:
		std::cout << "second time called\n\n";
		printmatrix("the first argument (projviewmatrix?)", pOut);
		printmatrix("the second argument (viewmatrix?)", pM1);
		printmatrix("the thirde argument (projectionmatrix?)", pM2);
		break;
	case 3:
		std::cout << "third time called\n\n";
		printmatrix("the first argument (projviewmatrix?)", pOut);
		printmatrix("the second argument (viewmatrix?)", pM1);
		printmatrix("the thirde argument (projectionmatrix?)", pM2);
		break;
	case 4:
		std::cout << "fourth time called\n\n";
		printmatrix("the first argument (projviewmatrix?)", pOut);
		printmatrix("the second argument (viewmatrix?)", pM1);
		printmatrix("the thirde argument (projectionmatrix?)", pM2);
		break;
	case 5:
		std::cout << "5 time called\n\n";
		printmatrix("the first argument (projviewmatrix?)", pOut);
		printmatrix("the second argument (viewmatrix?)", pM1);
		printmatrix("the thirde argument (projectionmatrix?)", pM2);
		break;
	}
	
	return oD3DXMatrixMultiply(pOut, pM1, pM2);
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
	fileDumper = new FileDumper("Matrix.txt");
	Utility utility;
	DWORD dwDXDevice = utility.FindPattern((DWORD)GetModuleHandle("d3d9.dll"), 0x128000, (PBYTE)"\xC7\x06\x00\x00\x00\x00\x89\x86\x00\x00\x00\x00\x89\x86", "xx????xx????xx");
	DWORD* pdwVTable;
	memcpy(&pdwVTable, (VOID*)(dwDXDevice + 2), 4);

	playerAddy = 0x49E451;
	playerRetrn = 0x49E451 + 9;
	JmpPatch((PVOID)ourFunc, (PVOID)playerAddy);
	oEndScene = (tEndScene)DetourFunction((PBYTE)pdwVTable[42], (PBYTE)hkEndScene);
	oDIP = (tDIP)DetourFunction((PBYTE)pdwVTable[82], (PBYTE)hkDIP);
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

		printf("> Threads Active.\n");
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		Sleep(500);
	}

	return TRUE;
}