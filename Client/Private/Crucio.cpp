#include "Crucio.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"

CCrucio::CCrucio(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CCrucio::CCrucio(const CCrucio& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CCrucio::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	BEGININSTANCE;
	//메인 이펙트
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Sprak_03")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Sprak_03")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Crucio/Main_Wand_Sprak_03"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Spark_02")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Spark_02")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Crucio/Main_Wand_Spark_02"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Spark_01")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Spark_01")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Crucio/Main_Wand_Spark_01"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Spark")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Spark")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Crucio/Main_Wand_Spark"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	//메인 트레일 추가 필요 << 파티클로 해볼라 했는데 구조상 파티클로 안됨.
	//차라리 나중에 매쉬보고 해야할듯

	//종료 이펙트 << 피격이펙트랑 복붙이나, 선딜레이만 없애주면 될듯함.

	//완드
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/BasicTrail/BasicTrail.trail"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Trail_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Trail_Effect")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/Crucio/Crucio.trail"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Dark_Distance")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Dark_Distance")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Crucio/Wand_Dark_Distance"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Dust_Distance")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Dust_Distance")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Crucio/Wand_Dust_Distance"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Red_Distance")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Red_Distance")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Crucio/Wand_Red_Distance"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Spark_Dark")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Spark_Dark")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Crucio/Wand_Spark_Dark"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Spark_Sub")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Spark_Sub")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Crucio/Wand_Spark_Sub"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	ENDINSTANCE;
	return S_OK;
}

HRESULT CCrucio::Initialize(void* pArg)
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

void CCrucio::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCrucio::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CCrucio::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
	//몹이랑 충돌했으면?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Enemy_Body")) != nullptr)
	{
		Set_MagicBallState(MAGICBALL_STATE_DYING);
	}
}

void CCrucio::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CCrucio::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CCrucio::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	return S_OK;
}

void CCrucio::Ready_Begin()
{
	__super::Ready_Begin();
}

void CCrucio::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
}

void CCrucio::Ready_CastMagic()
{
	Ready_SplineSpinMove(m_TrailVec[EFFECT_STATE_MAIN].data()[0],_float2(0.2f, 0.20f),0.5f);
	__super::Ready_CastMagic();
}

void CCrucio::Ready_Dying()
{
	__super::Ready_Dying();
}

void CCrucio::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CCrucio::Tick_DrawMagic(_float fTimeDelta)
{
	__super::Tick_DrawMagic(fTimeDelta);
}

void CCrucio::Tick_CastMagic(_float fTimeDelta)
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

void CCrucio::Tick_Dying(_float fTimeDelta)
{
	m_ParticleVec[EFFECT_STATE_MAIN].data()[0]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	m_ParticleVec[EFFECT_STATE_MAIN].data()[1]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	m_ParticleVec[EFFECT_STATE_MAIN].data()[2]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());

	__super::Tick_Dying(fTimeDelta);
}


HRESULT CCrucio::Add_Components()
{
	m_TrailVec[EFFECT_STATE_WAND].resize(2);
	m_ParticleVec[EFFECT_STATE_WAND].resize(5);
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")
		, TEXT("Com_BasicCast_Wand_Trail"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_WAND][0]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Trail_Effect")
		, TEXT("Com_Crucio_Wand_Trail"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_WAND][1]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Dark_Distance")
		, TEXT("Com_Hit_Particle01"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_WAND][0]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Dust_Distance")
		, TEXT("Com_Wand_Dust_Distance"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_WAND][1]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Red_Distance")
		, TEXT("Com_Wand_Red_Distance"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_WAND][2]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Spark_Dark")
		, TEXT("Com_Wand_Spark_Dark"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_WAND][3]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Wand_Spark_Sub")
		, TEXT("Com_Wand_Spark_Sub"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_WAND][4]))))
	{
		__debugbreak();
		return E_FAIL;
	}

	//메인 트레일 제작중
	m_ParticleVec[EFFECT_STATE_MAIN].resize(4);
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Sprak_03")
		, TEXT("Com_Wand_Sprak_03"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][0]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Spark_02")
		, TEXT("Com_Wand_Spark_02"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][1]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Spark_01")
		, TEXT("Com_Wand_Spark_01"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][2]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Spark")
		, TEXT("Com_Wand_Spark"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][3]))))
	{
		__debugbreak();
		return E_FAIL;
	}

	//히트 제작중

	return S_OK;
}

CCrucio* CCrucio::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CCrucio* pInstance = new CCrucio(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CCrucio");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCrucio::Clone(void* pArg)
{
	CCrucio* pInstance = new CCrucio(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCrucio");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCrucio::Free()
{
	__super::Free();
}

