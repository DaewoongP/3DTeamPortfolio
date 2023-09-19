#include "Protego.h"
#include "GameInstance.h"
#include "Protego_Effect.h"
CProtego::CProtego(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CProtego::CProtego(const CProtego& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CProtego::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProtego::Initialize(void* pArg)
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

	m_pTransform->Set_Position(m_pTarget->Get_Position());
	return S_OK;
}

void CProtego::Tick(_float fTimeDelta)
{
	m_pTransform->Set_Position(m_pTarget->Get_Position());
	m_pProtegoEffect->Get_Transform()->Set_Position(m_pTarget->Get_Position());
	__super::Tick(fTimeDelta);
}

void CProtego::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CProtego::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	//출돌된게 있습니다.
	__super::OnCollisionEnter(CollisionEventDesc);
}

void CProtego::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CProtego::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CProtego::Add_Components()
{
	try
	{
		if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Protego_Effect")
			, TEXT("Com_Protego_Effect"), reinterpret_cast<CComponent**>(&m_pProtegoEffect), &m_MagicBallDesc)))
			throw "Com_Protego_Effect";
	}
	catch (const _char* pErrorMessage)
	{
		string strErrorMessage = "Failed to Cloned in CProtego : ";
		strErrorMessage += pErrorMessage;
		MSG_BOX(strErrorMessage.data());
	}

	return S_OK;
}

HRESULT CProtego::Add_Effect()
{
	return S_OK;
}

HRESULT CProtego::Add_RigidBody()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry SphereGeometry = PxSphereGeometry(2.3f);
	RigidBodyDesc.pGeometry = &SphereGeometry;
	RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
	RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
	RigidBodyDesc.vInitPosition = _float3(0.f, 0.f, 0.f);
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerObject = this;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Magic_Ball");

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_RigidBody)");
		return E_FAIL;
	}
	// 리지드바디 액터 설정
	PxRigidBody* Rigid = m_pRigidBody->Get_RigidBodyActor();
	m_pTransform->Set_RigidBody(m_pRigidBody);
	Rigid->setMaxLinearVelocity(1000.f);
	Rigid->setMass(10.f);
	Rigid->setAngularDamping(0.7f);

	return S_OK;
}

CProtego* CProtego::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProtego* pInstance = New CProtego(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CProtego");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProtego::Clone(void* pArg)
{
	CProtego* pInstance = New CProtego(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CProtego");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CProtego::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pProtegoEffect);
	}
}
