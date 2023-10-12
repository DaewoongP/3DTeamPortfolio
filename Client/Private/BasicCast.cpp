#include "BasicCast.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"

CBasicCast::CBasicCast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CBasicCast::CBasicCast(const CBasicCast& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CBasicCast::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;
	BEGININSTANCE;

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_MagicTrail_BasicCast_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_MagicTrail_BasicCast_Effect")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/BasicCast/BasicCast.trail"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Hit_Effect_Splash_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Hit_Effect_Splash_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BasicCast/Ht_Effect_Splash"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Hit_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Hit_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Hit"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Glow_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Glow_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BasicCast/Glow"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_DefaultConeBoom_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_DefaultConeBoom_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BasicCast/DefaultConeBoom"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Default_SphereTrace_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Default_SphereTrace_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BasicCast/Default_SphereTrace"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Basic_Cast_Hit_Glow_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Basic_Cast_Hit_Glow_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BasicCast/Hit_Effect"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/BasicTrail/BasicTrail.trail"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Default_Distortion")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Default_Distortion")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Default/Default_Distortion"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;


	return S_OK;
}

HRESULT CBasicCast::Initialize(void* pArg)
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
	return S_OK;
}

void CBasicCast::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBasicCast::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CBasicCast::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	//¸÷ÀÌ¶û Ãæµ¹ÇßÀ¸¸é?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Enemy_Body")) != nullptr)
	{
		Set_MagicBallState(MAGICBALL_STATE_DYING);
	}
	__super::OnCollisionEnter(CollisionEventDesc);
}

void CBasicCast::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CBasicCast::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CBasicCast::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);

	// Light
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	CLight::LIGHTDESC LightDesc;
	LightDesc.eType = CLight::TYPE_POINT;
	LightDesc.fRange = 5.f;
	LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = LightDesc.vDiffuse;
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.vPos = m_pTransform->Get_Position().TransCoord();
	pGameInstance->Add_Light(LightDesc, &m_pLight);
	Safe_Release(pGameInstance);

	return S_OK;
}

void CBasicCast::Ready_Begin()
{
	__super::Ready_Begin();
}

void CBasicCast::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
}

void CBasicCast::Ready_CastMagic()
{
	Ready_SplineMove(m_TrailVec[EFFECT_STATE_MAIN][0]);
	__super::Ready_CastMagic();
}

void CBasicCast::Ready_Dying()
{
	__super::Ready_Dying();
}

void CBasicCast::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CBasicCast::Tick_DrawMagic(_float fTimeDelta)
{
	__super::Tick_DrawMagic(fTimeDelta);
}

void CBasicCast::Tick_CastMagic(_float fTimeDelta)
{
	__super::Tick_CastMagic(fTimeDelta);
	m_ParticleVec[EFFECT_STATE_MAIN][1]->Get_Transform()->LookAt(m_vEndPosition);
	if (m_fLerpAcc != 1)
	{
		m_fLerpAcc += fTimeDelta / m_fLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		m_TrailVec[EFFECT_STATE_MAIN][0]->Spline_Move(m_vSplineLerp[0], m_vStartPosition, m_vEndPosition, m_vSplineLerp[1], m_fLerpAcc);
		m_pTransform->Set_Position(XMVectorLerp(m_vStartPosition, m_vEndPosition, m_fLerpAcc));
	}
	else
	{
		Do_MagicBallState_To_Next();
	}
}

void CBasicCast::Tick_Dying(_float fTimeDelta)
{
	__super::Tick_Dying(fTimeDelta);
}

HRESULT CBasicCast::Add_Components()
{
	m_TrailVec[EFFECT_STATE_WAND].resize(1);
	m_ParticleVec[EFFECT_STATE_WAND].resize(1);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect"),
		TEXT("Com_Wand_Trail"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_WAND][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_BasicCast_Wand_Trail_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Default_SphereTrace_Particle")
		, TEXT("Com_Default_SphereTrace_Particle"), (CComponent**)&m_ParticleVec[EFFECT_STATE_WAND][0])))
	{
		__debugbreak();
		return E_FAIL;
	}

	m_TrailVec[EFFECT_STATE_MAIN].resize(1);
	m_ParticleVec[EFFECT_STATE_MAIN].resize(2);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_MagicTrail_BasicCast_Effect"),
		TEXT("Com_TrailEffect"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_MAIN][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_MagicTrail_BasicCast_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Glow_Particle")
		, TEXT("Com_Glow_Particle"), (CComponent**)&m_ParticleVec[EFFECT_STATE_MAIN][0])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Default_Distortion")
		, TEXT("Com_Default_Distortion"), (CComponent**)&m_ParticleVec[EFFECT_STATE_MAIN][1])))
	{
		__debugbreak();
		return E_FAIL;
	}
	
	
	m_ParticleVec[EFFECT_STATE_HIT].resize(4);
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_DefaultConeBoom_Particle")
		, TEXT("Com_DefaultConeBoom_Particle"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][0])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Basic_Cast_Hit_Glow_Particle")
		, TEXT("Com_Hit_Glow_Particle"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][1])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Hit_Effect_Splash_Particle")
		, TEXT("Com_Hit_Effect_Splash_Particle"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][2])))
	{
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Hit_Particle")
		, TEXT("Com_Hit_Particle"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][3])))
	{
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

CBasicCast* CBasicCast::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CBasicCast* pInstance = New CBasicCast(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CBasicCast");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBasicCast::Clone(void* pArg)
{
	CBasicCast* pInstance = New CBasicCast(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBasicCast");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBasicCast::Free()
{
	__super::Free();
}
