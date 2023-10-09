#include "..\Public\Leech_Juice_Item.h"

CLeech_Juice_Item::CLeech_Juice_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CLeech_Juice_Item::CLeech_Juice_Item(const CLeech_Juice_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CLeech_Juice_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.wstrKoreanName = TEXT("거머리 즙");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_LeechJuice.png");
	m_ItemCreateDesc.iCost = 70;

	// 재료 정보
	m_IngredientCreateDesc.eIngredient = INGREDIENT::LEECH_JUICE;
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::ANIMAL_BASED_INGREDIENT;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLeech_Juice_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CLeech_Juice_Item* CLeech_Juice_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CLeech_Juice_Item* pInstance = New CLeech_Juice_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CLeech_Juice_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLeech_Juice_Item::Clone(void* pArg)
{
	CLeech_Juice_Item* pInstance = New CLeech_Juice_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CLeech_Juice_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLeech_Juice_Item::Free(void)
{
	__super::Free();
}
