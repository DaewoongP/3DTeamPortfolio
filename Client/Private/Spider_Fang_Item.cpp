#include "..\Public\Spider_Fang_Item.h"

CSpider_Fang_Item::CSpider_Fang_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CSpider_Fang_Item::CSpider_Fang_Item(const CSpider_Fang_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CSpider_Fang_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.wstrKoreanName = TEXT("거머리 송곳니");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Ingredients/UI_T_Spider_fang.png");
	m_ItemCreateDesc.iCost = 90;

	// 재료 정보
	m_IngredientCreateDesc.eIngredient = INGREDIENT::SPIDER_FANG;
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::ANIMAL_BASED_INGREDIENT;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpider_Fang_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CSpider_Fang_Item* CSpider_Fang_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CSpider_Fang_Item* pInstance = New CSpider_Fang_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CSpider_Fang_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpider_Fang_Item::Clone(void* pArg)
{
	CSpider_Fang_Item* pInstance = New CSpider_Fang_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CSpider_Fang_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpider_Fang_Item::Free(void)
{
	__super::Free();
}
