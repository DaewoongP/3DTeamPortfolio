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
		BUFF_LEVIOSO		= 1 << 2,
		BUFF_LEVIOSO_TONGUE = 1 << 3,
		BUFF_STUPEFY		= 1 << 4,
		BUFF_ACCIO			= 1 << 5,
		BUFF_CONFRINGO		= 1 << 6,
		BUFF_NCENDIO		= 1 << 7,
		BUFF_BUFF_DESCENDO	= 1 << 8,
		BUFF_DESCENDO		= 1 << 9,
		BUFF_END = 10
	};

#pragma region UI
	enum MENU
	{
		GEAR,
		INVENTORY,
		QUEST,
		SETTING,
		MENU_END
	};

	enum POTIONTAP
	{
		DEFENSIVE_POWER_UP,
		ATTACK_POWER_UP,
		SHOW_TIME,
		THUNDER_CLOUD,
		INVISIBILITY_PILL,
		MANDRAKE,
		BITE_CABBAGE,
		TENTACULAR,
		POTIONTAP_END
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

	enum ITEMTYPE
	{
		HAND,
		FACE,
		HEAD,
		NECK,
		BACK,
		OUTFIT,
		RESOURCE,
		POTION,
		ITEMTYPE_END
	};


	enum INGREDIENT_TYPE
	{
		ANIMAL_BASED_INGREDIENT,
		PLANT_BASED_INGREDIENT,
		MINERAL_BASED_INGREDIENT,
		MAGICAL_BASED_INGREDIENT,
		INGREDIENT_TYPE_END
	};

	enum INGREDIENT
	{
		ASHWINDER_EGGS,
		DUGBOG_TONGUE,
		LEAPING_TOADSTOOL_CAPS,
		LACEWING_FLIES,
		KNOTGRASS,
		HORKLUMP_JUICE,
		LEECH_JUICE,
		MALLOWSWEET,
		MOONSTONE,
		SHRIVELFIG,
		MONGREL_FUR,
		TROLL_BOGEYS,
		STENCH_OF_THE_DEAD,
		SPIDER_FANG,
		DITTANY_LEAVES,
		DITTANY,
		FLUXWEED_STEM,
		KNOTGRASS_SPRIG,
		MALLOWSWEET_LEAVES,
		SHRIVELFIG_FRUIT,
		INGREDIENT_END
	};
}