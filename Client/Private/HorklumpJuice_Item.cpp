#include "HorklumpJuice_Item.h"
#include "GameInstance.h"

CHorklumpJuice_Item::CHorklumpJuice_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CIngredient(pDevice, pContext)
{
}

CHorklumpJuice_Item::CHorklumpJuice_Item(const CHorklumpJuice_Item& rhs)
	: CIngredient(rhs)
{
}

HRESULT CHorklumpJuice_Item::Initialize_Prototype(_uint iLevel)
{
	// ������ ����
	m_ItemCreateDesc.iCost = 100; // ����
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_HorklumpJuice_Item.png"); // UI���

	// ��� ����
	m_IngredientCreateDesc.eIngredient = INGREDIENT::HORKLUMP_JUICE; // ��� ����
	m_IngredientCreateDesc.eIngredientType = INGREDIENT_TYPE::PLANT_BASED_INGREDIENT; // ��� Ÿ��
	
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHorklumpJuice_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CHorklumpJuice_Item::Add_Components()
{
	// ȣŬ���� ���� ���� �߰��� ������Ʈ�� �����Ƿ� �� ���·� ����.
	return S_OK;
}

HRESULT CHorklumpJuice_Item::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// Matrices
	//FAILED_CHECK_RETURN(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr()), E_FAIL)
	//FAILED_CHECK_RETURN(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)), E_FAIL);
	//FAILED_CHECK_RETURN(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ)), E_FAIL);

	// Textures

	// RawValues

	Safe_Release(pGameInstance);

	return S_OK;
}

CHorklumpJuice_Item* CHorklumpJuice_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CHorklumpJuice_Item* pInstance = New CHorklumpJuice_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CHorklumpJuice_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHorklumpJuice_Item::Clone(void* pArg)
{
	CHorklumpJuice_Item* pInstance = New CHorklumpJuice_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CHorklumpJuice_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHorklumpJuice_Item::Free()
{
	__super::Free();
}