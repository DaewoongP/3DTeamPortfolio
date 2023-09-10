#include "..\Public\Dummy_Cloth.h"
#include "GameInstance.h"
#include "MeshParts.h"

CDummy_Cloth::CDummy_Cloth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummy_Cloth::CDummy_Cloth(const CDummy_Cloth& rhs)
	: CGameObject(rhs)
{
}

_bool CDummy_Cloth::Get_VertexIndex_By_Picking(_Inout_ _uint* pVertexIndex, _Inout_ _float3* pPickPosition)
{
	if (nullptr == m_pCurrent_Dynamic_Mesh)
		return false;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (false == pGameInstance->IsMouseInClient(m_pContext, g_hWnd))
	{
		Safe_Release(pGameInstance);
		return false;
	}

	vector<_float3> Vertices = m_pCurrent_Dynamic_Mesh->Get_VertexPositions();

	vector<_ulong> Indices = m_pCurrent_Dynamic_Mesh->Get_Indices();

	_float4 vMouseOrigin, vMouseDirection;
	if (FAILED(pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vMouseOrigin, &vMouseDirection)))
	{
		Safe_Release(pGameInstance);
		return false;
	}
	vMouseDirection.Normalize();

	Safe_Release(pGameInstance);
	
	_float fDist = 1000.f;
	_float fReturnDist = fDist;

	_float3 vVertex0, vIntersectVertex0;
	_float3 vVertex1, vIntersectVertex1;
	_float3 vVertex2, vIntersectVertex2;
	_uint iIndex0 = { 0 };
	_bool	isIntersects = { false };

	for (_uint i = 0; i < Indices.size() - 3; ++i)
	{
		vVertex0 = Vertices[Indices[i]];
		vVertex1 = Vertices[Indices[i + 1]];
		vVertex2 = Vertices[Indices[i + 2]];

		if (TriangleTests::Intersects(vMouseOrigin, vMouseDirection, vVertex0, vVertex1, vVertex2, fDist))
		{
			if (fDist < fReturnDist)
			{
				fReturnDist = fDist;
				iIndex0 = i;

				vIntersectVertex0 = vVertex0;
				vIntersectVertex1 = vVertex1;
				vIntersectVertex2 = vVertex2;

				isIntersects = true;
			}
		}
	}

	if (false == isIntersects)
		return false;

	_float3 vIntersectPosition = vMouseOrigin.xyz() + vMouseDirection.xyz() * fReturnDist;

	_float fVertexDist0 = (vIntersectPosition - vIntersectVertex0).Length();
	_float fVertexDist1 = (vIntersectPosition - vIntersectVertex1).Length();
	_float fVertexDist2 = (vIntersectPosition - vIntersectVertex2).Length();
	
	if (fVertexDist0 <= fVertexDist1 &&
		fVertexDist0 <= fVertexDist2)
	{
		*pVertexIndex = Indices[iIndex0];
		*pPickPosition = vIntersectVertex0;
		return true;
	}

	if (fVertexDist1 <= fVertexDist0 &&
		fVertexDist1 <= fVertexDist2)
	{
		*pVertexIndex = Indices[iIndex0 + 1];
		*pPickPosition = vIntersectVertex1;
		return true;
	}

	if (fVertexDist2 <= fVertexDist1 &&
		fVertexDist2 <= fVertexDist0)
	{
		*pVertexIndex = Indices[iIndex0 + 2];
		*pPickPosition = vIntersectVertex2;
		return true;
	}

	return false;
}

void CDummy_Cloth::Set_Model_Component(CCustomModel::MESHTYPE _eMeshType, const _tchar* _pModelTag)
{
	if (nullptr != m_pModelCom)
		Safe_Release(m_pModelCom);

	/* Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_CustomModel"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("Failed CDummy_Cloth Add_Component : (Com_Model)");
		return;
	}

	/* For.Prototype_Component_MeshParts_Robe_Student */
	if (FAILED(m_pModelCom->Add_MeshParts(LEVEL_TOOL, _pModelTag, _eMeshType, TEXT("Default"))))
	{
		MSG_BOX("CDummy_Cloth Failed Add_MeshParts");
		return;
	}

	m_eMeshPartsType = _eMeshType;

	//m_pModelCom->Play_Animation(0.f);
}

