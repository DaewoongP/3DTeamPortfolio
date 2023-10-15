#include "Accio.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"
#include "Wingardium_Effect.h"

CAccio::CAccio(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CAccio::CAccio(const CAccio& rhs)
	: CMagicBall(rhs)
{
}

void CAccio::TrailAction (void* pArg)
{
	ACCIO_TICKDESC* desc = static_cast<ACCIO_TICKDESC*>(pArg);
	m_pWingardiumEffect->TrailAction(desc->vPosition, desc->fTimeDelta);
	m_fWingardiumEffectDeadTimer = 0.3f;
}

HRESULT CAccio::Initialize_Prototype(_uint iLevel)
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
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_RainBow")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_RainBow")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Default/Default_WandFlare/RainBow/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_Spread")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_Spread")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Default/Default_WandFlare/Spread/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_WandGlow")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_WandGlow")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Default/Default_WandFlare/WandGlow/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_WandGlowLarge")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_WandGlowLarge")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Default/Default_WandFlare/WandGlow_Large/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Accio_HitMain")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Accio_HitMain")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Accio/Hit_Main/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Accio_Hit_Wave")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Accio_Hit_Wave")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Accio/Hit_Wave/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Accio_Hit_Wind")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Accio_Hit_Wind")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Accio/Hit_Wind/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;

	return S_OK;
}

HRESULT CAccio::Initialize(void* pArg)
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
	m_CollisionDesc.Action = bind(&CAccio::TrailAction, this, placeholders::_1);

	//트레일 색상변경부
	_float3 vColor = _float3(255.f / 255.f, 255.f / 255.f, 204.f / 255.f);
	m_pWingardiumEffect->Set_Color(vColor, vColor);
	for (int i = 0; i < EFFECT_STATE_END; i++)
	{
		for (_uint j = 0; j < m_TrailVec[i].size(); j++)
		{
			m_TrailVec[i].data()[j]->Set_Trail_HeadColor(vColor);
			m_TrailVec[i].data()[j]->Set_Trail_TailColor(vColor);
		}
		
	}

	Ready_Shake(30.0f, 2.0f, 0.1f);

	return S_OK;
}

void CAccio::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CAccio::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CAccio::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	//몹이랑 충돌했으면?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Enemy_Body")) != nullptr)
	{
		Set_MagicBallState(MAGICBALL_STATE_DYING);

#pragma region 카메라 쉐이크
		BEGININSTANCE;

		pGameInstance->Set_Shake(
			CCamera_Manager::SHAKE_TYPE_TRANSLATION,
			CCamera_Manager::SHAKE_AXIS_LOOK,
			CEase::IN_EXPO,
			5.0f,
			0.2f,
			-Shake_Power(CollisionEventDesc.pOtherTransform->Get_Position()),
			CCamera_Manager::SHAKE_POWER_DECRECENDO);

		ENDINSTANCE;
#pragma endregion
	}

	__super::OnCollisionEnter(CollisionEventDesc);
}

void CAccio::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CAccio::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CAccio::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	m_fWingardiumEffectDeadTimer = 0.3f;
	m_pWingardiumEffect->Disable();
	return S_OK;
}

void CAccio::Ready_Begin()
{
	__super::Ready_Begin();
}

void CAccio::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
}

void CAccio::Ready_CastMagic()
{
	Ready_SpinMove(m_TrailVec[EFFECT_STATE_MAIN].data()[0],_float2(1.f,0.f),0.5f);
	__super::Ready_CastMagic();
}

void CAccio::Ready_Dying()
{
	m_pWingardiumEffect->SetActionTrigger(true);
	m_pWingardiumEffect->Enable();
	__super::Ready_Dying();
}

void CAccio::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CAccio::Tick_DrawMagic(_float fTimeDelta)
{
	__super::Tick_DrawMagic(fTimeDelta);
}

void CAccio::Tick_CastMagic(_float fTimeDelta)
{
	__super::Tick_CastMagic(fTimeDelta);

	if (m_fLerpAcc != 1)
	{
		m_fLerpAcc += fTimeDelta / m_fLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		m_TrailVec[EFFECT_STATE_MAIN].data()[0]->Spin_Move(m_vEndPosition, m_vStartPosition,m_vSpinWeight,m_fSpinSpeed, m_fLerpAcc);
		m_pTransform->Set_Position(XMVectorLerp(m_vStartPosition, m_vEndPosition, m_fLerpAcc));
	}
	else
	{
		Do_MagicBallState_To_Next();
	}

	for (int i = 0; i < m_ParticleVec[EFFECT_STATE_MAIN].size(); i++)
	{
		m_ParticleVec[EFFECT_STATE_MAIN].data()[i]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	}
}

void CAccio::Tick_Dying(_float fTimeDelta)
{
	m_fWingardiumEffectDeadTimer -= fTimeDelta;

	m_ParticleVec[EFFECT_STATE_HIT][1]->Get_Transform()->Set_Position(m_CurrentTargetMatrix.Translation());
	m_ParticleVec[EFFECT_STATE_HIT][2]->Get_Transform()->Set_Position(m_CurrentTargetMatrix.Translation());

	if (m_fWingardiumEffectDeadTimer < 0)
	{
		Do_MagicBallState_To_Next();
	}
}

HRESULT CAccio::Add_Components()
{
	m_TrailVec[EFFECT_STATE_WAND].resize(1);
	m_ParticleVec[EFFECT_STATE_WAND].resize(1);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect"),
		TEXT("Com_Wand_Trail"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_WAND][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_BasicCast_Wand_Trail_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Defatul_Wand_Glow_Effect"),
		TEXT("Com_WandGlow"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_WAND][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Defatul_Wand_Glow_Effect)");
		__debugbreak();
		return E_FAIL;
	}

	m_TrailVec[EFFECT_STATE_MAIN].resize(1);
	m_ParticleVec[EFFECT_STATE_MAIN].resize(4);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_MagicTrail_Winga_Effect"),
		TEXT("Com_Effect"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_MAIN][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_MagicTrail_Winga_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Defatul_WandFlare_RainBow"),
		TEXT("Com_RainBow"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Defatul_WandFlare_RainBow)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Defatul_WandFlare_Spread"),
		TEXT("Com_Spread"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][1]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Defatul_WandFlare_Spread)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Defatul_WandFlare_WandGlow"),
		TEXT("Com_WandGlow"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][2]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Defatul_WandFlare_WandGlow)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Defatul_WandFlare_WandGlowLarge"),
		TEXT("Com_WandGlowLarge"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][3]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Defatul_WandFlare_WandGlowLarge)");
		__debugbreak();
		return E_FAIL;
	}

	m_ParticleVec[EFFECT_STATE_HIT].resize(3);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Accio_HitMain"),
		TEXT("Com_HitMain"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Accio_HitMain)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Accio_Hit_Wave"),
		TEXT("Com_Hit_Wave"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][1]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Accio_Hit_Wave)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Accio_Hit_Wind"),
		TEXT("Com_Hit_Wind"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][2]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Accio_Hit_Wind)");
		__debugbreak();
		return E_FAIL;
	}

	
		

	//별도
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Wingardium_Effect"),
		TEXT("Com_WingradiumEffect"), reinterpret_cast<CComponent**>(&m_pWingardiumEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Wingardium_Effect)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

CAccio* CAccio::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CAccio* pInstance = New CAccio(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CAccio");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAccio::Clone(void* pArg)
{
	CAccio* pInstance = New CAccio(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAccio");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAccio::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pWingardiumEffect);
	}
}
