#include "..\Public\MoonStone_Item.h"

CMoonStone_Item::CMoonStone_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CMoonStone_Item::CMoonStone_Item(const CMoonStone_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CMoonStone_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_MOONSTONE;
	m_ItemCreateDesc.wstrKoreanName = TEXT("월장석");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Moonstone.png");
	m_ItemCreateDesc.iCost = 90;

	// 재료 정보
	m_IngredientCreateDesc.eIngredient = INGREDIENT::MOONSTONE;
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::MINERAL_BASED_INGREDIENT;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMoonStone_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CMoonStone_Item* CMoonStone_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CMoonStone_Item* pInstance = New CMoonStone_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CMoonStone_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMoonStone_Item::Clone(void* pArg)
{
	CMoonStone_Item* pInstance = New CMoonStone_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CMoonStone_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoonStone_Item::Free(void)
{
	__super::Free();
}
