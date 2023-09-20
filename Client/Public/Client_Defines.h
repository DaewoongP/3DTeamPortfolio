#pragma once
#include "Process.h"
namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	const float g_fFrame = 60.f;

	enum LEVELID { LEVEL_STATIC, LEVEL_LOGO, LEVEL_MAINGAME, LEVEL_LOADING, LEVEL_END };

	enum SPELL
	{
		ACCIO					,//= 0,
		ALOHOMORA				,//= 1 << 0,
		ALTERATION				,//= 1 << 1,
		ARRESTOMOMENTUM			,//= 1 << 2,
		AVADAKEDAVRA			,//= 1 << 3,
		BOMBARDA				,//= 1 << 4,
		CONFRINGO				,//= 1 << 5,
		CONJURATION				,//= 1 << 6,
		CRUCIO					,//= 1 << 7,
		DEPULSO					,//= 1 << 8,
		DESCENDO				,//= 1 << 9,
		DIFFINDO				,//= 1 << 10,
		DISILLUSIONMENT			,//= 1 << 11,
		EVAESCO					,//= 1 << 12,
		EXPELLIARMUS			,//= 1 << 13,
		FLIPENDO				,//= 1 << 14,
		GLACIUS					,//= 1 << 15,
		IMPERIO					,//= 1 << 16,
		NCENDIO					,//= 1 << 17,
		LEVIOSO					,//= 1 << 18,
		LUMOS					,//= 1 << 19,
		PROTEGO					,//= 1 << 20,
		REPARO					,//= 1 << 21,
		REVELIO					,//= 1 << 22,
		TRANSFORMATION			,//= 1 << 23,
		TRANSFORMATIONOVERLAND	,//= 1 << 24,
		WINGARDIUMLEVIOSA		,//= 1 << 25,
		BASICCAST				,//= 1 << 26,
		STUPEFY					,//= 1 << 27,
		PETRIFICUSTOTALUS		,//= 1 << 28,
		MAGICTHROW				,//= 1 << 29,
		FINISHER				,//= 1 << 30,
		SPELL_END				
	};
}

#pragma region UI Scene
	enum MENU
	{
		GEAR,
		INVENTORY,
		//TALENT,
		//COLLECTION,
		//ACTION,
		//MAP,
		QUEST,
		//OWLMAIL,
		SETTING,
		MENU_END
	};
#pragma endregion

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;

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