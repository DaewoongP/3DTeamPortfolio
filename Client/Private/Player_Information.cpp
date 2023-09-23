#include "Player_Information.h"
#include "Client_Defines.h"
#include "GameInstance.h"

CPlayer_Information::CPlayer_Information(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComposite(_pDevice, _pContext)
{
}

CPlayer_Information::CPlayer_Information(const CPlayer_Information& rhs)
	: CComposite(rhs)
{
}

HRESULT CPlayer_Information::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Information::Initialize(void* pArg)
{
	m_vecItemCount.resize(PLAYER_ITEM_END);

	return S_OK;
}

void CPlayer_Information::Tick(_float fTimeDelta)
{
	
}

void CPlayer_Information::Late_Tick(_float fTimeDelta)
{
	
}

HRESULT CPlayer_Information::Add_Components()
{
	CHealth::HEALTHDESC HealthDesc;

	HealthDesc.iMaxHP = 100;

	/* Com_Health */
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Health"),
		TEXT("Com_Health"), reinterpret_cast<CComponent**>(&m_pHealth), &HealthDesc)))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Health)");
		return E_FAIL;
	}

	HealthDesc.iMaxHP = 100;

	/* Com_Finisher */
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Health"),
		TEXT("Com_Finisher"), reinterpret_cast<CComponent**>(&m_pFinisher), &HealthDesc)))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Finisher)");
		return E_FAIL;
	}


	return S_OK;
}

CPlayer_Information* CPlayer_Information::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Information* pInstance = New CPlayer_Information(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPlayer_Information");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CPlayer_Information::Clone(void* pArg)
{
	CPlayer_Information* pInstance = New CPlayer_Information(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPlayer_Information");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Information::Free()
{
	CComposite::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pHealth);
		Safe_Release(m_pFinisher);
	}
}
