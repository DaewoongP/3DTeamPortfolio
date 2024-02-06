#pragma once

namespace Client
{
	enum OBJ_TYPE { COMPONENT, GAMEOBJECT };

	enum LEVELID {
		LEVEL_STATIC, LEVEL_LOGO,
		LEVEL_CLIFFSIDE, LEVEL_VAULT,
		LEVEL_GREATHALL, LEVEL_SMITH,
		LEVEL_MINIGAME, LEVEL_PVP,
		LEVEL_SKY, LEVEL_SANCTUM,
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
		PROJECTILE_WHITE,
		PROJECTILE_BLACK,
		PENSIVE_GROUND_BALL,
		PENSIVE_FAIL_BALL,
		PENSIVE_SHOUTING,
		PENSIVE_SWORD_THROW,
		PENSIVE_MACE_ATTACK,
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
		BUFF_DESCENDO		= 1 << 8,
		BUFF_FLIPENDO		= 1 << 9,
		BUFF_PROTEGO		= 1 << 10,
		BUFF_DIFFINDO		= 1 << 11,
		BUFF_ARRESTOMOMENTUM= 1 << 12,
		BUFF_END = 12
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
		ENDURUS_POTION,
		MAXIMA_POTION,
		FOCUS_POTION,
		THUNDERBREW_POTION,
		INVISIBILITY_POTION,
		MANDRAKE,
		HEALTH_POTION,
		TENTACULAR,
		POTIONTAP_END
	};
#pragma endregion

	enum ITEM_ID
	{
		ITEM_ID_WIGGENWELD_POTION,
		ITEM_ID_EDURUS_POTION,
		ITEM_ID_FOCUS_POTION,
		ITEM_ID_MAXIMA_POTION,
		ITEM_ID_INVISIBILITY_POTION,
		ITEM_ID_THUNDERBEW_POTION,
		ITEM_ID_FELIX_FELICIS_POTION,
		ITEM_ID_CHINESE_CHOMPING_CABBAGE,
		ITEM_ID_ASHWINDER_EGGS, // 애쉬와인더 알
		ITEM_ID_DUGBOG_TONGUE, // 더그보그 혀
		ITEM_ID_LEAPING_TOADSTOOL_CAPS, // 독버섯 갓
		ITEM_ID_LACEWING_FLIES, // 풀 잠자리
		ITEM_ID_KNOTGRASS, // 마디풀
		ITEM_ID_HORKLUMP_JUICE, // 호클럼프 즙
		ITEM_ID_LEECH_JUICE, // 거머리 즙
		ITEM_ID_MALLOWSWEET, // 향이 나는 아욱 잎
		ITEM_ID_MOONSTONE, // 월장석
		ITEM_ID_SHRIVELFIG, // 오그라든 무화과 나무 열매
		ITEM_ID_MONGREL_FUR, // 잡종견 털
		ITEM_ID_TROLL_BOGEYS, // 트롤 점액
		ITEM_ID_STENCH_OF_THE_DEAD, // 죽음의 악취
		ITEM_ID_SPIDER_FANG, // 거미 송곳니
		ITEM_ID_DITTANY_LEAVES, // 꽃 박하 잎
		//ITEM_ID_DITTANY, // 꽃 박하 잎
		ITEM_ID_FLUXWEED_STEM, // 보름초 줄기
		//ITEM_ID_KNOTGRASS_SPRIG, // 마디풀
		ITEM_ID_MALLOWSWEET_LEAVES, // 향이 나는 아욱 잎
		//ITEM_ID_SHRIVELFIG_FRUIT, // 오그라든 무화과 나무 열매
		ITEM_ID_ROBE_ARCANE,
		ITEM_ID_ROBE_BEAST,
		ITEM_ID_ROBE_DARKARTS,
		ITEM_ID_ROBE_DARKARTS_DELUX,
		ITEM_ID_ROBE_QUDDITCH,
		ITEM_ID_HAT_ARCANE,
		ITEM_ID_HAT_WIZARD,
		ITEM_ID_OUTFIT_ARCANE,
		ITEM_ID_OUTFIT_ARCANE_A,
		ITEM_ID_OUTFIT_CELTIC,
		ITEM_ID_OUTFIT_CELTIC_A,
		ITEM_ID_OUTFIT_DARKARTS_DELUX,
		ITEM_ID_OUTFIT_DARKARTS_DELUX_A,
		ITEM_ID_MASK_GUARDIAN,
		ITEM_ID_MASK_NIFFLER,
		ITEM_ID_MASK_DEMIGUISE,
		ITEM_ID_END
	};

	enum TOOL
	{
		TOOL_WIGGENWELD_POTION,
		TOOL_EDURUS_POTION,
		TOOL_FOCUS_POTION,
		TOOL_MAXIMUS_POTION,
		TOOL_INVISIBILITY_POTION,
		TOOL_THUNDERBEW_POTION,
		TOOL_FELIX_FELICIS_POTION,
		TOOL_CHINESE_CHOMPING_CABBAGE,
		TOOL_END		
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
		ASHWINDER_EGGS, // 애쉬와인더 알
		DUGBOG_TONGUE, // 더그보그 혀
		LEAPING_TOADSTOOL_CAPS, // 독버섯 갓
		LACEWING_FLIES, // 풀 잠자리
		KNOTGRASS, // 마디풀
		HORKLUMP_JUICE, // 호클럼프 즙
		LEECH_JUICE, // 거머리 즙
		MALLOWSWEET, // 향이 나는 아욱 잎
		MOONSTONE, // 월장석
		SHRIVELFIG, // 오그라든 무화과 나무 열매
		MONGREL_FUR, // 잡종견 털
		TROLL_BOGEYS, // 트롤 점액
		STENCH_OF_THE_DEAD, // 죽음의 악취
		SPIDER_FANG, // 거미 송곳니
		DITTANY_LEAVES, // 꽃 박하 잎
		//DITTANY, // 꽃 박하 잎
		FLUXWEED_STEM, // 보름초 줄기
		//KNOTGRASS_SPRIG, // 마디풀
		MALLOWSWEET_LEAVES, // 향이 나는 아욱 잎
		//SHRIVELFIG_FRUIT, // 오그라든 무화과 나무 열매
		INGREDIENT_END
	};

	enum QUESTLIST
	{
		QUEST_FIG,
		QUEST_POTION,
		QUEST_TOWN,
		QUEST_SECRET,
		QUEST_BONE,
		QUEST_END
	};

	enum QUESTSTATE
	{
		QUESTSTATE_QUESTIONMARK,
		QUESTSTATE_UNLOCK,
		QUESTSTATE_CLEAR,
		QUESTSTATE_END
	};
}