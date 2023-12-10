#include "..\Public\Mongrel_Fur_Item.h"

CMongrel_Fur_Item::CMongrel_Fur_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CMongrel_Fur_Item::CMongrel_Fur_Item(const CMongrel_Fur_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CMongrel_Fur_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_MONGREL_FUR;
	m_ItemCreateDesc.wstrKoreanName = TEXT("잡종견 털");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Wolf_Byproduct.png");
	m_ItemCreateDesc.iCost = 60;

	// 재료 정보
	m_IngredientCreateDesc.eIngredient = INGREDIENT::MONGREL_FUR;
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::ANIMAL_BASED_INGREDIENT;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMongrel_Fur_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CMongrel_Fur_Item* CMongrel_Fur_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CMongrel_Fur_Item* pInstance = New CMongrel_Fur_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CMongrel_Fur_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMongrel_Fur_Item::Clone(void* pArg)
{
	CMongrel_Fur_Item* pInstance = New CMongrel_Fur_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CMongrel_Fur_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMongrel_Fur_Item::Free(void)
{
	__super::Free();
}
