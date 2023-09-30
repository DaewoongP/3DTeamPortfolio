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

void CLevioso::TrailAction (_float3 vPosition, _float fTimeDelta)
{
	m_pWingardiumEffect->TrailAction(vPosition, fTimeDelta);
	m_fWingardiumEffectDeadTimer = 0.3f;

}

HRESULT CLevioso::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
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
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_MagicTrail_Winga_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_MagicTrail_Winga_Effect"),
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
	return S_OK;
}

void CLevioso::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
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
	m_pWingardiumEffect->Disable();
	return S_OK;
}

void CLevioso::Ready_Begin()
{
	__super::Ready_Begin();
}

void CLevioso::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
}

void CLevioso::Ready_CastMagic()
{
	Ready_SpinMove(m_TrailVec[EFFECT_STATE_MAIN].data()[0],_float2(1.f,0.f),0.5f);
	__super::Ready_CastMagic();
}

void CLevioso::Ready_Dying()
{
	m_pWingardiumEffect->SetActionTrigger(true);
	m_pWingardiumEffect->Enable();
	__super::Ready_Dying();
}

void CLevioso::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CLevioso::Tick_DrawMagic(_float fTimeDelta)
{
	__super::Tick_DrawMagic(fTimeDelta);
}

void CLevioso::Tick_CastMagic(_float fTimeDelta)
{
	__super::Tick_CastMagic(fTimeDelta);

	if (m_fLerpAcc != 1)
	{
		m_fLerpAcc += fTimeDelta / m_fLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		m_TrailVec[EFFECT_STATE_MAIN].data()[0]->Spin_Move(m_vEndPosition, m_vStartPosition,m_vSpinWeight,m_fSpinSpeed, m_fLerpAcc);
		m_pTransform->Set_Position(m_TrailVec[EFFECT_STATE_MAIN].data()[0]->Get_Transform()->Get_Position());
		m_ParticleVec[EFFECT_STATE_MAIN].data()[0]->Get_Transform()->Set_Position(m_TrailVec[EFFECT_STATE_MAIN].data()[0]->Get_Transform()->Get_Position());
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
	m_TrailVec[EFFECT_STATE_WAND].resize(1);
	m_ParticleVec[EFFECT_STATE_WAND].resize(1);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect"),
		TEXT("Com_Wand_Trail"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_WAND][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_BasicCast_Wand_Trail_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Defatul_Wand_Glow_Effect"),
		TEXT("Com_WandGlow"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_WAND][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Defatul_Wand_Glow_Effect)");
		__debugbreak();
		return E_FAIL;
	}

	m_TrailVec[EFFECT_STATE_MAIN].resize(1);
	m_ParticleVec[EFFECT_STATE_MAIN].resize(1);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_MagicTrail_Winga_Effect"),
		TEXT("Com_Effect"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_MAIN][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_MagicTrail_Winga_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Levioso_TraceDust_Effect"),
		TEXT("Com_TraceDust"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Levioso_TraceDust_Effect)");
		__debugbreak();
		return E_FAIL;
	}

	m_ParticleVec[EFFECT_STATE_HIT].resize(2);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Levioso_HitDust_Effect"),
		TEXT("Com_HitDust_Effect"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Levioso_HitDust_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Levioso_HitExplosion_Effect"),
		TEXT("Com_HitExplosion_Effect"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][1]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Levioso_HitExplosion_Effect)");
		__debugbreak();
		return E_FAIL;
	}

	//º°µµ
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Wingardium_Effect"),
		TEXT("Com_WingradiumEffect"), reinterpret_cast<CComponent**>(&m_pWingardiumEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Wingardium_Effect)");
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
		Safe_Release(m_pWingardiumEffect);
	}
}
