#include "Meteor.h"
#include "GameInstance.h"
#include "Engine_Function.h"
#include "ParticleSystem.h"

CMeteor::CMeteor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMagicBall(pDevice,pContext)
{
}

CMeteor::CMeteor(const CMeteor& rhs) : CMagicBall(rhs)
{
}

HRESULT CMeteor::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;


	BEGININSTANCE
		if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Ground_Flip")))
		{
			if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Ground_Flip")
				, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Meteor/Ground_Flip/"), m_iLevel))))
			{
				ENDINSTANCE;
				__debugbreak();
				return E_FAIL;
			}
		}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireRing_MeshEffect")))
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

HRESULT CMeteor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("Failed Meteor Initialize");

		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Meteor Add_Components");

		return E_FAIL;
	}
	m_pTransform->Set_Speed(35.f);
	m_vLightColor = _float4(1, 0, 0, 1);
	return S_OK;
}

void CMeteor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CMeteor::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CMeteor::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
}

void CMeteor::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CMeteor::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CMeteor::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	//m_fDyingTimer = 1000.f;
	//m_ParticleVec[EFFECT_STATE_HIT][8]->Get_ShapeModuleRef().vLength.y = 0;
	//m_ParticleVec[EFFECT_STATE_HIT][8]->Get_VelocityOverLifetimeModuleRef().vLinearMin.y = 5.f;
	//m_ParticleVec[EFFECT_STATE_HIT][8]->Get_VelocityOverLifetimeModuleRef().vLinearMax.y = 5.f;

	return S_OK;
}

void CMeteor::Ready_Begin()
{
	__super::Ready_Begin();
}

void CMeteor::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
	m_pMeteorEffect->Play(m_vStartPosition);
	ADD_INCREASE_LIGHT(m_vStartPosition, 1.f, 5.f, m_vLightColor, true, 3.f);

}

void CMeteor::Ready_CastMagic()
{
	m_vEndPosition = m_CurrentTargetMatrix.Translation()* (rand()/10+20)*0.1f;
	m_pTransform->Set_Position(m_vStartPosition);
	m_fTimeScalePerDitance = 100 / _float3(m_vEndPosition - m_vStartPosition).Length();

	__super::Ready_CastMagic();


}

void CMeteor::Ready_Dying()
{

	//for (int i = 0; i < m_ParticleVec[EFFECT_STATE_HIT].size(); i++)
	//{
	//	m_ParticleVec[EFFECT_STATE_HIT].data()[i]->Enable(m_pMeshEffect->Get_Transform()->Get_Position());
	//	m_ParticleVec[EFFECT_STATE_HIT].data()[i]->Play(m_pMeshEffect->Get_Transform()->Get_Position());
	//}

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

void CMeteor::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CMeteor::Tick_DrawMagic(_float fTimeDelta)
{
	_float4x4 DirMatrix = {};
	m_pTransform->Set_Position(m_vStartPosition);
	_float3 vDir = m_pTargetWorldMatrix->Translation() - m_pTransform->Get_Position();
	vDir.Normalize();
	DirMatrix.Look(vDir);
	DirMatrix.m[3][0] = m_pTransform->Get_Position().x;
	DirMatrix.m[3][1] = m_pTransform->Get_Position().y;
	DirMatrix.m[3][2] = m_pTransform->Get_Position().z;
	m_pMeteorEffect->Get_Transform()->Set_WorldMatrix(DirMatrix);
}

void CMeteor::Tick_CastMagic(_float fTimeDelta)
{
	if (m_fLerpAcc != 1)
	{
		m_fLerpAcc += fTimeDelta / m_fLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		m_pTransform->Set_Position(XMVectorLerp(m_vStartPosition, m_vEndPosition, m_fLerpAcc));
		m_pMeteorEffect->Get_Transform()->Set_Position(m_pTransform->Get_Position());
	}
	else
	{
		Do_MagicBallState_To_Next();
	}
}


void CMeteor::Tick_Dying(_float fTimeDelta)
{

	_bool isAlive = { false };
	for (int i = 0; i < m_ParticleVec[EFFECT_STATE_HIT].size(); i++)
	{
		if (m_ParticleVec[EFFECT_STATE_HIT].data()[i]->IsEnable())
		{
			isAlive = true;
			break;
		}
	}

	if (!isAlive)
		Do_MagicBallState_To_Next();
}

HRESULT CMeteor::Add_Components()
{
	m_ParticleVec[EFFECT_STATE_HIT].resize(1);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Ground_Flip"),
		TEXT("Com_Ground_Flip"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Ground_Flip)");
		__debugbreak();
		return E_FAIL;
	}


	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Meteor")
		, TEXT("Com_Meteor"), (CComponent**)&m_pMeteorEffect), E_FAIL);

    return S_OK;
}

HRESULT CMeteor::Add_RigidBody()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.0f, 0.f);
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry SphereGeometry = PxSphereGeometry(2.f);
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

CMeteor* CMeteor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CMeteor* pInstance = New CMeteor(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX("Failed to Created CMeteor");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMeteor::Clone(void* pArg)
{
    CMeteor* pInstance = New CMeteor(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned CMeteor");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMeteor::Free()
{
    __super::Free();
    if (m_isCloned)
    {
        Safe_Release(m_pMeteorEffect);
    }
}
