#include "Model_Instance.h"

#include "Mesh.h"
#include "Texture.h"
#include "Mesh_Instance.h"

CModel_Instance::CModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CModel_Instance::CModel_Instance(const CModel_Instance& rhs)
	: CComponent(rhs)
	, m_pInstanceMatrix(rhs.m_pInstanceMatrix)
	, m_InstanceCnt(rhs.m_InstanceCnt)
{
}

HRESULT CModel_Instance::Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _float4x4& PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	return E_NOTIMPL;
}

HRESULT CModel_Instance::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

HRESULT CModel_Instance::Render()
{
	return E_NOTIMPL;
}

HRESULT CModel_Instance::Bind_Material(CShader* pShader, const char* pConstantName, _uint iMeshIndex, TextureType MaterialType)
{
	return E_NOTIMPL;
}

HRESULT CModel_Instance::Ready_Meshes(TYPE eType, _float4x4& PivotMatrix)
{
	return E_NOTIMPL;
}

HRESULT CModel_Instance::Ready_Materials(const _tchar* pModelFilePath)
{
	return E_NOTIMPL;
}

CModel_Instance* CModel_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, _float4x4& PivotMatrix)
{
	return nullptr;
}

CComponent* CModel_Instance::Clone(void* pArg)
{
	return nullptr;
}

void CModel_Instance::Free()
{
}
