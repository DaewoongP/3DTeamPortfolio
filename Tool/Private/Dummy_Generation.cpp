#include "Dummy_Generation.h"
#include "GameInstance.h"

CDummy_Generation::CDummy_Generation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummy_Generation::CDummy_Generation(const CDummy_Generation& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummy_Generation::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummy_Generation::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_float3* vPos = (_float3*)pArg;
		m_pTransform->Set_Position(*vPos);
	}

	return S_OK;
}

void CDummy_Generation::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CDummy_Generation::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}
}

HRESULT CDummy_Generation::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (nullptr == m_pShader ||
		nullptr == m_pModel)
		return S_OK;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; iMeshCount++)
	{
		m_pModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", iMeshCount);
		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);

	
		m_pShader->Begin("Default");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CDummy_Generation::Add_Model_Component(const wchar_t* wszModelTag)
{
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, wszModelTag,
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CDummy_Generation Add_Component : (Com_Buffer)");
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_TriangleMesh"),
		TEXT("Com_TriangleMesh"), reinterpret_cast<CComponent**>(&m_pTriangleMesh),m_pModel->Get_MeshesVec())))
	{
		MSG_BOX("Failed CDummy_Generation Add_Component : (Com_TriangleMesh)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDummy_Generation::Add_Shader_Component(const wchar_t* wszShaderTag)
{
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, wszShaderTag,
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CDummy_Generation Add_Component : (Com_Shader)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CDummy_Generation::Change_Model_Component(const _tchar* pPrototypeTag)
{
	if (FAILED(CComposite::Delete_Component(TEXT("Com_Buffer"))))
	{
		MSG_BOX("Failed to chagne CDummy_Generation Model");
		return E_FAIL;
	}

	if (FAILED(CComposite::Delete_Component(TEXT("Com_TriangleMesh"))))
	{
		MSG_BOX("Failed to chagne CDummy_Generation Com_TriangleMesh");
		return E_FAIL;
	}

	Safe_Release(m_pModel);
	Safe_Release(m_pTriangleMesh);

	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, pPrototypeTag,
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CDummy_Generation Change_Component : (Com_Buffer)");
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_TriangleMesh"),
		TEXT("Com_TriangleMesh"), reinterpret_cast<CComponent**>(&m_pTriangleMesh))))
	{
		MSG_BOX("Failed CDummy_Generation Change_Component : (Com_TriangleMesh)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDummy_Generation::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CDummy_Generation Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDummy_Generation::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CDummy_Generation* CDummy_Generation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_Generation* pInstance = New CDummy_Generation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDummy_Generation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_Generation::Clone(void* pArg)
{
	CDummy_Generation* pInstance = New CDummy_Generation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDummy_Generation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDummy_Generation::Free()
{
	__super::Free();
	
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer); 
	Safe_Release(m_pTriangleMesh);
}