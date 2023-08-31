#include "..\Public\CustomModel.h"

#include "Bone.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"

CCustomModel::CCustomModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CCustomModel::CCustomModel(const CCustomModel& rhs)
	: CComponent(rhs)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
}

const CBone* CCustomModel::Get_Bone(const _tchar* pBoneName)
{
	CModel::BONES Bones = m_MeshParts[HEAD]->Get_Bones();

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)
		{
			if (!lstrcmp(pBoneName, pBone->Get_Name()))
				return true;
			else
				return false;
		});

	if (iter == Bones.end())
		return nullptr;

	return *iter;
}

const _float4x4* CCustomModel::Get_BoneCombinedTransformationMatrixPtr(_uint iIndex)
{
	CModel::BONES Bones = m_MeshParts[HEAD]->Get_Bones();

	return Bones[iIndex]->Get_CombinedTransformationMatrixPtr();
}

_float4x4 CCustomModel::Get_BoneCombinedTransformationMatrix(_uint iIndex)
{
	CModel::BONES Bones = m_MeshParts[HEAD]->Get_Bones();

	return Bones[iIndex]->Get_CombinedTransformationMatrix();
}

HRESULT CCustomModel::Add_MeshParts(const _uint& _iMeshPartsIndex, const wstring& _wstrMeshPartsFilePath)
{
	if (nullptr != m_MeshParts[_iMeshPartsIndex])
		Safe_Release(m_MeshParts[_iMeshPartsIndex]);

	CMeshParts* pMeshParts = CMeshParts::Create(m_pDevice, m_pContext, _wstrMeshPartsFilePath);
	if (nullptr == pMeshParts)
	{
		MSG_BOX("[CCustomModel] pMeshParts is nullptr");
		return E_FAIL;
	}

	m_MeshParts[_iMeshPartsIndex] = pMeshParts;
	return S_OK;
}

HRESULT CCustomModel::Initialize_Prototype(_float4x4 PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	return S_OK;
}

HRESULT CCustomModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CCustomModel::Render(const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex)
{
	if (nullptr == m_MeshParts[_iMeshPartsIndex])
		return S_OK;

	return m_MeshParts[_iMeshPartsIndex]->Render(_iMeshIndex);
}

HRESULT CCustomModel::Find_BoneIndex(const _tchar* pBoneName, _uint* iIndex)
{
	CModel::BONES Bones = m_MeshParts[HEAD]->Get_Bones();

	*iIndex = 0;
	auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pValue) {
		if (!lstrcmp(pBoneName, pValue->Get_Name()))
			return true;
		else
		{
			++(*iIndex);
			return false;
		}
		});

	if (Bones.end() == iter)
	{
		MSG_BOX("Failed Find BoneIndex");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCustomModel::Bind_Material(CShader* _pShader, const char* _pConstantName, 
	const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex, Engine::TextureType _MaterialType)
{
	if (nullptr == m_MeshParts[_iMeshPartsIndex])
		return S_OK;

	return m_MeshParts[_iMeshPartsIndex]->Bind_Material(_pShader, _pConstantName, _iMeshIndex, _MaterialType);
}

HRESULT CCustomModel::Bind_BoneMatrices(CShader* pShader, const char* pConstantName, const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex)
{
	if (nullptr == m_MeshParts[_iMeshPartsIndex])
		return S_OK;

	CModel::BONES Bones = m_MeshParts[HEAD]->Get_Bones();

	_float4x4		BoneMatrices[MAX_SHADERMATRIX];
	ZeroMemory(BoneMatrices, sizeof(_float4x4) * MAX_SHADERMATRIX);

	m_MeshParts[_iMeshPartsIndex]->Get_Matrices(_iMeshIndex, Bones, BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

	pShader->Bind_Matrices(pConstantName, BoneMatrices, MAX_SHADERMATRIX);

	return S_OK;
}

CCustomModel* CCustomModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float4x4 PivotMatrix)
{
	CCustomModel* pInstance = new CCustomModel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(PivotMatrix)))
	{
		MSG_BOX("Failed to Created CCustomModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCustomModel::Clone(void* pArg)
{
	CCustomModel* pInstance = new CCustomModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCustomModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomModel::Free()
{
	__super::Free();

	for (auto& pMeshParts : m_MeshParts)
	{
		Safe_Release(pMeshParts);
	}
}
