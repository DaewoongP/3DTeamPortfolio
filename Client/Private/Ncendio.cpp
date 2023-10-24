#include "Ncendio.h"
#include "GameInstance.h"
#include "Engine_Function.h"
#include"ParticleSystem.h"

CNcendio::CNcendio(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CNcendio::CNcendio(const CNcendio& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CNcendio::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;
	
	BEGININSTANCE;
	//피격이펙트
	//if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_BurnTarget_Particle")))
	//{
	//	if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_BurnTarget_Particle")
	//		, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Incendio/BurnTarget/"), m_iLevel))))
	//	{
	//		ENDINSTANCE;
	//		__debugbreak();
	//		return E_FAIL;
	//	}
	//}
	//공중마법진
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_SkyCircle_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_SkyCircle_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Meteor/SkyCircle"), m_iLevel))))
		{
			ENDINSTANCE;
			__debugbreak();
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Ground_Flip")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Ground_Flip")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Meteor/Ground_Flip"), m_iLevel))))
		{
			ENDINSTANCE;
			__debugbreak();
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Meteor")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Meteor")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Meteor/Meteor.ME"), m_iLevel))))
		{
			ENDINSTANCE;
			__debugbreak();
			return E_FAIL;
		}
	}

	ENDINSTANCE;
	return S_OK;
}

HRESULT CNcendio::Initialize(void* pArg)
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

	m_pTransform->Set_Speed(35.f);

	m_ParticleVec[EFFECT_STATE_MAIN][0]->Get_Transform()->Set_Position(_float3(m_pTransform->Get_Position().x, m_pTransform->Get_Position().y+30.f, m_pTransform->Get_Position().z));

	Ready_Shake(30.0f, 2.0f, 0.04f);

	return S_OK;
}

void CNcendio::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CNcendio::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CNcendio::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
	//몹이랑 충돌했으면?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Enemy_Body")) != nullptr)
	{
#pragma region 카메라 쉐이크
		BEGININSTANCE;

		//pGameInstance->Set_SlowTime(TEXT("MainTimer"), 0.2f, 0.05f);

		_float3 vRandomAxis = *pGameInstance->Get_CamRight();

		_float3 vLook = *pGameInstance->Get_CamLook();

		_float fRandomRadian = _float(rand() % 30);

		fRandomRadian *= _float((rand() % 2) - 1);

		vRandomAxis = XMVector3TransformNormal(vRandomAxis, XMMatrixRotationAxis(vLook, XMConvertToRadians(fRandomRadian)));

		pGameInstance->Set_Shake(
			CCamera_Manager::SHAKE_PRIORITY_2,
			CCamera_Manager::SHAKE_TYPE_TRANSLATION,
			CCamera_Manager::SHAKE_AXIS_SET,
			CEase::OUT_QUAD,
			10.0f,
			3.0f,
			Shake_Power(CollisionEventDesc.pOtherTransform->Get_Position()),
			CCamera_Manager::SHAKE_POWER_DECRECENDO,
			vRandomAxis);

		ENDINSTANCE;
#pragma endregion
		Set_MagicBallState(MAGICBALL_STATE_DYING);
		for (_uint i = 0; i < m_ParticleVec->size(); ++i)
		{
			m_ParticleVec[EFFECT_STATE_HIT][i]->Enable();
			m_ParticleVec[EFFECT_STATE_HIT][i]->Play();
		}
	}

}

void CNcendio::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CNcendio::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CNcendio::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	return S_OK;
}

void CNcendio::Ready_Begin()
{
	// 인센디오 원형 불꽃을 타겟 포지션보다 살짝 낮춘다.
	_float3 vOffsetPosition = m_vStartPosition;
	vOffsetPosition.y -= 0.5f;

	// 맞춘 대상보다 텍스처가 더 앞으로 나오게 조정한다.
	_float3 vBurnTargetOffset = m_vStartPosition - m_vEndPosition;
	vBurnTargetOffset.Normalize();
	vBurnTargetOffset *= 0.5f;
	

	m_pMeteorEffect->Play(_float3(vOffsetPosition.x, vOffsetPosition.y+30.f, vOffsetPosition.z));
	m_pTransform->Set_Position(m_vStartPosition);
	
}

void CNcendio::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();

}

void CNcendio::Ready_CastMagic()
{	

	for (_uint i = 0; i < m_ParticleVec->size(); ++i)
	{
		m_ParticleVec[EFFECT_STATE_MAIN][i]->Enable();
		m_ParticleVec[EFFECT_STATE_MAIN][i]->Play(_float3(m_pTransform->Get_Position().x, m_pTransform->Get_Position().y + 30.f, m_pTransform->Get_Position().z));
	}

	m_vEndPosition = m_CurrentTargetMatrix.Translation();
	m_vEndPosition += m_vEndPosition * _float(rand() / 10 + 20) * 0.1f;
	m_fTimeScalePerDitance = 100 / _float3(m_vEndPosition - m_vStartPosition).Length();

	__super::Ready_CastMagic();
}

