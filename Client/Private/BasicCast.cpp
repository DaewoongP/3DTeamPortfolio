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
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	m_iLevel = iLevel;
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

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_DefaultConeEmit_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_DefaultConeEmit_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BasicCast/DefaultConeEmit"), m_iLevel))))
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

	if (FAILED(Add_Effect()))
	{
		MSG_BOX("Failed Player Add_Effect");

		return E_FAIL;
	}
	m_pHitEffect->Disable();
	m_pHitGlowEffect->Disable();
	m_pWandEffect->Disable();
	m_pFinalAttackEffect->Disable();
	m_pMainTrail->Disable();
	m_WandTrail->Disable();
	m_pMainGlow->Disable();
	m_pHitSplashEffect->Disable();

	m_pHitEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f,2.f));
	m_pHitGlowEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f,2.f));
	m_pWandEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f,2.f));
	m_pFinalAttackEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f,2.f));
	m_pMainGlow->Get_Transform()->Set_Scale(_float3(2.f, 2.f,2.f));
	m_pHitSplashEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f,2.f));

	
	return S_OK;
}

void CBasicCast::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_pWandEffect->Get_Transform()->Set_Position(vWandPosition);
	m_WandTrail->Get_Transform()->Set_Position(vWandPosition);
	m_pMainGlow->Get_Transform()->Set_Position(vWandPosition);
}

void CBasicCast::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CBasicCast::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	//몹이랑 충돌했으면?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag,TEXT("Enemy_Body")) != nullptr)
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
	
	m_pHitEffect->Disable();
	m_pHitGlowEffect->Disable();
	m_pWandEffect->Disable();
	m_pFinalAttackEffect->Disable();
	m_pMainTrail->Disable();
	m_WandTrail->Disable();
	m_pMainGlow->Disable();
	m_pHitSplashEffect->Disable();
	m_fLerpAcc = 0.0f;
	return S_OK;
}

void CBasicCast::Ready_Begin()
{
}

void CBasicCast::Ready_DrawMagic()
{
	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_WandTrail->Enable(vWandPosition);
	m_pWandEffect->Enable(vWandPosition);
}

void CBasicCast::Ready_CastMagic()
{
	Ready_SplineMove(m_pMainTrail);
	

	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_pMainGlow->Enable(vWandPosition);
	m_pMainTrail->Enable(vWandPosition);
	m_pMainGlow->Play(vWandPosition);
}

void CBasicCast::Ready_Dying()
{
	m_WandTrail->Disable();
	m_pWandEffect->Disable();

	m_pHitGlowEffect->Enable(m_pMainTrail->Get_Transform()->Get_Position());
	m_pHitEffect->Enable(m_pMainTrail->Get_Transform()->Get_Position());
	m_pHitSplashEffect->Enable(m_pMainTrail->Get_Transform()->Get_Position());

	m_pHitGlowEffect->Play(m_pMainTrail->Get_Transform()->Get_Position());
	m_pHitEffect->Play(m_pMainTrail->Get_Transform()->Get_Position());
	m_pHitSplashEffect->Play(m_pMainTrail->Get_Transform()->Get_Position());
}

void CBasicCast::Tick_Begin(_float fTimeDelta)
{
	//베이직 캐스트는 비긴 없습니다.
	Do_MagicBallState_To_Next();
}

void CBasicCast::Tick_DrawMagic(_float fTimeDelta)
{
	Do_MagicBallState_To_Next();
}

void CBasicCast::Tick_CastMagic(_float fTimeDelta)
{
	if (m_fLerpAcc != 1)
	{
		m_fLerpAcc += fTimeDelta / m_MagicBallDesc.fInitLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		m_pMainTrail->Spline_Move(m_vSplineLerp[0], m_vStartPostion, m_vTargetPosition, m_vSplineLerp[1], m_fLerpAcc);
		m_pTransform->Set_Position(m_pMainTrail->Get_Transform()->Get_Position());
	}
	else
	{
		Do_MagicBallState_To_Next();
	}
	m_pTransform->Set_Position(m_pMainTrail->Get_Transform()->Get_Position());
}

void CBasicCast::Tick_Dying(_float fTimeDelta)
{
	//사망하러 가겠습니다.
	if (!m_pHitEffect->IsEnable()&&
		!m_pHitGlowEffect->IsEnable())
	{
		Do_MagicBallState_To_Next();
	}
}

HRESULT CBasicCast::Add_Components()
{
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_DefaultConeBoom_Particle")
		, TEXT("Com_DefaultConeBoom_Particle"), (CComponent**)&m_pHitEffect)))
	{
		__debugbreak();
		return E_FAIL;
	}
	
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Default_SphereTrace_Particle")
		, TEXT("Com_Default_SphereTrace_Particle"), (CComponent**)&m_pWandEffect)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Basic_Cast_Hit_Glow_Particle")
		, TEXT("Com_Hit_Glow_Particle"), (CComponent**)&m_pHitGlowEffect)))
	{
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_DefaultConeEmit_Particle")
		, TEXT("Com_DefaultConeEmit_Particle"), (CComponent**)&m_pFinalAttackEffect)))
	{
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Glow_Particle")
		, TEXT("Com_Glow_Particle"), (CComponent**)&m_pMainGlow)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Hit_Effect_Splash_Particle")
		, TEXT("Com_Hit_Effect_Splash_Particle"), (CComponent**)&m_pHitSplashEffect)))
	{
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CBasicCast::Add_Effect()
{
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_MagicTrail_BasicCast_Effect"),
		TEXT("Com_TrailEffect"), reinterpret_cast<CComponent**>(&m_pMainTrail))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_MagicTrail_BasicCast_Effect)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect"),
		TEXT("Com_Wand_Trail"), reinterpret_cast<CComponent**>(&m_WandTrail))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_BasicCast_Wand_Trail_Effect)");
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
	if (true == m_isCloned)
	{
		Safe_Release(m_WandTrail);
		Safe_Release(m_pMainTrail);
		Safe_Release(m_pHitEffect);
		Safe_Release(m_pHitGlowEffect);
		Safe_Release(m_pWandEffect);
		Safe_Release(m_pFinalAttackEffect);
		Safe_Release(m_pMainGlow);
		Safe_Release(m_pHitSplashEffect);
	}
	
}
