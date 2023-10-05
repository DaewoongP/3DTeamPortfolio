#include "..\Public\AshwinderEggs_Item.h"

CAshwinderEggs_Item::CAshwinderEggs_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CAshwinderEggs_Item::CAshwinderEggs_Item(const CAshwinderEggs_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CAshwinderEggs_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.wstrKoreanName = TEXT("애쉬와인더 알");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_AshwinderEggs.png");
	m_ItemCreateDesc.iCost = 50;

	// 재료 정보
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::ANIMAL_BASED_INGREDIENT;
	m_IngredientCreateDesc.eIngredient = INGREDIENT::ASHWINDER_EGGS;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAshwinderEggs_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CAshwinderEggs_Item* CAshwinderEggs_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CAshwinderEggs_Item* pInstance = New CAshwinderEggs_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CAshwinderEggs_Item");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAshwinderEggs_Item::Clone(void* pArg)
{
	CAshwinderEggs_Item* pInstance = New CAshwinderEggs_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAshwinderEggs_Item");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAshwinderEggs_Item::Free(void)
{
	__super::Free();
}
