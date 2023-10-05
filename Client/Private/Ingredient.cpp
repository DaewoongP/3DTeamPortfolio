#include "Ingredient.h"
#include "GameInstance.h"
CIngredient::CIngredient(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CIngredient::CIngredient(const CIngredient& rhs)
	: CItem(rhs)
{
}

HRESULT CIngredient::Initialize_Prototype(_uint iLevel)
{
	m_ItemCreateDesc.eItemType = RESOURCE;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CIngredient::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CIngredient::Setting_By_Ingredient()
{
	switch (m_eIngredient)
	{
	case Client::ASHWINDER_EGGS:

		break;
	case Client::DITTANY_LEAVES:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("꽃 박하 잎");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Dittany.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::DITTANY:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("꽃 박하");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Dittany.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::DUGBOG_TONGUE:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("더그보그 혀");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_DugbogTongue.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::LEAPING_TOADSTOOL_CAPS:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("독버섯 갓");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_LeapingToadstool_Byproduct.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::LACEWING_FLIES:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("풀 잠자리");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_LacewingFlies.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::KNOTGRASS:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("마디풀");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Knotgrass_Byproduct.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::HORKLUMP_JUICE:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("호클럼프 즙");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_HorklumpJuice.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::LEECH_JUICE:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("거머리 즙");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_LeechJuice.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::MALLOWSWEET:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("향이 나는 아욱 잎");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Mallowsweet_Byproduct.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::MOONSTONE:
		m_eIngredientType = MINERAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("월장석");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Moonstone.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::SHRIVELFIG:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("오그라든 무화과 나무");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Shrivelfig_Byproduct.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::MONGREL_FUR:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("잡종견 털");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Wolf_Byproduct.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::TROLL_BOGEYS:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("트롤 점액");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_TrollMucus.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::STENCH_OF_THE_DEAD:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("죽음의 악취");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Skull.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::SPIDER_FANG:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("거머리 송곳니");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Spider_fang.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::FLUXWEED_STEM:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("보름초 줄기");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Icons_Fluxweed.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::KNOTGRASS_SPRIG:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("마디풀 가지");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Knotgrass_Byproduct.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::MALLOWSWEET_LEAVES:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("달콤한 잎");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Mallowsweet_Byproduct.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::SHRIVELFIG_FRUIT:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("오그라든 무화과 나무 열매");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Shrivelfig_Byproduct.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	default:
		break;
	}
}

void CIngredient::Free(void)
{
	__super::Free();
}
