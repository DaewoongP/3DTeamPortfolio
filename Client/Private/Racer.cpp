#include "Racer.h"
#include "GameInstance.h"
#include "FlyGameManager.h"

CRacer::CRacer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CRacer::CRacer(const CRacer& rhs)
	: CGameObject(rhs)
{
}

void CRacer::Add_Score(_uint iScore)
{
	static_cast<CFlyGameManager*>(m_pOwner)->Add_Score(m_iRacerNumber,iScore);
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
	if (nullptr == pArg)
	{
		MSG_BOX("CRacer Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CRacer::Initialize_Level(_uint iCurrentLevelIndex)
{
	m_pTransform->Set_RigidBody(m_pRigidBody);
	return S_OK;
}

void CRacer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CRacer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CRacer::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	//레이서는 공이랑만 충돌해서 태그 검사가 필요없어요.
	//대신 desc를 가져와 어떤 공과 충돌했는지를 판단해주겠음.

}

HRESULT CRacer::Add_Components()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
	RigidBodyDesc.fDynamicFriction = 1.f;
	RigidBodyDesc.fRestitution = 0.f;
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
	RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
	PxSphereGeometry pSphereGeomatry = PxSphereGeometry(0.4f);
	RigidBodyDesc.pGeometry = &pSphereGeomatry;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Racer");
	RigidBodyDesc.eThisCollsion = COL_RACER;
	RigidBodyDesc.eCollisionFlag = COL_ENEMY_RANGE | COL_MAGIC | COL_STATIC;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
		throw TEXT("Com_RigidBody");
	
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
	}
}