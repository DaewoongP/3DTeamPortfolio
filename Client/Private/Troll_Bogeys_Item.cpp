#include "..\Public\Troll_Bogeys_Item.h"

CTroll_Bogeys_Item::CTroll_Bogeys_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CTroll_Bogeys_Item::CTroll_Bogeys_Item(const CTroll_Bogeys_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CTroll_Bogeys_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.wstrKoreanName = TEXT("트롤 점액");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_TrollMucus.png");
	m_ItemCreateDesc.iCost = 70;

	// 재료 정보
	m_IngredientCreateDesc.eIngredient = INGREDIENT::TROLL_BOGEYS;
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::ANIMAL_BASED_INGREDIENT;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTroll_Bogeys_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CTroll_Bogeys_Item* CTroll_Bogeys_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CTroll_Bogeys_Item* pInstance = New CTroll_Bogeys_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CTroll_Bogeys_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTroll_Bogeys_Item::Clone(void* pArg)
{
	CTroll_Bogeys_Item* pInstance = New CTroll_Bogeys_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CTroll_Bogeys_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTroll_Bogeys_Item::Free(void)
{
	__super::Free();
}
