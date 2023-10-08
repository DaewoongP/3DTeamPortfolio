#include "..\Public\Shrivelfig_Item.h"

CShrivelfig_Item::CShrivelfig_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CShrivelfig_Item::CShrivelfig_Item(const CShrivelfig_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CShrivelfig_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.wstrKoreanName = TEXT("오그라든 무화과 나무 열매");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Shrivelfig_Byproduct.png");
	m_ItemCreateDesc.iCost = 80;

	// 재료 정보
	m_IngredientCreateDesc.eIngredient = INGREDIENT::SHRIVELFIG;
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::PLANT_BASED_INGREDIENT;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShrivelfig_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CShrivelfig_Item* CShrivelfig_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CShrivelfig_Item* pInstance = New CShrivelfig_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CShrivelfig_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShrivelfig_Item::Clone(void* pArg)
{
	CShrivelfig_Item* pInstance = New CShrivelfig_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CShrivelfig_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShrivelfig_Item::Free(void)
{
	__super::Free();
}
