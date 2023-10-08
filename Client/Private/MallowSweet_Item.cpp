#include "..\Public\MallowSweet_Item.h"

CMallowSweet_Item::CMallowSweet_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CMallowSweet_Item::CMallowSweet_Item(const CMallowSweet_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CMallowSweet_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.wstrKoreanName = TEXT("향이 나는 아욱 잎");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Mallowsweet_Byproduct.png");
	m_ItemCreateDesc.iCost = 80;

	// 재료 정보
	m_IngredientCreateDesc.eIngredient = INGREDIENT::MALLOWSWEET;
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::PLANT_BASED_INGREDIENT;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMallowSweet_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CMallowSweet_Item* CMallowSweet_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CMallowSweet_Item* pInstance = New CMallowSweet_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CMallowSweet_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMallowSweet_Item::Clone(void* pArg)
{
	CMallowSweet_Item* pInstance = New CMallowSweet_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CMallowSweet_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMallowSweet_Item::Free(void)
{
	__super::Free();
}
