#include "..\Public\Leaping_Toadstool_Caps_Item.h"

CLeaping_Toadstool_Caps_Item::CLeaping_Toadstool_Caps_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CLeaping_Toadstool_Caps_Item::CLeaping_Toadstool_Caps_Item(const CLeaping_Toadstool_Caps_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CLeaping_Toadstool_Caps_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.wstrKoreanName = TEXT("독버섯 갓");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_LeapingToadstool_Byproduct.png");
	m_ItemCreateDesc.iCost = 50;

	// 재료 정보
	m_IngredientCreateDesc.eIngredient = INGREDIENT::LEAPING_TOADSTOOL_CAPS;
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::ANIMAL_BASED_INGREDIENT;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLeaping_Toadstool_Caps_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CLeaping_Toadstool_Caps_Item* CLeaping_Toadstool_Caps_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CLeaping_Toadstool_Caps_Item* pInstance = New CLeaping_Toadstool_Caps_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CLeaping_Toadstool_Caps_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLeaping_Toadstool_Caps_Item::Clone(void* pArg)
{
	CLeaping_Toadstool_Caps_Item* pInstance = New CLeaping_Toadstool_Caps_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CLeaping_Toadstool_Caps_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLeaping_Toadstool_Caps_Item::Free(void)
{
	__super::Free();
}
