#include "Model_Anim_Instance.h"
#include "Model_Anim_Instance.h"

#include "Texture.h"
#include "Mesh_Instance.h"

CModel_Anim_Instance::CModel_Anim_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CModel(pDevice, pContext)
{
}

CModel_Anim_Instance::CModel_Anim_Instance(const CModel_Anim_Instance& rhs)
	: CModel(rhs)
{
	for (auto& pMesh : m_Meshes)
	{
		Safe_AddRef(pMesh);
	}

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMtrlTexture)
			Safe_AddRef(pTexture);
	}
}

HRESULT CModel_Anim_Instance::Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _float4x4* pInstanceMatrix, _uint iInstanceCnt, _float4x4 PivotMatrix)
{
	if (FAILED(__super::Initialize_Prototype(eType, pModelFilePath, PivotMatrix)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_Anim_Instance::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_Anim_Instance::Render(_uint iMeshIndex)
{
	if (FAILED(__super::Render(iMeshIndex)))
		return E_FAIL;

	return S_OK;
}

CModel_Anim_Instance* CModel_Anim_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, _float4x4* pInstanceMatrix, _uint iInstanceCnt, _float4x4 PivotMatrix)
{
	CModel_Anim_Instance* pInstance = New CModel_Anim_Instance(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, pInstanceMatrix, iInstanceCnt, PivotMatrix)))
	{
		MSG_BOX("Failed to Created CModel_Anim_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel_Anim_Instance::Clone(void* pArg)
{
	CModel_Anim_Instance* pInstance = New CModel_Anim_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CModel_Anim_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel_Anim_Instance::Free()
{
	__super::Free();
}
