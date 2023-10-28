#pragma once
#include "Process.h"
namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	const float g_fFrame = 60.f;

    _bool g_isNight = false;
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

#include "ImGui_Manager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb\stb_image.h"
#endif // _DEBUG

//--------------------------------------------------Macro
#define			BEGININSTANCE					CGameInstance* pGameInstance = CGameInstance::GetInstance();		\
												Safe_AddRef(pGameInstance);
#define			ENDINSTANCE						Safe_Release(pGameInstance);

#define ADD_TIMER_SAFELY(timerName, isRepeating, duration) \
    CGameInstance* instancePtr = CGameInstance::GetInstance(); \
    if (instancePtr) \
    { \
        Safe_AddRef(instancePtr); \
        instancePtr->Add_Timer(TEXT(timerName), isRepeating, duration); \
        Safe_Release(instancePtr); \
    }

#define RESET_TIMER_SAFELY(timerName) \
    CGameInstance* instancePtr = CGameInstance::GetInstance(); \
    if (instancePtr) \
    { \
        Safe_AddRef(instancePtr); \
        instancePtr->Reset_Timer(TEXT(timerName)); \
        Safe_Release(instancePtr); \
    }

#define CHECK_TIMER_SAFELY(timerName, result) \
    CGameInstance* pInstance = CGameInstance::GetInstance(); \
    if (pInstance) \
    { \
        Safe_AddRef(pInstance); \
        _bool result = pInstance->Check_Timer(TEXT(timerName)); \
        Safe_Release(pInstance); \
        result; \
    }

#define ADD_INCREASE_LIGHT(_vPos, _fStartRange, fTime, vColor, isIncrease, fIncreasePower) \
    CGameInstance* pGameInstance = CGameInstance::GetInstance(); \
    Safe_AddRef(pGameInstance); \
    pGameInstance->Add_InstanceLight(_vPos, _fStartRange, fTime, vColor, isIncrease, fIncreasePower); \
    Safe_Release(pGameInstance); \

#define ADD_DECREASE_LIGHT(_vPos, _fStartRange, fTime, vColor) \
    CGameInstance* pGameInstance = CGameInstance::GetInstance(); \
    Safe_AddRef(pGameInstance); \
    pGameInstance->Add_InstanceLight(_vPos, _fStartRange, fTime, vColor); \
    Safe_Release(pGameInstance); \