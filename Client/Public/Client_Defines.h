#pragma once
#include "Process.h"
namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	const float g_fFrame = 60.f;

	enum LEVELID { LEVEL_STATIC, LEVEL_LOGO, 
		LEVEL_MAINGAME, LEVEL_CLIFFSIDE, LEVEL_VAULT, 
		LEVEL_GREATHALL, LEVEL_SMITH, 
		LEVEL_MINIGAME, LEVEL_PVP, 
		LEVEL_SKY, LEVEL_DRAGON, 
		LEVEL_LOADING, LEVEL_END };

	enum SPELL
	{
		ACCIO,
		ALOHOMORA,
		ALTERATION,
		ARRESTOMOMENTUM,
		AVADAKEDAVRA,
		BOMBARDA,
		CONFRINGO,
		CONJURATION,
		CRUCIO,
		DEPULSO,
		DESCENDO,
		DIFFINDO,
		DISILLUSIONMENT,
		EVAESCO,
		EXPELLIARMUS,
		FLIPENDO,
		GLACIUS,
		IMPERIO,
		NCENDIO,
		LEVIOSO,
		LUMOS,
		PROTEGO,
		REPARO,
		REVELIO,
		TRANSFORMATION,
		TRANSFORMATIONOVERLAND,
		WINGARDIUMLEVIOSA,
		BASICCAST,
		STUPEFY,
		PETRIFICUSTOTALUS,
		MAGICTHROW,
		FINISHER,
		SPELL_END
	};

	// WITH MONSTER
	// 사용할 경우 주성환한테 얘기해주세요.
	enum BUFF_TYPE
	{
		BUFF_NONE			= 0,		// 아무 디버프 없음.( 평타 )
		BUFF_SLOW			= 1 << 0,
		BUFF_FROZEN			= 1 << 1,
		BUFF_CHANGE			= 1 << 2,
		BUFF_WEAPONLOSS		= 1 << 3,
		BUFF_FIRE			= 1 << 4,
		BUFF_LEVIOSO		= 1 << 5,
		BUFF_PUSH			= 1 << 6,
		BUFF_CURSE			= 1 << 7,
		BUFF_STUPEFY		= 1 << 8,
		BUFF_DESCENDO		= 1 << 9,
		BUFF_END			= 12
	};

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
}

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