void CNcendio::Ready_Dying()
{
	BEGININSTANCE
		_float3 vAxis = _float3(0, 1, 0);
	pGameInstance->Set_Shake(
		(CCamera_Manager::SHAKE_PRIORITY)0,
		(CCamera_Manager::SHAKE_TYPE)0,
		(CCamera_Manager::SHAKE_AXIS)3,
		(CEase::EASE)0,
		10,
		0.6f,
		0.2f,
		(CCamera_Manager::SHAKE_POWER)1,
		vAxis);
	ENDINSTANCE

}

void CNcendio::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);

}

void CNcendio::Tick_DrawMagic(_float fTimeDelta)
{
	//_float4x4 DirMatrix = {};
	//m_pTransform->Set_Position(m_vStartPosition);
	//_float3 vDir = m_pTargetWorldMatrix->Translation() - m_pTransform->Get_Position();
	//vDir.Normalize();
	//DirMatrix.Look(vDir);
	//DirMatrix.m[3][0] = m_pTransform->Get_Position().x;
	//DirMatrix.m[3][1] = m_pTransform->Get_Position().y;
	//DirMatrix.m[3][2] = m_pTransform->Get_Position().z;
	//m_pMeteorEffect->Get_Transform()->Set_WorldMatrix(DirMatrix);
}

void CNcendio::Tick_CastMagic(_float fTimeDelta)
{
	if (nullptr == m_pTargetWorldMatrix)
		return;
	m_pMeteorEffect->Get_Transform()->LookAt(m_pTargetWorldMatrix->Translation());
	m_pMeteorEffect->Get_Transform()->Go_Straight(fTimeDelta);
	m_pMeteorEffect->Get_Transform()->Set_Speed(35.f);

}

void CNcendio::Tick_Dying(_float fTimeDelta)
{
}

HRESULT CNcendio::Add_Components()
{
	m_ParticleVec[EFFECT_STATE_MAIN].resize(1);
	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_SkyCircle_Particle")
		, TEXT("Com_SkyCircle"), (CComponent**)&m_ParticleVec[EFFECT_STATE_MAIN][0]), E_FAIL);
	//FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_SpreadFire_Red")
	//	, TEXT("Com_SpreadFire_Red"), (CComponent**)&m_ParticleVec[EFFECT_STATE_MAIN][1]), E_FAIL);

	m_ParticleVec[EFFECT_STATE_HIT].resize(1);
	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Ground_Flip")
		, TEXT("Com_FireCircleBoom"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][0]), E_FAIL);
	//FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_FireCircleBoom_Particle2")
	//	, TEXT("Com_FireCircleBoom"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][1]), E_FAIL);
	//FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_FireCircleBoom_Particle3")
	//	, TEXT("Com_FireCircleBoom"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][2]), E_FAIL);
	//FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_FireCircleBoom_Particle4")
	//	, TEXT("Com_FireCircleBoom"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][3]), E_FAIL);
	//FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_BurnTarget_Particle")
	//	, TEXT("Com_BurnTarget"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][4]), E_FAIL);
	//FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_SmokeCloud_Particle")
	//	, TEXT("Com_SmokeCloud"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][5]), E_FAIL);
	//FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Fire_Particle")
	//	, TEXT("Com_SmokeCloud"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][6]), E_FAIL);
	//FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Fire_Particle2")
	//	, TEXT("Com_SmokeCloud"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][7]), E_FAIL);


	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Meteor")
		, TEXT("Com_Meteor"), (CComponent**)&m_pMeteorEffect), E_FAIL);


	return S_OK;
}
HRESULT CNcendio::Add_RigidBody()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.0f, 0.f);
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry SphereGeometry = PxSphereGeometry(3.f);
	RigidBodyDesc.pGeometry = &SphereGeometry;
	RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
	RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.eThisCollsion = COL_MAGIC;
	RigidBodyDesc.eCollisionFlag = m_eCollisionFlag;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Magic_Ball");

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_RigidBody)");
		return E_FAIL;
	}

	return S_OK;
}

CNcendio* CNcendio::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CNcendio* pInstance = New CNcendio(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CNcendio");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNcendio::Clone(void* pArg)
{
	CNcendio* pInstance = New CNcendio(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CNcendio");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNcendio::Free()
{
	__super::Free();
	if(m_isCloned== true)
	{
		Safe_Release(m_pMeteorEffect);
	}
}

