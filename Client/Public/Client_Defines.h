#pragma once
#include "Process.h"
namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	const float g_fFrame = 60.f;
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;

#include "Client_Enum.h"
#include "Client_Struct.h"

#ifdef _DEBUG

#include "imgui.h"
#include "backends\imgui_impl_win32.h"
#include "backends\imgui_impl_dx11.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb\stb_image.h"
#endif // _DEBUG

//--------------------------------------------------Macro
#define			BEGININSTANCE					CGameInstance* pGameInstance = CGameInstance::GetInstance();		\
												Safe_AddRef(pGameInstance);
#define			ENDINSTANCE						Safe_Release(pGameInstance);