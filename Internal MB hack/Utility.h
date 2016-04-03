#include <d3dx9.h>
#include <fstream>

class Utility
{
public:
	bool bCompare(const BYTE* pData, const BYTE* bMask, const char* szMask);
	DWORD FindPattern(DWORD dValor, DWORD dLer, BYTE *bMaskara, char * szMaskara);
	BOOL DetectWorldPos(LPDIRECT3DDEVICE9 pDevice);
};

