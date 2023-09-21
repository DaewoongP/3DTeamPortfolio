
#include "Confringo_Explosive_Effect.h"
#include "GameInstance.h"

CConfringo_Explosive_Effect::CConfringo_Explosive_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CConfringo_Explosive_Effect::CConfringo_Explosive_Effect(const CConfringo_Explosive_Effect& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
{
}

HRESULT CConfringo_Explosive_Effect::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	m_iLevel = iLevel;

	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireBallDir_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireBallDir_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/FireBallDir"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireBallB_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireBallB_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/FireBallB"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_Explosive_Particle03")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_Explosive_Particle03")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/FlameRound"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Default_Magic_Boom_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Default_Magic_Boom_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Default_Spell"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_DefaultConeEmit_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_DefaultConeEmit_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/DefaultConeEmit"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;
	return S_OK;
}

HRESULT CConfringo_Explosive_Effect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));
	m_pTransform->Set_Speed(20.f);
	
	m_pParticleTransform = m_pParticleSystem->Get_Transform();
	Safe_AddRef(m_pParticleTransform);
	m_pParticleSystem->Disable();
	for (int i = 0; i < 3; i++)
	{
		m_pExplosiveParticle[i]->Disable();
		m_pExplosiveTransform[i] = m_pExplosiveParticle[i]->Get_Transform();
		Safe_AddRef(m_pExplosiveTransform[i]);
	}

	return S_OK;
}

void CConfringo_Explosive_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CConfringo_Explosive_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CConfringo_Explosive_Effect::Play_Particle(_float3 vPos)
{
	m_pParticleTransform->Set_Position(vPos);
	for (int i = 0; i < 3; i++)
	{
		m_pExplosiveTransform[i]->Set_Position(vPos);
		m_pExplosiveParticle[i]->Play();
	}
	m_pParticleSystem->Play();
}

_bool CConfringo_Explosive_Effect::Get_Particle_State_End()
{
	return (!m_pParticleSystem->IsEnable() && !m_pExplosiveParticle[0]->IsEnable() &&
		!m_pExplosiveParticle[1]->IsEnable() && !m_pExplosiveParticle[2]->IsEnable());
}

void CConfringo_Explosive_Effect::ResetParticle()
{
	m_pParticleSystem->Reset_AllParticles(); 
	for (auto particle : m_pExplosiveParticle) particle->Reset_AllParticles();
}

void CConfringo_Explosive_Effect::Set_Position(_float3 vPos)
{
	m_pTransform->Set_Position(vPos);
}

HRESULT CConfringo_Explosive_Effect::Add_Components()
{
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Default_Magic_Boom_Particle")
		, TEXT("Com_Boom_Particle"), (CComponent**)&m_pParticleSystem)))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Confringo_Explosive_Particle01")
		, TEXT("Com_Explosive_Particle01"), (CComponent**)&m_pExplosiveParticle[0])))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Confringo_Explosive_Particle02")
		, TEXT("Com_Explosive_Particle02"), (CComponent**)&m_pExplosiveParticle[1])))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Confringo_Explosive_Particle03")
		, TEXT("Com_Explosive_Particle03"), (CComponent**)&m_pExplosiveParticle[2])))
		return E_FAIL;
	
	return S_OK;
}

CConfringo_Explosive_Effect* CConfringo_Explosive_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CConfringo_Explosive_Effect* pInstance = New CConfringo_Explosive_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CConfringo_Explosive_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CConfringo_Explosive_Effect::Clone(void* pArg)
{
	CConfringo_Explosive_Effect* pInstance = New CConfringo_Explosive_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CConfringo_Explosive_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CConfringo_Explosive_Effect::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pParticleSystem);
		Safe_Release(m_pParticleTransform);

		for (int i = 0; i < 3; i++)
		{
			Safe_Release(m_pExplosiveParticle[i]);
			Safe_Release(m_pExplosiveTransform[i]);
		}
	}
}
