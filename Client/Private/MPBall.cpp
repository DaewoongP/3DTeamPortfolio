#include "MPBall.h"
#include"GameInstance.h"
#include"ParticleSystem.h"	

CMPBall::CMPBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :CGameObject(pDevice,pContext)
{
}

CMPBall::CMPBall(const CMPBall& rhs) :CGameObject(rhs)
{
}

HRESULT CMPBall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	m_iLevel ;


	BEGININSTANCE;
	




	ENDINSTANCE;

	return S_OK;
}

HRESULT CMPBall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("Failed MPBall Initialize");

		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed MPBall Add_Components");

		return E_FAIL;
	}

	return S_OK;
}

void CMPBall::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CMPBall::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CMPBall::Add_Components()
{
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CMagicBall Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(Add_RigidBody()))
	{
		MSG_BOX("Failed CMagicBall SettingRigidBody : (Com_RigidBody)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMPBall::Add_RigidBody()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(5.f, 0.0f, 5.f);
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry SphereGeometry = PxSphereGeometry(0.2f);
	RigidBodyDesc.pGeometry = &SphereGeometry;
	RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
	RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.eThisCollsion = COL_ITEM;
	RigidBodyDesc.eCollisionFlag = m_eCollisionFlag;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "MPBall");

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_RigidBody)");
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

CMPBall* CMPBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMPBall* pInstance = New CMPBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMPBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMPBall::Clone(void* pArg)
{
	CMPBall* pInstance = New CMPBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMPBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMPBall::Free()
{
	__super::Free();
}
