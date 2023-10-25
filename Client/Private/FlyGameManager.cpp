#include "FlyGameManager.h"
#include "Client_Defines.h"
#include "GameInstance.h"

CFlyGameManager::CFlyGameManager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComposite(_pDevice, _pContext)
{
}

CFlyGameManager::CFlyGameManager(const CFlyGameManager& rhs)
	: CComposite(rhs)
{
}

HRESULT CFlyGameManager::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFlyGameManager::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Add_Components : CFlyGameManager");

		return E_FAIL;
	}

	m_fGameTimer = 180.f;

	return S_OK;
}

void CFlyGameManager::Tick(_float fTimeDelta)
{
	if (m_isGameContinue)
	{
		m_fGameTimer -= fTimeDelta;
		if (m_fGameTimer < 0)
		{
			m_isGameContinue = false;
		}
	}
	
	__super::Tick(fTimeDelta);
}

void CFlyGameManager::Late_Tick(_float fTimeDelta)
{
	CComposite::Late_Tick(fTimeDelta);	
}

HRESULT CFlyGameManager::Add_Components()
{
	

	/* Com_Health */
	//if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Health"),
	//	TEXT("Com_Health"), reinterpret_cast<CComponent**>(&m_pHealth), &HealthDesc)))
	//{
	//	MSG_BOX("Failed CFlyGameManager Add_Component : (Com_Health)");
	//	__debugbreak();
	//	return E_FAIL;
	//}
	return S_OK;
}

CFlyGameManager* CFlyGameManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFlyGameManager* pInstance = New CFlyGameManager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CFlyGameManager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CFlyGameManager::Clone(void* pArg)
{
	CFlyGameManager* pInstance = New CFlyGameManager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CFlyGameManager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlyGameManager::Free()
{
	CComposite::Free();

	if (true == m_isCloned)
	{
	}
}
