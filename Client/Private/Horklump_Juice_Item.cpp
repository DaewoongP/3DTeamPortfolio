#include "..\Public\Horklump_Juice_Item.h"

CHorklump_Juice_Item::CHorklump_Juice_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CHorklump_Juice_Item::CHorklump_Juice_Item(const CHorklump_Juice_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CHorklump_Juice_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.wstrKoreanName = TEXT("호클럼프 즙");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_HorklumpJuice.png");
	m_ItemCreateDesc.iCost = 60;

	// 재료 정보
	m_IngredientCreateDesc.eIngredient = INGREDIENT::HORKLUMP_JUICE;
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::ANIMAL_BASED_INGREDIENT;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHorklump_Juice_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CHorklump_Juice_Item* CHorklump_Juice_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CHorklump_Juice_Item* pInstance = New CHorklump_Juice_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CHorklump_Juice_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHorklump_Juice_Item::Clone(void* pArg)
{
	CHorklump_Juice_Item* pInstance = New CHorklump_Juice_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CHorklump_Juice_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHorklump_Juice_Item::Free(void)
{
	__super::Free();
}
