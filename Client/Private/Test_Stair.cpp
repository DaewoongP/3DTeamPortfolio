#include "..\Public\Test_Stair.h"
#include "GameInstance.h"
#include "PhysXConverter.h"
#include "ConvexMesh.h"
#include "Model.h"
#include "TriangleMesh.h"

CTest_Stair::CTest_Stair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTest_Stair::CTest_Stair(const CTest_Stair& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTest_Stair::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTest_Stair::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_pTransform->Set_Position(_float3(0,0,3));
	}

	m_pTransform->Set_RigidBody(m_pRigidBody);

	return S_OK;
}

void CTest_Stair::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CTest_Stair::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
		m_pRenderer->Add_DebugGroup(m_pConvexMesh);
#endif // _DEBUG
	}
}

HRESULT CTest_Stair::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (nullptr == m_pShaderCom||
		nullptr == m_pModelCom)
		return S_OK;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTest_Stair::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CTest_Stair Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CTest_Stair Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.vInitPosition = _float3(15.f, 5.f, 5.f);
	RigidBodyDesc.fStaticFriction = 0.5f;
	RigidBodyDesc.fDynamicFriction = 0.5f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry GeoMetry = PxSphereGeometry(2.f);
	RigidBodyDesc.pGeometry = &GeoMetry;
	
	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_RigidBody)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTest_Stair::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	Safe_Release(pGameInstance);

	return S_OK;
}

CTest_Stair* CTest_Stair::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTest_Stair* pInstance = New CTest_Stair(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTest_Stair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTest_Stair::Clone(void* pArg)
{
	CTest_Stair* pInstance = New CTest_Stair(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTest_Stair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTest_Stair::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pModelCom);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRenderer);
		Safe_Release(m_pConvexMesh);
		Safe_Release(m_pTriangleMesh);
		Safe_Release(m_pRigidBody);
	}
}
