
#include "Confringo_WandDust_Effect.h"
#include "GameInstance.h"

CConfringo_WandDust_Effect::CConfringo_WandDust_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CConfringo_WandDust_Effect::CConfringo_WandDust_Effect(const CConfringo_WandDust_Effect& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
{
}

HRESULT CConfringo_WandDust_Effect::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	m_iLevel = iLevel;

	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_DustExpand_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_DustExpand_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/DustExpand"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_DustConcentration_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_DustConcentration_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/DustConcentration"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;
	return S_OK;
}

HRESULT CConfringo_WandDust_Effect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));
	m_pTransform->Set_Speed(20.f);
	
	m_pExpandTransform = m_pParticleDustExpand->Get_Transform();
	Safe_AddRef(m_pExpandTransform);
	
	m_pConcentrationTransform = m_pParticleDustConcentration->Get_Transform();
	Safe_AddRef(m_pConcentrationTransform);

	m_pParticleDustExpand->Enable();
	m_pParticleDustConcentration->Enable();
	

	return S_OK;
}

void CConfringo_WandDust_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CConfringo_WandDust_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CConfringo_WandDust_Effect::Play_Particle(_float3 vPos)
{
	m_pExpandTransform->Set_Position(vPos);
	m_pConcentrationTransform->Set_Position(vPos);
	m_pParticleDustExpand->Play();
	m_pParticleDustConcentration->Play();
}

void CConfringo_WandDust_Effect::Set_Position(_float3 vPos)
{
	m_pTransform->Set_Position(vPos);
}

HRESULT CConfringo_WandDust_Effect::Add_Components()
{
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Confringo_DustExpand_Particle")
		, TEXT("Com_Expand_Particle"), (CComponent**)&m_pParticleDustExpand)))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Confringo_DustConcentration_Particle")
		, TEXT("Com_Concentration_Particle"), (CComponent**)&m_pParticleDustConcentration)))
		return E_FAIL;

	return S_OK;
}

CConfringo_WandDust_Effect* CConfringo_WandDust_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CConfringo_WandDust_Effect* pInstance = New CConfringo_WandDust_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CConfringo_WandDust_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CConfringo_WandDust_Effect::Clone(void* pArg)
{
	CConfringo_WandDust_Effect* pInstance = New CConfringo_WandDust_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CConfringo_WandDust_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CConfringo_WandDust_Effect::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pParticleDustExpand);
		Safe_Release(m_pParticleDustConcentration);
		Safe_Release(m_pExpandTransform);
		Safe_Release(m_pConcentrationTransform);
	}
}
