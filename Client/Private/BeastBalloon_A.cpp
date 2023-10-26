#include "..\Public\BeastBalloon_A.h"

#include "GameInstance.h"

CBeastBalloon_A::CBeastBalloon_A(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBalloon(pDevice, pContext)
{
}

CBeastBalloon_A::CBeastBalloon_A(const CBeastBalloon_A& rhs)
	: CBalloon(rhs)
{
}

HRESULT CBeastBalloon_A::Initialize(void* pArg)
{
	NULL_CHECK_RETURN(pArg, E_FAIL);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);

	return S_OK;
}

HRESULT CBeastBalloon_A::Initialize_Level(_uint iCurrentLevelIndex)
{
	if (FAILED(Add_Components_Level(iCurrentLevelIndex)))
		return E_FAIL;

	if (FAILED(__super::Initialize_Level(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CBeastBalloon_A::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBeastBalloon_A::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CBeastBalloon_A::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	// 부딪 힌 대상의 정보를 가져와서 
	SCOREDESC ScoreDesc;
	ScoreDesc.Score = m_iScore;
	ScoreDesc.wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	//매니저에게 전달한 후 

	// 내가 부딪혔다는 사실을 갱신한다.
	m_isDead = true;
}

HRESULT CBeastBalloon_A::Render()
{
	if (FAILED(m_pEmissiveTexture->Bind_ShaderResource(m_pShader, "g_EmissiveTexture")))
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBeastBalloon_A::Add_Components()
{
	/* For.EmissiveTexture */
	m_pEmissiveTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/NonAnims/BeastBalloon_A/T_HM_Beast_Balloons_D.dds"));
	if (nullptr == m_pEmissiveTexture)
	{
		MSG_BOX("[CBeastBalloon_A] Failed Clone Component : EmissiveTexture");
		return E_FAIL;
	}

	/* Com_RigidBody */
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
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
	RigidBodyDesc.eThisCollsion = COL_STATIC;
	RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_ENEMY;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Body");
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("[CBeastBalloon_A] Failed Add_Components : Com_RigidBody");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBeastBalloon_A::Add_Components_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Model_BeastBalloon_A"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("[CBeastBalloon_A] Failed Add_Components_Level : Com_Model");
		return E_FAIL;
	}

	return S_OK;
}

CBeastBalloon_A* CBeastBalloon_A::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBeastBalloon_A* pInstance = New CBeastBalloon_A(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBeastBalloon_A");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBeastBalloon_A::Clone(void* pArg)
{
	CBeastBalloon_A* pInstance = New CBeastBalloon_A(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBeastBalloon_A");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastBalloon_A::Free()
{
	__super::Free();

	Safe_Release(m_pEmissiveTexture);
}