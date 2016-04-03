#include "stdafx.h"
#include "Directx9Hack.h"

void Directx9Hack::GenerateTexture(LPDIRECT3DDEVICE9 pDevice, IDirect3DTexture9 **ppD3Dtex, DWORD colour32)
{
	if (FAILED(pDevice->CreateTexture(8, 8, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, ppD3Dtex, NULL)));

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

}

void Directx9Hack::doDisassembleShader(LPDIRECT3DDEVICE9 pDevice, char* FileName)
{
	std::ofstream oLogFile(FileName, std::ios::trunc);

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

	oLogFile << "abc" << static_cast<char*>(bOut->GetBufferPointer()) << std::endl;
	oLogFile.close();

	delete[] pData;

	pShader->Release();
}