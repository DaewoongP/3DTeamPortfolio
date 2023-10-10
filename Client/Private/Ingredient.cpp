#include "Ingredient.h"
#include "GameInstance.h"

CIngredient::CIngredient(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CIngredient::CIngredient(const CIngredient& rhs)
	: CItem(rhs)
	, m_IngredientCreateDesc(rhs.m_IngredientCreateDesc)
{
}

HRESULT CIngredient::Initialize_Prototype(_uint iLevel)
{
	// ������ ������� �����;����� ��ӱ��� ������ ��¿ �� ���� Ŭ���� ������ ���Ե�.
	m_ItemCreateDesc.eItemType = RESOURCE;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CIngredient::Initialize(void* pArg)
{
	//m_eIngredient = ((CLONE_DESC*)pArg)->m_eIngredient;
	Setting_By_Ingredient();

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

//HRESULT CIngredient::Ready_Ingredient_UITexture()
//{
//	CGameInstance* pGameInstance = CGameInstance::GetInstance();
//	Safe_AddRef(pGameInstance);
//
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_AshwinderEggs.png")), E_FAIL);
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_DugbogTongue.png")), E_FAIL);
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_LeapingToadstool_Byproduct.png")), E_FAIL);
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_LacewingFlies.png")), E_FAIL);
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Knotgrass_Byproduct.png")), E_FAIL);
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_HorklumpJuice.png")), E_FAIL);
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_LeechJuice.png")), E_FAIL);
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Mallowsweet_Byproduct.png")), E_FAIL);
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Moonstone.png")), E_FAIL);
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Shrivelfig_Byproduct.png")), E_FAIL);
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Wolf_Byproduct.png")), E_FAIL);
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_TrollMucus.png")), E_FAIL);
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Skull.png")), E_FAIL);
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Spider_fang.png")), E_FAIL);
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Dittany.png")), E_FAIL);
//	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
//		, TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Icons_Fluxweed.png")), E_FAIL);
//
//	Safe_Release(pGameInstance);
//
//	return S_OK;
//}

void CIngredient::Setting_By_Ingredient()
{
	/*switch (m_eIngredient)
	{
	case Client::ASHWINDER_EGGS:
		m_ItemCreateDesc.wstrKoreanName = TEXT("�ֽ����δ� ��");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_AshwinderEggs.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::DITTANY_LEAVES:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("�� ���� ��");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Dittany.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::DITTANY:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("�� ����");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Dittany.png");
		m_ItemCreateDesc.iCost = 60;
		break;
	case Client::DUGBOG_TONGUE:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("���׺��� ��");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_DugbogTongue.png");
		m_ItemCreateDesc.iCost = 70;
		break;
	case Client::LEAPING_TOADSTOOL_CAPS:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("������ ��");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_LeapingToadstool_Byproduct.png");
		m_ItemCreateDesc.iCost = 80;
		break;
	case Client::LACEWING_FLIES:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("Ǯ ���ڸ�");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_LacewingFlies.png");
		m_ItemCreateDesc.iCost = 90;
		break;
	case Client::KNOTGRASS:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("����Ǯ");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Knotgrass_Byproduct.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::HORKLUMP_JUICE:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("ȣŬ���� ��");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_HorklumpJuice.png");
		m_ItemCreateDesc.iCost = 60;
		break;
	case Client::LEECH_JUICE:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("�ŸӸ� ��");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_LeechJuice.png");
		m_ItemCreateDesc.iCost = 70;
		break;
	case Client::MALLOWSWEET:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("���� ���� �ƿ� ��");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Mallowsweet_Byproduct.png");
		m_ItemCreateDesc.iCost = 80;
		break;
	case Client::MOONSTONE:
		m_eIngredientType = MINERAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("���弮");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Moonstone.png");
		m_ItemCreateDesc.iCost = 90;
		break;
	case Client::SHRIVELFIG:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("���׶�� ��ȭ�� ����");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Shrivelfig_Byproduct.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::MONGREL_FUR:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("������ ��");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Wolf_Byproduct.png");
		m_ItemCreateDesc.iCost = 60;
		break;
	case Client::TROLL_BOGEYS:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("Ʈ�� ����");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_TrollMucus.png");
		m_ItemCreateDesc.iCost = 70;
		break;
	case Client::STENCH_OF_THE_DEAD:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("������ ����");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Skull.png");
		m_ItemCreateDesc.iCost = 80;
		break;
	case Client::SPIDER_FANG:
		m_eIngredientType = ANIMAL_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("�ŸӸ� �۰���");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Spider_fang.png");
		m_ItemCreateDesc.iCost = 90;
		break;
	case Client::FLUXWEED_STEM:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("������ �ٱ�");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Icons_Fluxweed.png");
		m_ItemCreateDesc.iCost = 60;
		break;
	case Client::KNOTGRASS_SPRIG:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("����Ǯ ����");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Knotgrass_Byproduct.png");
		m_ItemCreateDesc.iCost = 50;
		break;
	case Client::MALLOWSWEET_LEAVES:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("������ ��");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Mallowsweet_Byproduct.png");
		m_ItemCreateDesc.iCost = 70;
		break;
	case Client::SHRIVELFIG_FRUIT:
		m_eIngredientType = PLANT_BASED_INGREDIENT;
		m_ItemCreateDesc.wstrKoreanName = TEXT("���׶�� ��ȭ�� ���� ����");
		m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Shrivelfig_Byproduct.png");
		m_ItemCreateDesc.iCost = 80;
		break;
	default:
		break;
	}*/
}

void CIngredient::Free(void)
{
	__super::Free();
}
