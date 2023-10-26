#include "Racer.h"
#include "GameInstance.h"

CRacer::CRacer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CRacer::CRacer(const CRacer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CRacer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRacer::Initialize(void* pArg)
{
	m_eRacerType  = static_cast<RACERINITDESC*>(pArg)->eRacerType;
	m_iRacerNumber = static_cast<RACERINITDESC*>(pArg)->iRacerNumber;
	if (nullptr == pArg)
	{
		MSG_BOX("CRacer Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_CollisionRequestDesc.eRacerType = m_eRacerType;
	m_CollisionRequestDesc.iRacerNumber = m_iRacerNumber;
	m_pRigidBody->Enable_Collision("Racer", this, &m_CollisionRequestDesc);
	return S_OK;
}

HRESULT CRacer::Initialize_Level(_uint iCurrentLevelIndex)
{
	return S_OK;
}

void CRacer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CRacer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (nullptr != m_pRenderer)
	{
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

void CRacer::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
}

HRESULT CRacer::Add_Components()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
	RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f);
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 1.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry pSphereGeomatry1 = PxSphereGeometry(1.5f);
	RigidBodyDesc.pGeometry = &pSphereGeomatry1;
	RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
	RigidBodyDesc.vDebugColor = _float4(0.f, 0.f, 1.f, 1.f);
	RigidBodyDesc.pOwnerObject = this;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Racer");
	RigidBodyDesc.eThisCollsion = COL_RACER;
	RigidBodyDesc.eCollisionFlag = COL_BALLOON;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("CLoadBalloon Failed Clone Component : Com_RigidBody");
		return E_FAIL;
	}

	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("CLoadBalloon Failed Clone Component : Com_Renderer");
		return E_FAIL;
	}

	return S_OK;
}


CRacer* CRacer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRacer* pInstance = New CRacer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRacer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRacer::Clone(void* pArg)
{
	CRacer* pInstance = New CRacer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CRacer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRacer::Free()
{
	__super::Free();
	if (m_isCloned)
	{
		Safe_Release(m_pRigidBody);
		Safe_Release(m_pRenderer);
	}
}