#pragma once
#include "Process.h"
namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	const float g_fFrame = 60.f;

	enum LEVELID { LEVEL_STATIC, LEVEL_LOGO, LEVEL_STAGE1, LEVEL_BOSS, LEVEL_LOADING, LEVEL_END };
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;



//--------------------------------------------------Macro
#define			BEGININSTANCE					CGameInstance* pGameInstance = CGameInstance::GetInstance();		\
												Safe_AddRef(pGameInstance);
#define			ENDINSTANCE						Safe_Release(pGameInstance);