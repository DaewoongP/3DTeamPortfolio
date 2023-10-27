#include "FireWorks.h"
#include"GameInstance.h"
#include"Light.h"
#include"Engine_Function.h"

#include"ParticleSystem.h"



CFireWorks::CFireWorks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :CGameObject(pDevice,pContext)
{
}

CFireWorks::CFireWorks(const CFireWorks& rhs) : CGameObject(rhs),m_iLevel(rhs.m_iLevel)
{
}

HRESULT CFireWorks::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iLevel = iLevel;
	//m_eType = eType;
	BEGININSTANCE
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_Stem")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_Stem")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/FireWorks/FireStem"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_Head")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_Head")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/FireWorks/FireHead"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_SpreadHead")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_SpreadHead")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/FireWorks/FireHeadSpread"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	ENDINSTANCE

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
	Matrix* pMatrix = reinterpret_cast<Matrix*>(pArg);

	m_pTransform->Set_WorldMatrix(*pMatrix);
	FirePosition = m_pTransform->Get_Position();

	//for (_uint i = 0; i < m_vecParticle.size(); ++i)
	//{
	//	m_vecParticle[i]->Play(FirePosition);
	//}
	m_vecParticle[0]->Play(FirePosition);

	//Play(FirePosition);

	//for (_uint i = 0; i < m_vecParticle.size(); ++i)
	//	m_vecParticle[i]->Play(m_pTransform->Get_Position());
	
	return S_OK;
}


void CFireWorks::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_fHeight += fTimeDelta;
	m_pTransform->Set_Position(_float3(FirePosition.x, FirePosition.y+ m_fHeight, FirePosition.z));
	_uint i = rand() % 2 + 1;

	
	if (1.f <= m_fHeight)
	{
		m_vecParticle[0]->Stop();
		m_vecParticle[i]->Play(m_pTransform->Get_Position());
		m_fHeight = 0.f;
	}
	

}

void CFireWorks::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CFireWorks::Play(_float3 vPosition)
{
	//switch (i)
	//{
	//case 1:
	//	m_vecParticle[0]->Play(_float3(vPosition.x, m_fHeight, vPosition.z));
	//
	//	if(10.f<=m_fHeight)
	//	m_vecParticle[1]->Play(_float3(vPosition.x, vPosition.y+10.f, vPosition.z));
	//
	//	break;
	//case 2:
	//
	//	m_vecParticle[0]->Play(_float3(vPosition.x, m_fHeight, vPosition.z));
	//	if (10.f <= m_fHeight)
	//		m_vecParticle[2]->Play(_float3(vPosition.x, vPosition.y + 10.f, vPosition.z));
	//	break;
	//default:
	//	break;
	//}

	m_pTransform->Set_Position(vPosition);
	
	return S_OK;
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

CFireWorks* CFireWorks::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CFireWorks* pInstance = New CFireWorks(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Create Brath_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFireWorks::Clone(void* pArg)
{
	CFireWorks* pInstance = New CFireWorks(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CFireWorks");
		Safe_Release(pInstance);
	}
	return pInstance;
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
