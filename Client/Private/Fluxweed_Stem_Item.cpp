#include "..\Public\Fluxweed_Stem_Item.h"

CFluxweed_Stem_Item::CFluxweed_Stem_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CFluxweed_Stem_Item::CFluxweed_Stem_Item(const CFluxweed_Stem_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CFluxweed_Stem_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.wstrKoreanName = TEXT("보름초 줄기");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Icons_Fluxweed.png");
	m_ItemCreateDesc.iCost = 60;

	// 재료 정보
	m_IngredientCreateDesc.eIngredient = INGREDIENT::FLUXWEED_STEM;
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::PLANT_BASED_INGREDIENT;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFluxweed_Stem_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CFluxweed_Stem_Item* CFluxweed_Stem_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CFluxweed_Stem_Item* pInstance = New CFluxweed_Stem_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CFluxweed_Stem_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFluxweed_Stem_Item::Clone(void* pArg)
{
	CFluxweed_Stem_Item* pInstance = New CFluxweed_Stem_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CFluxweed_Stem_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFluxweed_Stem_Item::Free(void)
{
	__super::Free();
}
