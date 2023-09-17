
#include "Default_Magic_Effect.h"
#include "GameInstance.h"

CDefault_Magic_Effect::CDefault_Magic_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDefault_Magic_Effect::CDefault_Magic_Effect(const CDefault_Magic_Effect& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
{
}

HRESULT CDefault_Magic_Effect::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	m_iLevel = iLevel;

	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Default_Magic_Trail")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Default_Magic_Trail")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../"), m_iLevel))))
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

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Default_Magic_Trace_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Default_Magic_Trace_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Default_Spell/Default_Magic_Trace"), m_iLevel))))
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

HRESULT CDefault_Magic_Effect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));
	m_pTransform->Set_Speed(20.f);
	
	m_pParticleTransform = m_pParticleSystem->Get_Transform();
	Safe_AddRef(m_pParticleTransform);

	m_pTrailTransform = m_pTrail->Get_Transform();
	Safe_AddRef(m_pTrailTransform);

	m_pTraceParticleTransform = m_pTraceParticle->Get_Transform();
	Safe_AddRef(m_pTraceParticleTransform);

	m_pConeEmitTransform = m_pConeEmitParticle->Get_Transform();
	Safe_AddRef(m_pConeEmitTransform);

	return S_OK;
}

void CDefault_Magic_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	SHAPE_MODULE& ShapeModule = m_pTraceParticle->Get_ShapeModuleRef();
	if(XMVectorGetX(XMVector3Length(m_pTransform->Get_Position()- m_vPrevPos)))
		ShapeModule.ShapeMatrix.MatrixLookAt(m_pTransform->Get_Position(), m_vPrevPos, _float3(0.f, 1.f, 0.f));
}

void CDefault_Magic_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	m_pTrailTransform->Set_Position(m_pTransform->Get_Position());
	m_vPrevPos = m_pTransform->Get_Position();
}

void CDefault_Magic_Effect::Play_Particle(_float3 vPos)
{
	m_pParticleTransform->Set_Position(vPos);
	m_pParticleSystem->Play();
}

void CDefault_Magic_Effect::Play_ConeEmit(_float3 vPos)
{
	m_pConeEmitTransform->Set_Position(vPos);
	m_pConeEmitParticle->Play();
}

void CDefault_Magic_Effect::Set_Position(_float3 vPos)
{
	m_pTransform->Set_Position(vPos);
}

void CDefault_Magic_Effect::Enable_Trail(_bool _isEnable)
{
	(true == _isEnable) ? m_pTrail->Enable() : m_pTrail->Disable();
}

HRESULT CDefault_Magic_Effect::Reset_Trail()
{
	if (m_pTrail == nullptr)
	{
		MSG_BOX("Failed to Reset_Trail");
		return S_OK;
	}
		
	return   m_pTrail->Reset_Trail(); 
}
void CDefault_Magic_Effect::Enable_TraceParticle(_bool _isEnable)
{
	(true == _isEnable) ? m_pTraceParticle->Enable() : m_pTraceParticle->Disable();
}

HRESULT CDefault_Magic_Effect::Add_Components()
{
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Default_Magic_Trail")
		, TEXT("Com_Trail"), (CComponent**)&m_pTrail)))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Default_Magic_Boom_Particle")
		, TEXT("Com_Boom_Particle"), (CComponent**)&m_pParticleSystem)))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Default_Magic_Trace_Particle")
		, TEXT("Com_Trace_Particle"), (CComponent**)&m_pTraceParticle)))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_DefaultConeEmit_Particle")
		, TEXT("Com_DefaultConeEmit_Particle"), (CComponent**)&m_pConeEmitParticle)))
		return E_FAIL;
	
	return S_OK;
}

CDefault_Magic_Effect* CDefault_Magic_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CDefault_Magic_Effect* pInstance = New CDefault_Magic_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CDefault_Magic_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDefault_Magic_Effect::Clone(void* pArg)
{
	CDefault_Magic_Effect* pInstance = New CDefault_Magic_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDefault_Magic_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDefault_Magic_Effect::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pTrail);
		Safe_Release(m_pParticleSystem);
		Safe_Release(m_pTraceParticle);
		Safe_Release(m_pConeEmitParticle);

		Safe_Release(m_pParticleTransform);
		Safe_Release(m_pTrailTransform);
		Safe_Release(m_pTraceParticleTransform);
		Safe_Release(m_pConeEmitTransform);
	}
}
