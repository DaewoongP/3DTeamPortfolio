#include "WiggenweldPotion.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"

CWiggenweldPotion::CWiggenweldPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CWiggenweldPotion::CWiggenweldPotion(const CWiggenweldPotion& rhs)
	: CItem(rhs)
{
}

void CWiggenweldPotion::Set_HealthCom(CHealth* pHealthCom)
{
	Safe_Release(m_pTargetHealthCom);
	m_pTargetHealthCom = pHealthCom;
	Safe_AddRef(m_pTargetHealthCom);
}

HRESULT CWiggenweldPotion::Initialize_Prototype(_uint iLevel, const _tchar* pUIImagePath)
{
	if (FAILED(__super::Initialize_Prototype(iLevel, pUIImagePath)))
		return E_FAIL;

	m_iLevel = iLevel;
	m_eItemType = ITEMTYPE::POTION;

	return S_OK;
}

HRESULT CWiggenweldPotion::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTargetHealthCom = reinterpret_cast<INIT_DESC*>(pArg)->pHealthCom;
	Safe_AddRef(m_pTargetHealthCom);
	return S_OK;
}

void CWiggenweldPotion::Use(_float3 vPlayPos,void* ExtraValue)
{
	m_pTargetHealthCom->Heal(m_fRecoveryAmount);
	// m_pEffect(vPlayPos);
}

HRESULT CWiggenweldPotion::Add_Components()
{


	return S_OK;
}

CWiggenweldPotion* CWiggenweldPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, const _tchar* pUIImagePath)
{
	CWiggenweldPotion* pInstance = New CWiggenweldPotion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel, pUIImagePath)))
	{
		MSG_BOX("Failed to Created CWiggenweldPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWiggenweldPotion::Clone(void* pArg)
{
	CWiggenweldPotion* pInstance = New CWiggenweldPotion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CWiggenweldPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWiggenweldPotion::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pTargetHealthCom);
	}
}