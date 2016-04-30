#include "stdafx.h"
#include "Utility.h"

bool Utility::bCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask == 'x' && *pData != *bMask)
			return false;
	return (*szMask) == NULL;
}

DWORD Utility::FindPattern(DWORD dValor, DWORD dLer, BYTE *bMaskara, char * szMaskara)
{
	for (DWORD i = 0; i < dLer; i++)
		if (bCompare((PBYTE)(dValor + i), bMaskara, szMaskara))
			return (DWORD)(dValor + i);
	return false;
}

BOOL Utility::DetectWorldPos(LPDIRECT3DDEVICE9 pDevice)
{
	D3DXMATRIX WorldPos;
	pDevice->GetTransform(D3DTS_WORLD, &WorldPos);

	if (WorldPos._11 == 1.0f && WorldPos._22 == 1.0f && WorldPos._33 == 1.0f && WorldPos._44 == 1.0f)
	{
		return false; //game is using Effect->SetMatrix
	}
	else
	{
		return true; // game is using Device->SetTransform
	}

}
