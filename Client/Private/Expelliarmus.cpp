#include "Expelliarmus.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"

CExpelliarmus::CExpelliarmus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CExpelliarmus::CExpelliarmus(const CExpelliarmus& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CExpelliarmus::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	BEGININSTANCE;
	//¸ÞÀÎ ÀÌÆåÆ®
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Expelliarmus_Trail")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Expelliarmus_Trail")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/Expelliarmus/Expelliarmus.trail"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Expelliarmus_Dust_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Expelliarmus_Dust_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Expelliarmus/ChaseDust"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	//Á¾·á ÀÌÆåÆ®
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Expelliarmus_HitEffect01")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Expelliarmus_HitEffect01")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Expelliarmus/HitEffect01"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Expelliarmus_HitEffect02")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Expelliarmus_HitEffect02")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Expelliarmus/HitEffect02"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	//¿Ïµå
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/BasicTrail/BasicCast/BasicTrail.trail"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	ENDINSTANCE;
	return S_OK;
}

HRESULT CExpelliarmus::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("Failed Player Initialize");

		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Player Add_Components");

		return E_FAIL;
	}
	
	_float3 vColor = _float3(204.f / 255.f, 204.f / 255.f, 0);
	for (_uint j = 0; j < m_TrailVec[EFFECT_STATE_WAND].size(); j++)
	{
		m_TrailVec[EFFECT_STATE_WAND].data()[j]->Set_Trail_HeadColor(vColor);
		m_TrailVec[EFFECT_STATE_WAND].data()[j]->Set_Trail_TailColor(vColor);
	}
	return S_OK;
}

void CExpelliarmus::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CExpelliarmus::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CExpelliarmus::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
	//¸÷ÀÌ¶û Ãæµ¹ÇßÀ¸¸é?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Enemy_Body")) != nullptr)
	{
		Set_MagicBallState(MAGICBALL_STATE_DYING);
	}
}

void CExpelliarmus::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CExpelliarmus::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CExpelliarmus::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	return S_OK;
}

void CExpelliarmus::Ready_Begin()
{
	__super::Ready_Begin();
}

void CExpelliarmus::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
}

void CExpelliarmus::Ready_CastMagic()
{
	Ready_SplineSpinMove(m_TrailVec[EFFECT_STATE_MAIN].data()[0],_float2(0.2f, 0.20f),0.5f);
	__super::Ready_CastMagic();
}

void CExpelliarmus::Ready_Dying()
{
	__super::Ready_Dying();
}

void CExpelliarmus::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CExpelliarmus::Tick_DrawMagic(_float fTimeDelta)
{
	__super::Tick_DrawMagic(fTimeDelta);
}

void CExpelliarmus::Tick_CastMagic(_float fTimeDelta)
{
	__super::Tick_CastMagic(fTimeDelta);
	if (m_fLerpAcc != 1)
	{
		m_fLerpAcc += fTimeDelta / m_fLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		m_TrailVec[EFFECT_STATE_MAIN].data()[0]->Spline_Spin_Move(m_vSplineLerp[0], m_vStartPosition, m_vEndPosition, m_vSplineLerp[1], m_vSpinWeight, m_fSpinSpeed, m_fLerpAcc);
		m_ParticleVec[EFFECT_STATE_MAIN].data()[0]->Get_Transform()->Set_Position(m_TrailVec[EFFECT_STATE_MAIN].data()[0]->Get_Transform()->Get_Position());
		m_pTransform->Set_Position(m_TrailVec[EFFECT_STATE_MAIN].data()[0]->Get_Transform()->Get_Position());
	}
	else 
	{
		Do_MagicBallState_To_Next();
	}
}

void CExpelliarmus::Tick_Dying(_float fTimeDelta)
{
	__super::Tick_Dying(fTimeDelta);
}


HRESULT CExpelliarmus::Add_Components()
{
	m_TrailVec[EFFECT_STATE_WAND].resize(1);
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")
		, TEXT("Com_Wand_Trail_Effect"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_WAND][0]))))
	{
		__debugbreak();
		return E_FAIL;
	}

	m_TrailVec[EFFECT_STATE_MAIN].resize(1);
	m_ParticleVec[EFFECT_STATE_MAIN].resize(1);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Expelliarmus_Trail"),
		TEXT("Com_Trail"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_MAIN][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Confringo_Trail)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Expelliarmus_Dust_Effect")
		, TEXT("Com_Main_Dust"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][0]))))
	{
		__debugbreak();
		return E_FAIL;
	}

	m_ParticleVec[EFFECT_STATE_HIT].resize(2);
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Expelliarmus_HitEffect01")
		, TEXT("Com_Explosive_Particle01"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][0]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Expelliarmus_HitEffect02")
		, TEXT("Com_Explosive_Particle02"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][1]))))
	{
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

CExpelliarmus* CExpelliarmus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CExpelliarmus* pInstance = new CExpelliarmus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CExpelliarmus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CExpelliarmus::Clone(void* pArg)
{
	CExpelliarmus* pInstance = new CExpelliarmus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CExpelliarmus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExpelliarmus::Free()
{
	__super::Free();
}

