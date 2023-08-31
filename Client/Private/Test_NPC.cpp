#include "Test_NPC.h"

#include "GameInstance.h"

CTest_NPC::CTest_NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTest_NPC::CTest_NPC(const CTest_NPC& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTest_NPC::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTest_NPC::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	m_pTransform->Set_Position(_float3(10.f, 0.f, 0.f));
	m_pTransform->Set_Scale(_float3(100.f, 100.f, 100.f));

	return S_OK;
}

void CTest_NPC::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CTest_NPC::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTest_NPC::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE);

		m_pShaderCom->Begin("AnimMesh");

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CTest_NPC::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
}

void CTest_NPC::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
}

void CTest_NPC::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
}

HRESULT CTest_NPC::Make_AI()
{
	return S_OK;
}

HRESULT CTest_NPC::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Fiona"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_Model)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTest_NPC::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CTest_NPC* CTest_NPC::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTest_NPC* pInstance = new CTest_NPC(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTest_NPC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CTest_NPC* CTest_NPC::Clone(void* pArg)
{
	CTest_NPC* pInstance = new CTest_NPC(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTest_NPC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTest_NPC::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pStatusCom);
	Safe_Release(m_pBehaviorTree);
}
