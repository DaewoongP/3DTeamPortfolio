#include "..\Public\Dittany_Leaves_Item.h"

CDittany_Leaves_Item::CDittany_Leaves_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CDittany_Leaves_Item::CDittany_Leaves_Item(const CDittany_Leaves_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CDittany_Leaves_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.wstrKoreanName = TEXT("꽃 박하 잎");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Plants/UI_T_Dittany.png");
	m_ItemCreateDesc.iCost = 50;

	// 재료 정보
	m_IngredientCreateDesc.eIngredient = INGREDIENT::DITTANY_LEAVES;
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::PLANT_BASED_INGREDIENT;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDittany_Leaves_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CDittany_Leaves_Item* CDittany_Leaves_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CDittany_Leaves_Item* pInstance = New CDittany_Leaves_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CDittany_Leaves_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDittany_Leaves_Item::Clone(void* pArg)
{
	CDittany_Leaves_Item* pInstance = New CDittany_Leaves_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CDittany_Leaves_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDittany_Leaves_Item::Free(void)
{
	__super::Free();
}
