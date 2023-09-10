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

	m_pModelCom->Play_Animation(0.f);
}

void CDummy_Cloth::Set_MeshIndex(_uint _iMeshIndex)
{
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
}

void CDummy_Cloth::Late_Tick(_float fTimeDelta)
{
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
					m_pShaderCom->Begin("AnimMesh");
				else
					m_pShaderCom->Begin("AnimColorMesh");

				m_pModelCom->Render(iPartsIndex, i);
			}
		}
	}

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
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
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