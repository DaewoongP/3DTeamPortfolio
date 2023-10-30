#include "Pulse.h"

#include "GameInstance.h"

CPulse::CPulse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPulse::CPulse(const CPulse& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPulse::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPulse::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Failed Initialize CPulse : pArg is nullptr");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	{
		PULSEINITDESC* pPulseInitDesc = static_cast<PULSEINITDESC*>(pArg);
		m_pTarget = pPulseInitDesc->pTarget;
		if (nullptr == m_pTarget)
		{
			MSG_BOX("Failed Initialize CPulse : m_pTarget is nullptr");
			return E_FAIL;
		}
		m_fMoveSpeed = pPulseInitDesc->fSpeed;
		m_fLifeTime = pPulseInitDesc->fLifeTime;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);

	m_CollisionRequestDesc.eType = CEnemy::ATTACK_LIGHT;
	m_CollisionRequestDesc.iDamage = 50;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;

	return S_OK;
}

void CPulse::Tick(_float fTimeDelta)
{
	if (false == m_isEnable)
		return;

	__super::Tick(fTimeDelta);

	m_fTimeAcc += fTimeDelta;
	if (m_fLifeTime <= m_fTimeAcc)
	{
		m_isEnable = false;
		m_pRigidBody->Disable_Collision("Attack_Pulse");
		return;
	}

	_float3 vPosition = m_pTransform->Get_Position();
	_float3 vTargetPosition = m_pTarget->Get_Transform()->Get_Position();
	_float3 vDirection = vTargetPosition - vPosition;
	vDirection.Normalize();

	vPosition += vDirection * m_fMoveSpeed;
	m_pTransform->Set_Position(vPosition);
}

void CPulse::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	if (nullptr != m_pRenderer)
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
}

HRESULT CPulse::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CPulse::Reset(const PULSEINITDESC& ResetDesc)
{
	m_pTarget = ResetDesc.pTarget;
	if (nullptr == m_pTarget)
	{
		MSG_BOX("Failed Reset CPulse : m_pTarget is nullptr");
		return;
	}
	m_pTransform->Set_Position(ResetDesc.vPosition);
	m_fMoveSpeed = ResetDesc.fSpeed;
	m_fLifeTime = ResetDesc.fLifeTime;
	m_fTimeAcc = 0.f;
	m_isEnable = true;

	m_pRigidBody->Enable_Collision("Attack_Pulse", this, &m_CollisionRequestDesc);
}

HRESULT CPulse::Add_Components()
{
	/* Com_RigidBody */
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
	RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 1.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry pSphereGeometry = PxSphereGeometry(0.2f);
	RigidBodyDesc.pGeometry = &pSphereGeometry;
	RigidBodyDesc.eConstraintFlag = CRigidBody::None;
	RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.eThisCollsion = COL_MAGIC;
	RigidBodyDesc.eCollisionFlag = COL_PLAYER;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Attack_Pulse");
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CPulse Add_Component : (Com_RigidBody)");
		__debugbreak();
		return E_FAIL;
	}

#ifdef _DEBUG

	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CBreath Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

#endif // _DEBUG

	return S_OK;
}

CPulse* CPulse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPulse* pInstance = New CPulse(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPulse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPulse::Clone(void* pArg)
{
	CPulse* pInstance = New CPulse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPulse");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPulse::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pRigidBody);
#ifdef _DEBUG
		Safe_Release(m_pRenderer);
#endif // _DEBUG
	}
}
