#include "..\Public\Dugbog_Tongue_Item.h"

CDugbog_Tongue_Item::CDugbog_Tongue_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CDugbog_Tongue_Item::CDugbog_Tongue_Item(const CDugbog_Tongue_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CDugbog_Tongue_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_DUGBOG_TONGUE;
	m_ItemCreateDesc.wstrKoreanName = TEXT("더그보그 혀");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_DugbogTongue.png");
	m_ItemCreateDesc.iCost = 70;

	// 재료 정보
	m_IngredientCreateDesc.eIngredient = INGREDIENT::DUGBOG_TONGUE;
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::ANIMAL_BASED_INGREDIENT;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDugbog_Tongue_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CDugbog_Tongue_Item* CDugbog_Tongue_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CDugbog_Tongue_Item* pInstance = New CDugbog_Tongue_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CDugbog_Tongue_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDugbog_Tongue_Item::Clone(void* pArg)
{
	CDugbog_Tongue_Item* pInstance = New CDugbog_Tongue_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CDugbog_Tongue_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDugbog_Tongue_Item::Free(void)
{
	__super::Free();
}
