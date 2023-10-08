#include "..\Public\Knotgrass_Item.h"

CKnotgrass_Item::CKnotgrass_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CKnotgrass_Item::CKnotgrass_Item(const CKnotgrass_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CKnotgrass_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.wstrKoreanName = TEXT("마디풀");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Knotgrass_Byproduct.png");
	m_ItemCreateDesc.iCost = 50;

	// 재료 정보
	m_IngredientCreateDesc.eIngredient = INGREDIENT::KNOTGRASS;
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::PLANT_BASED_INGREDIENT;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CKnotgrass_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CKnotgrass_Item* CKnotgrass_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CKnotgrass_Item* pInstance = New CKnotgrass_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CKnotgrass_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CKnotgrass_Item::Clone(void* pArg)
{
	CKnotgrass_Item* pInstance = New CKnotgrass_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CKnotgrass_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKnotgrass_Item::Free(void)
{
	__super::Free();
}