void CDummy_Cloth::Set_MeshIndex(_uint _iMeshIndex)
{
	if (nullptr == m_pModelCom)
		return;

	array<CMeshParts*, CCustomModel::MESH_END> MeshParts = m_pModelCom->Get_MeshParts();
	for (_uint i = 0; i < CCustomModel::MESH_END; ++i)
	{
		if (nullptr == MeshParts[i])
			continue;

		const vector<class CMesh*> Meshes = static_cast<CMeshParts*>(MeshParts[i])->Get_Meshes();

		for (auto Mesh : Meshes)
		{
			static_cast<CDynamic_Mesh*>(Mesh)->Reset_Position();
		}

		if (i == m_eMeshPartsType)
		{
			if (nullptr != m_pCurrent_Dynamic_Mesh)
			{
				Safe_Release(m_pCurrent_Dynamic_Mesh);
			}

			m_pCurrent_Dynamic_Mesh = static_cast<CDynamic_Mesh*>(Meshes[_iMeshIndex]);
			Safe_AddRef(m_pCurrent_Dynamic_Mesh);
		}
	}

	m_iMeshIndex = _iMeshIndex;
}

HRESULT CDummy_Cloth::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummy_Cloth::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CDummy_Cloth::Tick(_float fTimeDelta)
{
	if (nullptr != m_pModelCom &&
		true == m_isTesting)
		m_pModelCom->Tick(m_eMeshPartsType, m_iMeshIndex, fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CDummy_Cloth::Late_Tick(_float fTimeDelta)
{
	if (true == m_isRemakeMesh)
	{
		m_isRemakeMesh = false;

		m_pCurrent_Dynamic_Mesh->Remake_ClothMesh(m_InvMasses);
	}

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}
}

HRESULT CDummy_Cloth::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (nullptr != m_pModelCom)
	{
		for (_uint iPartsIndex = 0; iPartsIndex < CCustomModel::MESH_END; ++iPartsIndex)
		{
			_uint		iNumMeshes = m_pModelCom->Get_NumMeshes(iPartsIndex);

			for (_uint i = 0; i < iNumMeshes; ++i)
			{
				m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", iPartsIndex, i);

				m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", iPartsIndex, i, DIFFUSE);
				if (m_iMeshIndex != i)
					m_pShaderCom->Begin("Default");
				else
				{
					if (true == m_isWireFrame)
					{
						if (true == m_isMeshHighLight)
							m_pShaderCom->Begin("MeshWireColor");
						else
							m_pShaderCom->Begin("MeshWire");
					}
					else
					{
						if (true == m_isMeshHighLight)
							m_pShaderCom->Begin("MeshColor");
						else
							m_pShaderCom->Begin("Default");
					}
				}

				m_pModelCom->Render(iPartsIndex, i);
			}
		}
	}

	return S_OK;
}

void CDummy_Cloth::Reset_Position()
{
	if (nullptr == m_pCurrent_Dynamic_Mesh)
		return;

	m_pCurrent_Dynamic_Mesh->Reset_Position();
}

HRESULT CDummy_Cloth::Remake_ClothMesh(vector<_float> InvMasses)
{
	m_isRemakeMesh = true;
	m_InvMasses.clear();
	m_InvMasses = InvMasses;

	return S_OK;
}

HRESULT CDummy_Cloth::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CMapDummy Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CDummy_Cloth Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDummy_Cloth::SetUp_ShaderResources()
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

CDummy_Cloth* CDummy_Cloth::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_Cloth* pInstance = New CDummy_Cloth(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDummy_Cloth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_Cloth::Clone(void* pArg)
{
	CDummy_Cloth* pInstance = New CDummy_Cloth(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDummy_Cloth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy_Cloth::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCurrent_Dynamic_Mesh);
}