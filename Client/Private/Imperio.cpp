#include "Imperio.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"

CImperio::CImperio(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CImperio::CImperio(const CImperio& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CImperio::Initialize_Prototype(_uint iLevel)
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
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Imperio_Wand_Cast_Glow")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Imperio_Wand_Cast_Glow")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Imperio/Wand_Cast_Glow"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Imperio_Wand_Cast_Rainbow")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Imperio_Wand_Cast_Rainbow")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Imperio/Wand_Cast_Rainbow"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Imperio_Wand_Cast_Spread")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Imperio_Wand_Cast_Spread")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Imperio/Wand_Cast_Spread"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	//Á¾·á ÀÌÆåÆ®
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Imperio_Target_Hit_01")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Imperio_Target_Hit_01")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Imperio/Target_Hit_01"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Imperio_Target_Hit_02")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Imperio_Target_Hit_02")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Imperio/Target_Hit_02"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	//¿Ïµå
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Imperio_Wand_Trail")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Imperio_Wand_Trail")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/Imperio/Imperio.trail"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	ENDINSTANCE;
	return S_OK;
}

HRESULT CImperio::Initialize(void* pArg)
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

void CImperio::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CImperio::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CImperio::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
	//¸÷ÀÌ¶û Ãæµ¹ÇßÀ¸¸é?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Enemy_Body")) != nullptr)
	{
		Set_MagicBallState(MAGICBALL_STATE_DYING);
	}
}

void CImperio::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CImperio::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CImperio::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	return S_OK;
}

void CImperio::Ready_Begin()
{
	__super::Ready_Begin();
}

void CImperio::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
}

void CImperio::Ready_CastMagic()
{
	Ready_SplineSpinMove(m_TrailVec[EFFECT_STATE_MAIN].data()[0],_float2(0.2f, 0.20f),0.5f);
	__super::Ready_CastMagic();
}

void CImperio::Ready_Dying()
{
	__super::Ready_Dying();
}

void CImperio::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CImperio::Tick_DrawMagic(_float fTimeDelta)
{
	__super::Tick_DrawMagic(fTimeDelta);
}

void CImperio::Tick_CastMagic(_float fTimeDelta)
{
	__super::Tick_CastMagic(fTimeDelta);
	if (m_fLerpAcc != 1)
	{
		m_fLerpAcc += fTimeDelta / m_fLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		m_TrailVec[EFFECT_STATE_MAIN].data()[0]->Spline_Spin_Move(m_vSplineLerp[0], m_vStartPosition, m_vEndPosition, m_vSplineLerp[1], m_vSpinWeight, m_fSpinSpeed, m_fLerpAcc);
		m_pTransform->Set_Position(m_TrailVec[EFFECT_STATE_MAIN].data()[0]->Get_Transform()->Get_Position());
	}
	else 
	{
		Do_MagicBallState_To_Next();
	}

	m_ParticleVec[EFFECT_STATE_MAIN].data()[0]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	m_ParticleVec[EFFECT_STATE_MAIN].data()[1]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	m_ParticleVec[EFFECT_STATE_MAIN].data()[2]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
}

void CImperio::Tick_Dying(_float fTimeDelta)
{
	m_ParticleVec[EFFECT_STATE_MAIN].data()[0]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	m_ParticleVec[EFFECT_STATE_MAIN].data()[1]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	m_ParticleVec[EFFECT_STATE_MAIN].data()[2]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());

	__super::Tick_Dying(fTimeDelta);
}


HRESULT CImperio::Add_Components()
{
	m_TrailVec[EFFECT_STATE_WAND].resize(1);
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Imperio_Wand_Trail")
		, TEXT("Com_Wand_Trail_Effect"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_WAND][0]))))
	{
		__debugbreak();
		return E_FAIL;
	}

	m_TrailVec[EFFECT_STATE_MAIN].resize(1);
	m_ParticleVec[EFFECT_STATE_MAIN].resize(3);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Expelliarmus_Trail"),
		TEXT("Com_Trail"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_MAIN][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Confringo_Trail)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Imperio_Wand_Cast_Glow")
		, TEXT("Com_Main_Glow"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][0]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Imperio_Wand_Cast_Rainbow")
		, TEXT("Com_Main_Rainbow"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][1]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Imperio_Wand_Cast_Spread")
		, TEXT("Com_Main_Spread"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][2]))))
	{
		__debugbreak();
		return E_FAIL;
	}

	m_ParticleVec[EFFECT_STATE_HIT].resize(2);
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Imperio_Target_Hit_01")
		, TEXT("Com_Hit_Particle01"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][0]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Imperio_Target_Hit_02")
		, TEXT("Com_Hit_Particle02"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][1]))))
	{
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

CImperio* CImperio::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CImperio* pInstance = new CImperio(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CImperio");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CImperio::Clone(void* pArg)
{
	CImperio* pInstance = new CImperio(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CImperio");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImperio::Free()
{
	__super::Free();
}

