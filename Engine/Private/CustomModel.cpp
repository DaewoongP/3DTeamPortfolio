#include "..\Public\CustomModel.h"

#include "Bone.h"
#include "Mesh.h"
#include "Shader.h"
#include "Notify.h"
#include "Texture.h"
#include "Channel.h"
#include "Animation.h"
#include "Transform.h"
#include "MeshParts.h"
#include "GameInstance.h"

CCustomModel::CCustomModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CModel(pDevice, pContext)
{
}

CCustomModel::CCustomModel(const CCustomModel& rhs)
	: CModel(rhs)
{
}

_uint CCustomModel::Get_NumMeshes(const _uint& _iMeshPartsIndex) const
{
	if (0 > _iMeshPartsIndex || MESH_END <= _iMeshPartsIndex)
	{
		MSG_BOX("[CCustomModel] Range Of Out Array");
		return E_FAIL;
	}

	if (nullptr == m_MeshParts[_iMeshPartsIndex].pMeshParts)
		return 0;

	return m_MeshParts[_iMeshPartsIndex].pMeshParts->Get_NumMeshes();
}

void CCustomModel::Set_WindVelocity(_float3 vWindVelocity)
{
	for (auto& Mesh : m_MeshParts)
	{
		if (nullptr != Mesh.pMeshParts)
			Mesh.pMeshParts->Set_WindVelocity(vWindVelocity);
	}
}

HRESULT CCustomModel::Set_Top_For_Robe(const _uint& _iLevelIndex)
{
	if (true == m_isRobeTop)
		return S_OK;

	wstring wstrPrototypeTag = m_MeshParts[TOP].wstrPrototypeTag;
	wstrPrototypeTag += TEXT("_A");
	
	if (FAILED(Add_MeshParts(_iLevelIndex, wstrPrototypeTag, TOP)))
		return E_FAIL;

	m_isRobeTop = true;

	return S_OK;
}

HRESULT CCustomModel::Set_Top_For_NonRobe(const _uint& _iLevelIndex)
{
	if (false == m_isRobeTop)
		return S_OK;

	wstring wstrPrototypeTag = m_MeshParts[TOP].wstrPrototypeTag;
	wstrPrototypeTag = wstrPrototypeTag.substr(0, wstrPrototypeTag.size() - 2);

	if (FAILED(Add_MeshParts(_iLevelIndex, wstrPrototypeTag, TOP)))
		return E_FAIL;

	m_isRobeTop = false;

	return S_OK;
}

