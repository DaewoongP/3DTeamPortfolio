#include "..\Public\Model_LOD.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "GameObject.h"
#include "Shader.h"

_uint CModel_LOD::Get_NumMeshes() const
{
	if (nullptr == m_pCurrentModel)
	{
		MSG_BOX("CurrentModel NULL");
		return 0;
	}

	return m_pCurrentModel->Get_NumMeshes();
}

HRESULT CModel_LOD::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	if (nullptr == m_pCurrentModel)
	{
		MSG_BOX("CurrentModel NULL");
		return E_FAIL;
	}

	return m_pCurrentModel->Bind_BoneMatrices(pShader, pConstantName, iMeshIndex);
}

HRESULT CModel_LOD::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, Engine::TextureType MaterialType)
{
	if (nullptr == m_pCurrentModel)
	{
		MSG_BOX("CurrentModel NULL");
		return E_FAIL;
	}

	return m_pCurrentModel->Bind_Material(pShader, pConstantName, iMeshIndex, MaterialType);
}

CModel_LOD::CModel_LOD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CModel_LOD::CModel_LOD(const CModel_LOD& rhs)
	: CComposite(rhs)
	, m_iNumModels(rhs.m_iNumModels)
{
	for (auto& ModelPair : rhs.m_LodPrototypes)
	{
		m_LodPrototypes.push_back({ ModelPair.first, ModelPair.second });
	}
}

HRESULT CModel_LOD::Initialize_Prototype(LODDESC& LodDesc, const _tchar* szModelFilePath, _uint iNumModel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	CComponent_Manager* pComponent_Manager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponent_Manager);

	for (_uint i = 0; i < iNumModel; ++i)
	{
		_tchar szLodModelFilePath[MAX_STR] = TEXT("");
		wsprintf(szLodModelFilePath, szModelFilePath, i);

		_tchar		wszModelIndex[MAX_STR] = TEXT("");
		_tchar*		pModelPrototypeName = New _tchar[MAX_STR];
		ZeroMemory(pModelPrototypeName, sizeof(_tchar) * MAX_STR);
		_itow_s(i, wszModelIndex, MAX_STR, 10);

		lstrcat(pModelPrototypeName, LodDesc.szPrototypeName);
		lstrcat(pModelPrototypeName, wszModelIndex);
		
		if (0 != i)
		{
			wstring wstrPath = szLodModelFilePath;
			wstrPath = wstrPath.substr(0, wstrPath.size() - 4);
			wstrPath += TEXT("_Lod");
			wstrPath += wszModelIndex;
			wstrPath += TEXT(".dat");
			ZeroMemory(szLodModelFilePath, sizeof(_tchar) * MAX_STR);
			lstrcpy(szLodModelFilePath, wstrPath.c_str());
		}

 		if (FAILED(pComponent_Manager->Add_Prototype(LodDesc.iLevelIndex, pModelPrototypeName,
			CModel::Create(m_pDevice, m_pContext,
				LodDesc.eModelType, szLodModelFilePath, LodDesc.PivotMatrix), true)))
		{
			MSG_BOX("Failed Create Prototype Lods");
			Safe_Release(pComponent_Manager);
			return E_FAIL;
		}

		m_LodPrototypes.push_back({ pModelPrototypeName, LodDesc.iLevelIndex });
	}

	m_iNumModels = iNumModel;
	
	Safe_Release(pComponent_Manager);

	return S_OK;
}

HRESULT CModel_LOD::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fMaxLodDistance = { 100.f };

	return S_OK;
}

void CModel_LOD::Tick(_float fTimeDelta)
{
	Check_CameraDistance();

	if (nullptr != m_pCurrentModel)
		m_pCurrentModel->Tick(fTimeDelta);
}

void CModel_LOD::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pCurrentModel)
		m_pCurrentModel->Late_Tick(fTimeDelta);
}

HRESULT CModel_LOD::Render(_uint iMeshIndex)
{
	if (nullptr == m_pCurrentModel)
		return E_FAIL;

	if (FAILED(m_pCurrentModel->Render(iMeshIndex)))
		return E_FAIL;

	return S_OK;
}

void CModel_LOD::Play_Animation(_float fTimeDelta)
{
	if (nullptr == m_pCurrentModel)
		return;

	m_pCurrentModel->Play_Animation(fTimeDelta);
}

HRESULT CModel_LOD::Add_Components()
{
	_uint iModelIndex = { 0 };

	for (auto& LodPair : m_LodPrototypes)
	{
		CModel*		pModel = { nullptr };
		_tchar* wszComponentName = New _tchar[MAX_STR];
		ZeroMemory(wszComponentName, sizeof(_tchar) * MAX_STR);
		lstrcpy(wszComponentName, TEXT("Com_Model"));

		_tchar		wszModelIndex[MAX_STR] = TEXT("");
		
		_itow_s(iModelIndex, wszModelIndex, MAX_STR, 10);
		++iModelIndex;

		lstrcat(wszComponentName, wszModelIndex);

		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(LodPair.second, LodPair.first,
			wszComponentName, reinterpret_cast<CComponent**>(&pModel))))
		{
			MSG_BOX("Failed CModel_LOD Add_Component Com_LodModels");
			return E_FAIL;
		}

		m_ModelComponentTags.push_back(wszComponentName);

		Safe_Release(pModel);

		if (iModelIndex == m_LodPrototypes.size())
		{
			m_pCurrentModel = pModel;
			Safe_AddRef(m_pCurrentModel);
		}
	}
	
	return S_OK;
}

void CModel_LOD::Check_CameraDistance()
{
	if (nullptr == m_pOwner)
		return;
	
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float3 vOwnerPosition = static_cast<CGameObject*>(m_pOwner)->Get_Transform()->Get_Position();
	_float4 vCamPosition = *pPipeLine->Get_CamPosition();

	Safe_Release(pPipeLine);

	_float fDistance = (vOwnerPosition - vCamPosition.xyz()).Length();

	Safe_Release(m_pCurrentModel);

	for (_uint i = 0; i < m_iNumModels; ++i)
	{
		// 현재 위치가 단계 안에 있을 경우 해당 LOD로 적용
		if (fDistance < m_fMaxLodDistance * _float(i + 1) / m_iNumModels)
		{
			m_pCurrentModel = static_cast<CModel*>(Find_Component(m_ModelComponentTags[i]));
			Safe_AddRef(m_pCurrentModel);
			break;
		}

		if (fDistance > m_fMaxLodDistance)
		{
			m_pCurrentModel = static_cast<CModel*>(Find_Component(m_ModelComponentTags[m_iNumModels - 1]));
			Safe_AddRef(m_pCurrentModel);
			break;
		}
	}
}

CModel_LOD* CModel_LOD::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LODDESC& LodDesc, const _tchar* szModelFilePath, _uint iNumModel)
{
	CModel_LOD* pInstance = New CModel_LOD(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(LodDesc, szModelFilePath, iNumModel)))
	{
		MSG_BOX("Failed to Created CModel_LOD");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel_LOD::Clone(void* pArg)
{
	CModel_LOD* pInstance = New CModel_LOD(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CModel_LOD");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel_LOD::Free()
{
	__super::Free();

	Safe_Release(m_pCurrentModel);

	if (false == m_isCloned)
	{
		for (auto& LodPair : m_LodPrototypes)
		{
			Safe_Delete_Array(LodPair.first);
		}

		m_LodPrototypes.clear();
	}
	else
	{
		for (auto& ModelTag : m_ModelComponentTags)
		{
			Safe_Delete_Array(ModelTag);
		}

		m_ModelComponentTags.clear();
	}
}
