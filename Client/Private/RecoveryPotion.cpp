#include "RecoveryPotion.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"

CRecoveryPotion::CRecoveryPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CRecoveryPotion::CRecoveryPotion(const CRecoveryPotion& rhs)
	: CGameObject(rhs)
{
}

HRESULT CRecoveryPotion::Initialize_Prototype(_uint iLevel)
{
	//if (FAILED(__super::Initialize_Prototype(iLevel)))
	//	return E_FAIL;

	//CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//Safe_AddRef(pGameInstance);

	//if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_Component_Texture_UI_T_WoundCleaning")))
	//{
	//	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Texture_UI_T_WoundCleaning")
	//		, CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_WoundCleaning.png"))), E_FAIL);
	//}

	//Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CRecoveryPotion::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CRecoveryPotion::Use()
{
	cout << "아이템 썼어요" << endl;
	//// m_pUITexture는 부모쪽에서 Safe_Release를 불러주고 있음.
	//FAILED_CHECK_RETURN(Add_Component(m_iLevel, TEXT("Prototype_Component_Texture_UI_T_WoundCleaning")
	//	, TEXT("Com_UITexture"), reinterpret_cast<CComponent**>(&m_pUITexture)), E_FAIL);

}

HRESULT CRecoveryPotion::Add_Components()
{
	return S_OK;
}

HRESULT CRecoveryPotion::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);



	Safe_Release(pGameInstance);
	return S_OK;
}

CRecoveryPotion* CRecoveryPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CRecoveryPotion* pInstance = New CRecoveryPotion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CRecoveryPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRecoveryPotion::Clone(void* pArg)
{
	CRecoveryPotion* pInstance = New CRecoveryPotion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CRecoveryPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRecoveryPotion::Free()
{
	__super::Free();
}