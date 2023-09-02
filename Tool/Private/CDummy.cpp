#include "CDummy.h"
#include "GameInstance.h"

CDummy::CDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice,pContext)
{
}

CDummy::CDummy(const CDummy& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummy::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummy::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;	

	return S_OK;
}

void CDummy::Tick(_float fTimeDelta)
{
	if (nullptr != m_pModel)
	{
		m_pModel->Play_Animation(fTimeDelta,m_pTransform);
	}

	__super::Tick(fTimeDelta);
}

void CDummy::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}
}

HRESULT CDummy::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (nullptr == m_pShader ||
		nullptr == m_pModel)
		return S_OK;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;
	
	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", i);

		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", i, DIFFUSE);

		m_pShader->Begin("AnimMesh");

		m_pModel->Render(i);
	}
	return S_OK;
}

HRESULT CDummy::Add_Model_Component(const wchar_t* wszModelTag)
{
	if (m_pModel != nullptr)
	{
		MSG_BOX("Model is Already Exist");
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, wszModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CDummy Add_Component : (Com_Model)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CDummy::Add_Shader_Component(const wchar_t* wszShaderTag)
{
	if (m_pShader != nullptr)
	{
		MSG_BOX("Shader is Already Exist");
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, wszShaderTag,
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CDummy Add_Component : (Com_Shader)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CDummy::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CDummy Add_Component : (Com_Renderer)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CDummy::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CDummy* CDummy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy* pInstance = New CDummy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy::Clone(void* pArg)
{
	CDummy* pInstance = New CDummy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDummys");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDummy::Free()
{
	__super::Free();
	Safe_Release(m_pTransform);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
