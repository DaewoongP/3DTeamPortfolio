#include "Stupefy.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"

CStupefy::CStupefy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CStupefy::CStupefy(const CStupefy& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CStupefy::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	BEGININSTANCE;
	//¸ÞÀÎ ÀÌÆåÆ®
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_Trail")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_Trail")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/Confringo/Confringo.trail"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Stupefy_Main_Glow")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Stupefy_Main_Glow")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Stupefy/Main_Glow"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	//Á¾·á ÀÌÆåÆ®
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Stupefy_Hit_Dust")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Stupefy_Hit_Dust")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Stupefy/Hit_Dust"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Stupefy_Hit_Effect01")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Stupefy_Hit_Effect01")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Stupefy/Hit_Effect01"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Stupefy_Hit_Glow")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Stupefy_Hit_Glow")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Stupefy/Hit_Glow"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Stupefy_Hit_Water")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Stupefy_Hit_Water")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Stupefy/Hit_Water"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	ENDINSTANCE;
	return S_OK;
}

HRESULT CStupefy::Initialize(void* pArg)
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

	Ready_Shake(60.0f, 2.0f, 0.04f);

	return S_OK;
}

void CStupefy::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CStupefy::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CStupefy::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
	//¸÷ÀÌ¶û Ãæµ¹ÇßÀ¸¸é?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Enemy_Body")) != nullptr)
	{
		Set_MagicBallState(MAGICBALL_STATE_DYING);

#pragma region Ä«¸Þ¶ó ½¦ÀÌÅ©
		BEGININSTANCE;

		//pGameInstance->Set_SlowTime(TEXT("MainTimer"), 0.2f, 0.05f);

		_float3 vRandomAxis = *pGameInstance->Get_CamRight();

		_float3 vLook = *pGameInstance->Get_CamLook();

		_float fRandomRadian = _float(rand() % 30);

		fRandomRadian *= _float((rand() % 2) - 1);

		vRandomAxis = XMVector3TransformNormal(vRandomAxis, XMMatrixRotationAxis(vLook, XMConvertToRadians(fRandomRadian)));

		pGameInstance->Set_Shake(
			CCamera_Manager::SHAKE_TYPE_TRANSLATION,
			CCamera_Manager::SHAKE_AXIS_SET,
			CEase::OUT_QUAD,
			10.0f,
			1.0f,
			Shake_Power(CollisionEventDesc.pOtherTransform->Get_Position()),
			CCamera_Manager::SHAKE_POWER_DECRECENDO,
			vRandomAxis);

		ENDINSTANCE;
#pragma endregion
	}
}

void CStupefy::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CStupefy::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CStupefy::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	return S_OK;
}

void CStupefy::Ready_Begin()
{
	__super::Ready_Begin();
}

void CStupefy::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
}

void CStupefy::Ready_CastMagic()
{
	Ready_SplineSpinMove(m_TrailVec[EFFECT_STATE_MAIN].data()[0], _float2(0.2f, 0.20f), 0.5f);
	__super::Ready_CastMagic();
}

void CStupefy::Ready_Dying()
{
	__super::Ready_Dying();
}

void CStupefy::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CStupefy::Tick_DrawMagic(_float fTimeDelta)
{
	__super::Tick_DrawMagic(fTimeDelta);
}

void CStupefy::Tick_CastMagic(_float fTimeDelta)
{
	__super::Tick_CastMagic(fTimeDelta);
	if (m_fLerpAcc != 1)
	{
		m_fLerpAcc += fTimeDelta / m_fLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		m_TrailVec[EFFECT_STATE_MAIN].data()[0]->Spline_Spin_Move(m_vSplineLerp[0], m_vStartPosition, m_vEndPosition, m_vSplineLerp[1], m_vSpinWeight, m_fSpinSpeed, m_fLerpAcc);
		m_pTransform->Set_Position(XMVectorLerp(m_vStartPosition, m_vEndPosition, m_fLerpAcc));
	}
	else
	{
		Do_MagicBallState_To_Next();
	}
	m_ParticleVec[EFFECT_STATE_MAIN].data()[0]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
}

void CStupefy::Tick_Dying(_float fTimeDelta)
{
	__super::Tick_Dying(fTimeDelta);
}


HRESULT CStupefy::Add_Components()
{
	m_TrailVec[EFFECT_STATE_MAIN].resize(1);
	m_ParticleVec[EFFECT_STATE_MAIN].resize(1);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Confringo_Trail"),
		TEXT("Com_Trail"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_MAIN][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Confringo_Trail)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Stupefy_Main_Glow")
		, TEXT("Com_Main_Glow"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][0]))))
	{
		__debugbreak();
		return E_FAIL;
	}

	m_ParticleVec[EFFECT_STATE_HIT].resize(4);
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Stupefy_Hit_Dust")
		, TEXT("Com_Hit_Dust"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][0]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Stupefy_Hit_Effect01")
		, TEXT("Com_Hit_Effect01"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][1]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Stupefy_Hit_Glow")
		, TEXT("Com_Hit_Glow"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][2]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Stupefy_Hit_Water")
		, TEXT("Com_Hit_Water"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][3]))))
	{
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

CStupefy* CStupefy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CStupefy* pInstance = new CStupefy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CStupefy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStupefy::Clone(void* pArg)
{
	CStupefy* pInstance = new CStupefy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CStupefy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStupefy::Free()
{
	__super::Free();
}

