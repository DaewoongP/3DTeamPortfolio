#include "..\Public\Lacewing_Flies_Item.h"

CLacewing_Flies_Item::CLacewing_Flies_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CLacewing_Flies_Item::CLacewing_Flies_Item(const CLacewing_Flies_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CLacewing_Flies_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.wstrKoreanName = TEXT("풀 잠자리");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_LacewingFlies.png");
	m_ItemCreateDesc.iCost = 90;

	// 재료 정보
	m_IngredientCreateDesc.eIngredient = INGREDIENT::LACEWING_FLIES;
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::ANIMAL_BASED_INGREDIENT;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLacewing_Flies_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CLacewing_Flies_Item* CLacewing_Flies_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CLacewing_Flies_Item* pInstance = New CLacewing_Flies_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CLacewing_Flies_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLacewing_Flies_Item::Clone(void* pArg)
{
	CLacewing_Flies_Item* pInstance = New CLacewing_Flies_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CLacewing_Flies_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLacewing_Flies_Item::Free(void)
{
	__super::Free();
}
