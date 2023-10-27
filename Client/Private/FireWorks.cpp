#include "FireWorks.h"
#include"GameInstance.h"
#include"Light.h"
#include"Engine_Function.h"

#include"ParticleSystem.h"



CFireWorks::CFireWorks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :CMapObject(pDevice,pContext)
{
}

CFireWorks::CFireWorks(const CFireWorks& rhs) : CMapObject(rhs)
{
}

HRESULT CFireWorks::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFireWorks::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("FireWorks is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if(FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFireWorks::Initialize_Level(_uint iCurrentLevelIndex)
{
	for (_uint i = 0; i < m_vecParticle.size(); ++i)
		m_vecParticle[i]->Play(m_pTransform->Get_Position());

	return S_OK;
}

void CFireWorks::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CFireWorks::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CFireWorks::Render()
{
	return E_NOTIMPL;
}

HRESULT CFireWorks::Add_Components()
{
	m_vecParticle.resize(3);

	if (FAILED(CComposite::Add_Component(LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks_Stem"),
		TEXT("Com_FireWorks_Stem"), reinterpret_cast<CComponent**>(&m_vecParticle[0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks_Stem)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks_Head"),
		TEXT("Com_FireWorks_Head"), reinterpret_cast<CComponent**>(&m_vecParticle[1]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks_Head)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks_SpreadHead"),
		TEXT("Com_FireWorks_SpreadHead"), reinterpret_cast<CComponent**>(&m_vecParticle[2]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks_SpreadHead)");
		__debugbreak();
		return E_FAIL;
	}


	return S_OK;
}

CFireWorks* CFireWorks::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CFireWorks::Clone(void* pArg)
{
	return nullptr;
}

void CFireWorks::Free()
{
	__super::Free();
	if (m_isCloned)
	{	
		for(_uint i = 0 ; i<m_vecParticle.size();++i)
		Safe_Release(m_vecParticle[i]);
	}

}
