#include "FireWorks.h"
#include"GameInstance.h"
#include"Light.h"
#include"Engine_Function.h"

#include"ParticleSystem.h"



CFireWorks::CFireWorks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :CGameObject(pDevice,pContext)
{
}

CFireWorks::CFireWorks(const CFireWorks& rhs) : CGameObject(rhs),m_iLevel(rhs.m_iLevel),m_eType(rhs.m_eType)
{
}

HRESULT CFireWorks::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iLevel = iLevel;
	m_eType = TYPE_UP;
	BEGININSTANCE
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_Stem")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_Stem")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/FireWorks/Stem"), m_iLevel))))
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

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_Head2")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_Head2")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/FireWorks/FireHead2"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_Head3")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_Head3")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/FireWorks/FireHead3"), m_iLevel))))
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

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_SpreadHead2")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_SpreadHead2")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/FireWorks/FireHeadSpread2"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_SpreadHead3")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireWorks_SpreadHead3")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/FireWorks/FireHeadSpread3"), m_iLevel))))
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
	
	m_iRandTime = rand() % 7 + 10;

	m_vecParticle[0]->Get_Transform()->Set_Position(FirePosition);

	m_vecParticle[0]->Play(FirePosition);

	
	
	return S_OK;
}


void CFireWorks::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

		switch (m_eType)
		{
		case Client::CFireWorks::TYPE_UP:
		{	m_fHeight += fTimeDelta * 5.f;
			m_pTransform->Set_Position(_float3(FirePosition.x, FirePosition.y + m_fHeight, FirePosition.z));
			m_vecParticle[0]->Get_Transform()->Set_Position(m_pTransform->Get_Position());
			if (m_fRandHeight <= m_fHeight)
			{
				m_vecParticle[0]->Stop();
			
				m_fHeight = 0.f;
				m_eType = TYPE_BURST;
			}
			}
				break;
		case Client::CFireWorks::TYPE_BURST:
		{
			m_vecParticle[0]->Get_Transform()->Set_Position(FirePosition);
			_uint i = rand() % 7;
			m_vecParticle[i]->Play(m_pTransform->Get_Position());
			m_fTimeAcc = 0.f;
			m_eType = TYPE_READY;
		}
			break;
		case Client::CFireWorks::TYPE_READY:
		{	m_fTimeAcc += fTimeDelta;
			 m_iRandTime = rand() % 5 + 6;
		
		 if (m_iRandTime < m_fTimeAcc)
		 {
			 m_vecParticle[0]->Play(FirePosition);
			 m_fRandHeight = Random_Generator(10.f, 35.f);
			 m_eType = TYPE_UP;
		 }

		}
			break;
		case Client::CFireWorks::TYPE_END:
			break;
		default:
			break;
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
	m_vecParticle.resize(7);

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

	if (FAILED(CComposite::Add_Component(LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks_Head2"),
		TEXT("Com_FireWorks_Head2"), reinterpret_cast<CComponent**>(&m_vecParticle[2]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks_Head2)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks_Head3"),
		TEXT("Com_FireWorks_Head3"), reinterpret_cast<CComponent**>(&m_vecParticle[3]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks_Head3)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks_SpreadHead"),
		TEXT("Com_FireWorks_SpreadHead"), reinterpret_cast<CComponent**>(&m_vecParticle[4]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks_SpreadHead)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks_SpreadHead2"),
		TEXT("Com_FireWorks_SpreadHead2"), reinterpret_cast<CComponent**>(&m_vecParticle[5]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks_SpreadHead2)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks_SpreadHead3"),
		TEXT("Com_FireWorks_SpreadHead3"), reinterpret_cast<CComponent**>(&m_vecParticle[6]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks_SpreadHead3)");
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
