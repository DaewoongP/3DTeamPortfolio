#pragma once
namespace Tool
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	const float g_fFrame = 60.f;

	enum LEVELID { LEVEL_TOOL, LEVEL_END };
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Tool;

#include "Tool_Functions.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "ImGuiFileDialog.h"

#include <wincodec.h> // Optional

//--------------------------------------------------Macro
#define            BEGININSTANCE                    CGameInstance* pGameInstance = CGameInstance::GetInstance();        \
													Safe_AddRef(pGameInstance);
#define            ENDINSTANCE                      Safe_Release(pGameInstance);

#define LIST_INDEX_ACCESS(_list, _iIndex, _OutData)				\
		if (false == _list.empty())								\
		{														\
			auto Iter = _list.begin();							\
																\
			for(size_t Index = 0; Index < _iIndex; Index++)		\
			{													\
				++Iter;											\
			}													\
																\
			_OutData = &(*Iter);								\
		}														

#define LIST_INDEX_ACCESS_POINTER(_list, _iIndex, _OutDataPointer)	\
		if (false == _list.empty())									\
		{															\
			auto Iter = _list.begin();								\
																	\
			for(size_t Index = 0; Index < _iIndex; Index++)			\
			{														\
				++Iter;												\
			}														\
																	\
			_OutDataPointer = (*Iter);								\
		}	