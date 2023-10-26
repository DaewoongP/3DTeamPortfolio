#include "..\Public\BeastBalloon_D.h"
#include "GameInstance.h"

CBeastBalloon_D::CBeastBalloon_D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBalloon(pDevice, pContext)
{
}

CBeastBalloon_D::CBeastBalloon_D(const CBeastBalloon_D& rhs)
	: CBalloon(rhs)
{
}

HRESULT CBeastBalloon_D::Initialize(void* pArg)
{
	NULL_CHECK_RETURN(pArg, E_FAIL);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);

	m_eBallonActionType = CBalloon::TYPE_UP;

	return S_OK;
}

HRESULT CBeastBalloon_D::Initialize_Level(_uint iCurrentLevelIndex)
{
	if (FAILED(Add_Components_Level(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CBeastBalloon_D::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBeastBalloon_D::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBeastBalloon_D::Render()
{
	if (FAILED(m_pEmissiveTexture->Bind_ShaderResource(m_pShader, "g_EmissiveTexture")))
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBeastBalloon_D::Add_Components()
{
	/* For.EmissiveTexture */
	m_pEmissiveTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/NonAnims/BeastBalloon_D/T_HM_Beast_Balloons_D.dds"));
	if (nullptr == m_pEmissiveTexture)
	{
		MSG_BOX("[CBeastBalloon_D] Failed Clone Component : EmissiveTexture");
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
	RigidBodyDesc.eThisCollsion = COL_BALLOON;
	RigidBodyDesc.eCollisionFlag = COL_RACER;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Body");
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("[CBeastBalloon_D] Failed Add_Components : Com_RigidBody");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBeastBalloon_D::Add_Components_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Model_BeastBalloon_D"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("[CBeastBalloon_D] Failed Add_Components_Level : Com_Model");
		return E_FAIL;
	}

	return S_OK;
}

CBeastBalloon_D* CBeastBalloon_D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBeastBalloon_D* pInstance = New CBeastBalloon_D(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBeastBalloon_D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBeastBalloon_D::Clone(void* pArg)
{
	CBeastBalloon_D* pInstance = New CBeastBalloon_D(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBeastBalloon_D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastBalloon_D::Free()
{
	__super::Free();

	Safe_Release(m_pEmissiveTexture);
}