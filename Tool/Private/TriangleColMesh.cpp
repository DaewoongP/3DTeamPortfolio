#include "TriangleColMesh.h"
#include "GameInstance.h"
#include "Mesh.h"
CTriangleColMesh::CTriangleColMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTriangleColMesh::CTriangleColMesh(const CTriangleColMesh& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTriangleColMesh::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTriangleColMesh::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_float4x4* vWorldMatrix = (_float4x4*)pArg;
		m_pTransform->Set_WorldMatrix(*vWorldMatrix);
	}

	return S_OK;
}

void CTriangleColMesh::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CTriangleColMesh::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_PICKING, this);
	}
}

HRESULT CTriangleColMesh::Render()
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

//현재 내 모델에 들어있는 모든 매쉬들을 순회하며
//정점들을 vector에 담아서 반환해줍니다.
vector<_float3> CTriangleColMesh::Get_VertexPositions()
{
	if(m_pModel==nullptr)
		return vector<_float3>();

	vector<_float3> vertexPointVec;

	vector<CMesh*>* MeshVec = m_pModel->Get_MeshesVec();
	for (CMesh* mesh : (*MeshVec))
	{
		_uint max = mesh->Get_NumVertices();
		vector<_float3>* meshVertexVec = mesh->Get_VerticesPositionVec();
		for (_uint i = 0; i < max; i++)
		{
			vertexPointVec.push_back((*meshVertexVec)[i]);
		}
	}
	return vertexPointVec;
}

//현재 내 모델에 들어있는 모든 매쉬들을 순회하며
//인덱스들을 vector에 담아서 반환해줍니다.
vector<_ulong> CTriangleColMesh::Get_Indices()
{
	if (m_pModel == nullptr)
		return vector<_ulong>();

	vector<_ulong> IndicesVec;

	vector<CMesh*>* MeshVec = m_pModel->Get_MeshesVec();
	for (CMesh* mesh : (*MeshVec))
	{
		_uint max = mesh->Get_NumIndices();
		vector<PxU32>* meshIndicesVec = mesh->Get_IndicesVec();
		for (_uint i = 0; i < max; i++)
		{
			IndicesVec.push_back((*meshIndicesVec)[i]);
		}
	}
	return IndicesVec;
}

HRESULT CTriangleColMesh::Add_Model_Component(const _tchar* wszModelTag)
{
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, wszModelTag,
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CTriangleColMesh Add_Component : (Com_Buffer)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CTriangleColMesh::Add_Shader_Component(const _tchar* wszShaderTag)
{
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, wszShaderTag,
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CTriangleColMesh Add_Component : (Com_Shader)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CTriangleColMesh::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CTriangleColMesh Add_Component : (Com_Renderer)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CTriangleColMesh::SetUp_ShaderResources()
{
	BEGININSTANCE; if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL; ENDINSTANCE;

	return S_OK;
}

CTriangleColMesh* CTriangleColMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTriangleColMesh* pInstance = New CTriangleColMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTriangleColMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTriangleColMesh::Clone(void* pArg)
{
	CTriangleColMesh* pInstance = New CTriangleColMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTriangleColMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTriangleColMesh::Free()
{
	__super::Free();
	Safe_Release(m_pTransform);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