HRESULT CCustomModel::Initialize_Prototype(TYPE eType, const wstring& _wstrModelFilePath, _float4x4 _PivotMatrix)
{
	if (FAILED(__super::Initialize_Prototype(eType, _wstrModelFilePath.c_str(), _PivotMatrix)))
	{
		MSG_BOX("[CCustomModel] Failed __super::Initialize_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCustomModel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("[CCustomModel] Failed __super::Initialize");
		return E_FAIL;
	}

	return S_OK;
}

void CCustomModel::Tick(const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex, _float _fTimeDelta)
{
	if (0 > _iMeshPartsIndex || MESH_END <= _iMeshPartsIndex)
	{
		MSG_BOX("[CCustomModel] Range Of Out Array");
		return;
	}

	if (nullptr == m_MeshParts[_iMeshPartsIndex].pMeshParts)
		return;

	m_MeshParts[_iMeshPartsIndex].pMeshParts->Tick(_iMeshIndex, _fTimeDelta);
}

HRESULT CCustomModel::Render(const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex)
{
	if (0 > _iMeshPartsIndex || MESH_END <= _iMeshPartsIndex)
	{
		MSG_BOX("[CCustomModel] Range Of Out Array");
		return E_FAIL;
	}

	if (nullptr == m_MeshParts[_iMeshPartsIndex].pMeshParts)
		return S_OK;

	return m_MeshParts[_iMeshPartsIndex].pMeshParts->Render(_iMeshIndex);
}

HRESULT CCustomModel::Bind_Material(CShader* _pShader, const char* _pConstantName,
	const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex, Engine::TextureType _MaterialType)
{
	if (nullptr == m_MeshParts[_iMeshPartsIndex].pMeshParts)
		return S_OK;

	return m_MeshParts[_iMeshPartsIndex].pMeshParts->Bind_Material(_pShader, _pConstantName, _iMeshIndex, _MaterialType);
}

HRESULT CCustomModel::Bind_BoneMatrices(CShader* pShader, const char* pConstantName, const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex)
{
	if (nullptr == m_MeshParts[_iMeshPartsIndex].pMeshParts)
		return S_OK;

	_float4x4		BoneMatrices[MAX_SHADERMATRIX];
	ZeroMemory(BoneMatrices, sizeof(_float4x4) * MAX_SHADERMATRIX);

	m_MeshParts[_iMeshPartsIndex].pMeshParts->Get_Matrices(_iMeshIndex, m_Bones, BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

	pShader->Bind_Matrices(pConstantName, BoneMatrices, MAX_SHADERMATRIX);

	return S_OK;
}

HRESULT CCustomModel::Bind_Color(CShader* _pShader, const char* _pConstantName, const _uint& _iMeshPartsIndex)
{
	if (nullptr == m_MeshParts[_iMeshPartsIndex].pMeshParts)
		return S_OK;

	_float4 vColor = m_MeshParts[_iMeshPartsIndex].pMeshParts->Get_Parts_Color();
	_pShader->Bind_RawValue(_pConstantName, &vColor, sizeof(_float4));

	return S_OK;
}

HRESULT CCustomModel::Add_MeshParts(const _uint& _iLevelIndex, const wstring& _wstrPrototypeTag, 
	MESHTYPE _eMeshPartsType, const _float4& _vColor, const _tchar* _szClothDataFilePath)
{
	if (0 > _eMeshPartsType || MESH_END <= _eMeshPartsType)
	{
		MSG_BOX("[CCustomModel] Range Of Out Array");
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CMeshParts::MESHPARTSDESC MeshPartsDesc;
	MeshPartsDesc.pBones = &m_Bones;
	MeshPartsDesc.szClothDataFilePath = _szClothDataFilePath;
	MeshPartsDesc.vColor = _vColor;

	CMeshParts* pMeshParts = { nullptr };
	if (TOP == _eMeshPartsType &&
		true == m_isRobeTop)
	{
		wstring wstrPrototypeTag = _wstrPrototypeTag + TEXT("_A");
		pMeshParts = static_cast<CMeshParts*>(pGameInstance->Clone_Component(_iLevelIndex, wstrPrototypeTag.c_str(), &MeshPartsDesc));
	}
	else
		pMeshParts = static_cast<CMeshParts*>(pGameInstance->Clone_Component(_iLevelIndex, _wstrPrototypeTag.c_str(), &MeshPartsDesc));

	Safe_Release(pGameInstance);

	if (nullptr == pMeshParts)
	{
		MSG_BOX("[CCustomModel] pMeshParts is nullptr");
		return E_FAIL;
	}

	if (nullptr != m_MeshParts[_eMeshPartsType].pMeshParts)
		Safe_Release(m_MeshParts[_eMeshPartsType].pMeshParts);

	m_MeshParts[_eMeshPartsType].pMeshParts = pMeshParts;
	m_MeshParts[_eMeshPartsType].wstrPrototypeTag = _wstrPrototypeTag;
	pMeshParts->Set_MeshType(_eMeshPartsType);
	
	return S_OK;
}

CCustomModel* CCustomModel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, TYPE eType, const wstring& _wstrModelFilePath, _float4x4 _PivotMatrix)
{
	CCustomModel* pInstance = New CCustomModel(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize_Prototype(eType, _wstrModelFilePath, _PivotMatrix)))
	{
		MSG_BOX("Failed to Created CCustomModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCustomModel::Clone(void* pArg)
{
	CCustomModel* pInstance = New CCustomModel(*this);

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
		Safe_Release(pMeshParts.pMeshParts);
	}
}