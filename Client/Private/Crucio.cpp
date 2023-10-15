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

	//몹한테 나오는 메인 이펙트
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Hit_Dark")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Hit_Dark")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Crucio/Hit_Dark"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Hit_Distortion")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Hit_Distortion")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Crucio/Hit_Distortion"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	//메인 매쉬이펙트 
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Lightning03")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_Lightning03")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Crucio/Lightning03/Lightning03.ME"), m_iLevel))))
		{
			ENDINSTANCE;
			__debugbreak();
			return E_FAIL;
		}
	}

	//종료 이펙트
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_End_Dust")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_End_Dust")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Crucio/End_Dust"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_End_Spark")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_End_Spark")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Crucio/End_Spark"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_End_Spread")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Crucio_End_Spread")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Crucio/End_Spread"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

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
	m_fLightningTimer -= fTimeDelta;
	if (m_fLightningTimer < 0)
	{
		//완드 위치에서 생성.
		m_pLightningMeshEffect->Play(m_CurrentWeaponMatrix.Translation());
		//스케일은 distance /3;
		_float fDistance = Vector3::Distance(m_CurrentWeaponMatrix.Translation(), m_CurrentTargetMatrix.Translation());
		m_pLightningMeshEffect->Get_Transform()->Set_Scale(_float3(3.f, 3.f, fDistance / 3.f));
		//로테이션은 z축만 랜덤으로 생성.
		m_pLightningMeshEffect->Get_Transform()->LookAt(m_CurrentTargetMatrix.Translation());
		_float3 Axis = m_CurrentTargetMatrix.Translation() - m_CurrentWeaponMatrix.Translation();
		m_pLightningMeshEffect->Get_Transform()->Turn(Axis,XMConvertToRadians(rand()%360));
		m_fLightningTimer = 0.1f;
	}

	_bool isAlive = { false };
	for (int i = 0; i < m_ParticleVec[EFFECT_STATE_MAIN].size(); i++)
	{
		if (m_ParticleVec[EFFECT_STATE_MAIN].data()[i]->IsEnable())
		{
			isAlive = true;
			break;
		}
	}

	if (!isAlive && m_fWandParticleDelayTimer > 500)
		Do_MagicBallState_To_Next();
	
	for (_int i = 0; i < m_ParticleVec[EFFECT_STATE_WAND].size(); ++i)
	{
		m_ParticleVec[EFFECT_STATE_WAND].data()[i]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	}
	for (_int i = 0; i < 5; ++i)
	{
		m_ParticleVec[EFFECT_STATE_MAIN].data()[i]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	}
	for (_int i = 4; i < 11; ++i)
	{
		m_ParticleVec[EFFECT_STATE_MAIN].data()[i]->Get_Transform()->Set_Position(m_CurrentTargetMatrix.Translation());
	}
	m_pTransform->Set_Position(m_CurrentTargetMatrix.Translation());
}

void CCrucio::Tick_Dying(_float fTimeDelta)
{
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

	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Lightning03")
		, TEXT("Com_Lightning01"), (CComponent**)&m_pLightningMeshEffect), E_FAIL);
	
	//메인 트레일 제작중
	m_ParticleVec[EFFECT_STATE_MAIN].resize(11);
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
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Hit_Distortion")
		, TEXT("Com_Wand_Distortion"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][4]))))
	{
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Sprak_03")
		, TEXT("Com_Target_Sprak_03"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][5]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Spark_02")
		, TEXT("Com_Target_Spark_02"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][6]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Spark_01")
		, TEXT("Com_Target_Spark_01"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][7]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Main_Wand_Spark")
		, TEXT("Com_Target_Spark"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][8]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Hit_Dark")
		, TEXT("Com_Target_Dark_Hit"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][9]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_Hit_Distortion")
		, TEXT("Com_Target_Distortion_Hit"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][10]))))
	{
		__debugbreak();
		return E_FAIL;
	}

	

	//히트 제작중
	m_ParticleVec[EFFECT_STATE_HIT].resize(3);
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_End_Dust")
		, TEXT("Com_Target_End_Dust"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][0]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_End_Spark")
		, TEXT("Com_Target_End_Spark"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][1]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Crucio_End_Spread")
		, TEXT("Com_Target_End_Spread"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][2]))))
	{
		__debugbreak();
		return E_FAIL;
	}
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
	Safe_Release(m_pLightningMeshEffect);
	__super::Free();
}

