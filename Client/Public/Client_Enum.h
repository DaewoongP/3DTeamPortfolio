#pragma once

namespace Client
{
	enum LEVELID {
		LEVEL_STATIC, LEVEL_LOGO,
		LEVEL_CLIFFSIDE, LEVEL_VAULT,
		LEVEL_GREATHALL, LEVEL_SMITH,
		LEVEL_MINIGAME, LEVEL_PVP,
		LEVEL_SKY, LEVEL_DRAGON,
		LEVEL_LOADING, LEVEL_END
	};

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
		BUFF_NONE			= 0,		// 아무 디버프 없음
		BUFF_ATTACK_LIGHT	= 1 << 0,	// 기본공격
		BUFF_ATTACK_HEAVY	= 1 << 1,	// 마지막 공격
		BUFF_SLOW			= 1 << 2,
		BUFF_FROZEN			= 1 << 3,
		BUFF_UNGRAVITY		= 1 << 4,
		BUFF_WEAPONLOSS		= 1 << 5,
		BUFF_FIRE			= 1 << 6,
		BUFF_LEVIOSO		= 1 << 7,
		BUFF_PUSH			= 1 << 8,
		BUFF_CURSE			= 1 << 9,
		BUFF_STUPEFY		= 1 << 10,
		BUFF_DESCENDO		= 1 << 11,
		BUFF_LEVIOSO_TONGUE = 1 << 12,
		BUFF_END = 14
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

	enum ITEM
	{
		ITEM_WIGGENWELD_POTION,
		ITEM_EDURUS_POTION,
		ITEM_FOCUS_POTION,
		ITEM_MAXIMUS_POTION,
		ITEM_INVISIBILITY_POTION,
		ITEM_THUNDERBEW_POTION,
		ITEM_FELIX_FELICIS_POTION,
		ITEM_CHINESE_CHOMPING_CABBAGE,
		ITEM_END		
	};
}