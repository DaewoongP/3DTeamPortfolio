#include "Levioso.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"
#include "Wingardium_Effect.h"

CLevioso::CLevioso(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CLevioso::CLevioso(const CLevioso& rhs)
	: CMagicBall(rhs)
{
}

void CLevioso::TrailAction(_float3 vPosition, _float fTimeDelta)
{
	m_pWingardiumEffect->TrailAction(vPosition, fTimeDelta);
	m_fWingardiumEffectDeadTimer = 0.3f;

}

HRESULT CLevioso::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Wingardium_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Wingardium_Effect"),
			CWingardium_Effect::Create(m_pDevice, m_pContext, iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_MagicTraill_Winga_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_MagicTraill_Winga_Effect"),
			CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/Wingardium/Wingardium.trail"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/BasicTrail/BasicTrail.trail"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_Wand_Glow_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_Wand_Glow_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Default/Default_WandGlow/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Levioso_TraceDust_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Levioso_TraceDust_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Levioso/MainParticle/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Levioso_HitDust_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Levioso_HitDust_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Levioso/HitMain/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Levioso_HitExplosion_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Levioso_HitExplosion_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Levioso/HitBamm/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevioso::Initialize(void* pArg)
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
	m_CollisionDesc.Action = bind(&CLevioso::TrailAction, this, placeholders::_1, placeholders::_2);

	m_pMainTrail->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pTraceDustEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pWingardiumEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pHitDustEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pHitExplosionEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pWandTrail->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pWandGlow->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));

	return S_OK;
}

void CLevioso::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_pWandTrail->Get_Transform()->Set_Position(vWandPosition);
	m_pWandGlow->Get_Transform()->Set_Position(vWandPosition);
}

void CLevioso::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CLevioso::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	//¸÷ÀÌ¶û Ãæµ¹ÇßÀ¸¸é?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Enemy_Body")) != nullptr)
	{
		Set_MagicBallState(MAGICBALL_STATE_DYING);
	}

	__super::OnCollisionEnter(CollisionEventDesc);
}

void CLevioso::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CLevioso::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CLevioso::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	m_fWingardiumEffectDeadTimer = 0.3f;
	m_fLerpAcc = 0.0f;
	m_pMainTrail->Disable();
	m_pWingardiumEffect->Disable();
	m_pWandTrail->Disable();
	m_pWandGlow->Disable();
	m_pTraceDustEffect->Disable();
	m_pHitDustEffect->Disable();
	m_pHitExplosionEffect->Disable();
	return S_OK;
}

void CLevioso::Ready_Begin()
{
	
}

void CLevioso::Ready_DrawMagic()
{
	m_pWandTrail->Enable();
	m_pWandGlow->Enable();

	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_pWandTrail->Get_Transform()->Set_Position(vWandPosition);
	m_pWandGlow->Get_Transform()->Set_Position(vWandPosition);
	m_pWandTrail->Reset_Trail(_float3(vWandPosition) + _float3(0, 0.5f, 0), _float3(vWandPosition) + _float3(0, -0.5f, 0));
}

void CLevioso::Ready_CastMagic()
{
	m_pMainTrail->Enable();
	m_pTraceDustEffect->Enable();

	Ready_SpinMove(m_pMainTrail,_float2(1.f,0.f),30.f);
	m_pTraceDustEffect->Get_EmissionModuleRef().Setting_PrevPos(m_vStartPostion);
	m_pTraceDustEffect->Play(m_vStartPostion);
	m_pTraceDustEffect->Get_Transform()->Set_Position(m_vStartPostion);
	
}

void CLevioso::Ready_Dying()
{
	m_pMainTrail->Disable();
	m_pTraceDustEffect->Disable();
	m_pWingardiumEffect->SetActionTrigger(true);
	m_pWingardiumEffect->Enable();

	m_pHitDustEffect->Enable();
	m_pHitExplosionEffect->Enable();
	
	m_pHitDustEffect->Play(m_pMainTrail->Get_Transform()->Get_Position());
	m_pHitExplosionEffect->Play(m_pMainTrail->Get_Transform()->Get_Position());
}

void CLevioso::Tick_Begin(_float fTimeDelta)
{
	Do_MagicBallState_To_Next();
}

void CLevioso::Tick_DrawMagic(_float fTimeDelta)
{
	Do_MagicBallState_To_Next();
}

void CLevioso::Tick_CastMagic(_float fTimeDelta)
{
	if (m_fLerpAcc != 1)
	{
		m_fLerpAcc += fTimeDelta / m_MagicBallDesc.fInitLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		m_pMainTrail->Spin_Move(m_vTargetPosition, m_MagicBallDesc.vStartPosition,m_vSpinWeight,m_fSpinSpeed, m_fLerpAcc);
		m_pTransform->Set_Position(m_pMainTrail->Get_Transform()->Get_Position());
		m_pTraceDustEffect->Get_Transform()->Set_Position(m_pMainTrail->Get_Transform()->Get_Position());
	}
	else
	{
		Do_MagicBallState_To_Next();
	}
}

void CLevioso::Tick_Dying(_float fTimeDelta)
{
	m_fWingardiumEffectDeadTimer -= fTimeDelta;
	if (m_fWingardiumEffectDeadTimer < 0)
	{
		Do_MagicBallState_To_Next();
	}
}

HRESULT CLevioso::Add_Components()
{
	return S_OK;
}

HRESULT CLevioso::Add_Effect()
{
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_MagicTraill_Winga_Effect"),
		TEXT("Com_Effect"), reinterpret_cast<CComponent**>(&m_pMainTrail))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_MagicTraill_Winga_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Wingardium_Effect"),
		TEXT("Com_WingradiumEffect"), reinterpret_cast<CComponent**>(&m_pWingardiumEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Wingardium_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Defatul_Wand_Glow_Effect"),
		TEXT("Com_WandGlow"), reinterpret_cast<CComponent**>(&m_pWandGlow))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Defatul_Wand_Glow_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect"),
		TEXT("Com_Wand_Trail"), reinterpret_cast<CComponent**>(&m_pWandTrail))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_BasicCast_Wand_Trail_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Levioso_TraceDust_Effect"),
		TEXT("Com_TraceDust"), reinterpret_cast<CComponent**>(&m_pTraceDustEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Levioso_TraceDust_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Levioso_HitDust_Effect"),
		TEXT("Com_HitDust_Effect"), reinterpret_cast<CComponent**>(&m_pHitDustEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Levioso_HitDust_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Levioso_HitExplosion_Effect"),
		TEXT("Com_HitExplosion_Effect"), reinterpret_cast<CComponent**>(&m_pHitExplosionEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Levioso_HitExplosion_Effect)");
		__debugbreak();
		return E_FAIL;
	}



	return S_OK;
}

CLevioso* CLevioso::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CLevioso* pInstance = New CLevioso(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CLevioso");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLevioso::Clone(void* pArg)
{
	CLevioso* pInstance = New CLevioso(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CLevioso");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevioso::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pMainTrail);
		Safe_Release(m_pTraceDustEffect);
		Safe_Release(m_pHitDustEffect);
		Safe_Release(m_pHitExplosionEffect);
		Safe_Release(m_pWingardiumEffect);
		Safe_Release(m_pWandTrail);
		Safe_Release(m_pWandGlow);
	}
}
