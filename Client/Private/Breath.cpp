#include "Breath.h"

#include "GameInstance.h"

#include "Cylinder.h"

CBreath::CBreath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBreath::CBreath(const CBreath& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBreath::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBreath::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);

	return S_OK;
}

void CBreath::Tick(_float fTimeDelta)
{
	if (false == m_isActionBreath)
		return;

	__super::Tick(fTimeDelta);

	if (nullptr != m_pCylinder)
		m_pCylinder->Tick(fTimeDelta);
}

void CBreath::Late_Tick(_float fTimeDelta)
{
	if (false == m_isActionBreath)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pCylinder)
		m_pCylinder->Late_Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}

	Safe_Release(pGameInstance);
}

void CBreath::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
}

void CBreath::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
}

HRESULT CBreath::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CBreath::Reset(const BREATHINITDESC& tagResetDesc)
{
	m_pTransform->Set_Position(tagResetDesc.vPosition);
	m_pTransform->Rotation(tagResetDesc.vRight, tagResetDesc.fRadian);

	CCylinder::CYLINDERINITDESC CylinderDesc;
	CylinderDesc.fRadian = tagResetDesc.fRadian;
	CylinderDesc.vPosition = tagResetDesc.vPosition;
	CylinderDesc.vRight = tagResetDesc.vRight;
	CylinderDesc.vSacle = _float3(0.2f, tagResetDesc.fDistance, 0.2f);
	m_pCylinder->Reset(CylinderDesc);
}

HRESULT CBreath::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CBreath Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CBreath Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* For.Com_RigidBody */
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
	RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 1.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxCapsuleGeometry pCapsuleGeometry = PxCapsuleGeometry(0.5f, 20.f);
	RigidBodyDesc.pGeometry = &pCapsuleGeometry;
	RigidBodyDesc.eConstraintFlag = CRigidBody::None;
	RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.eThisCollsion = COL_MAGIC;
	RigidBodyDesc.eCollisionFlag = COL_PLAYER;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Breath");
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CBreath Add_Component : (Com_RigidBody)");
		__debugbreak();
		return E_FAIL;
	}

	/* Object Cylinder */
	BEGININSTANCE;
	m_pCylinder = dynamic_cast<CCylinder*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Cylinder")));
	ENDINSTANCE;
	if(nullptr == m_pCylinder)
	{
		MSG_BOX("Failed CBreath Add_Component : m_pCylinder is nullptr");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBreath::SetUp_ShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

CBreath* CBreath::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBreath* pInstance = New CBreath(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBreath");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBreath::Clone(void* pArg)
{
	CBreath* pInstance = New CBreath(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBreath");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBreath::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pCylinder);
		Safe_Release(m_pRigidBody);
		Safe_Release(m_pShader);
		Safe_Release(m_pRenderer);
	}
}
