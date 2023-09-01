#pragma once
#include "Process.h"
namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	const float g_fFrame = 60.f;

	enum LEVELID { LEVEL_STATIC, LEVEL_LOGO, LEVEL_MAINGAME, LEVEL_LOADING, LEVEL_END };
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;

#ifdef _DEBUG

#include "ImGui\imgui.h"
#include "ImGui\backends\imgui_impl_win32.h"
#include "ImGui\backends\imgui_impl_dx11.h"

#define STB_IMAGE_IMPLEMENTATION
#include "ImGui\stb\stb_image.h"
#endif // _DEBUG

//--------------------------------------------------Macro
#define			BEGININSTANCE					CGameInstance* pGameInstance = CGameInstance::GetInstance();		\
												Safe_AddRef(pGameInstance);
#define			ENDINSTANCE						Safe_Release(pGameInstance);