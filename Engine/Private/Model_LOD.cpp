#include "..\Public\Model_LOD.h"

CModel_LOD::CModel_LOD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CModel_LOD::CModel_LOD(const CModel_LOD& rhs)
	: CComposite(rhs)
{
}

HRESULT CModel_LOD::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	/* For.Prototype_Component_Model_Weopon_Armored_Troll */
		/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Weopon_Armored_Troll"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Troll_Armored_Club/Troll_Armored_Club.dat"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Weopon_Armored_Troll");*/
	return S_OK;
}

HRESULT CModel_LOD::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == pArg)
	{
		MSG_BOX("Failed Get LOD Data");
		return E_FAIL;
	}

	if (FAILED(Add_Components(*reinterpret_cast<LODDESC*>(pArg))))
		return E_FAIL;

	return S_OK;
}

void CModel_LOD::Tick(_float fTimeDelta)
{
}

void CModel_LOD::Late_Tick(_float fTimeDelta)
{
}

HRESULT CModel_LOD::Render()
{
	return S_OK;
}

HRESULT CModel_LOD::Add_Components(const LODDESC& LodDesc)
{
	_uint iIndex = { 0 };
	//for (auto& LodData : LodDesc.LODModels)
	//{
	//	CModel*			pModel = { nullptr };
	//	_tchar			wszComponentName[MAX_STR] = TEXT("Com_Model");

	//	/* For.Com_Model */
	//	if (FAILED(CComposite::Add_Component(LodDesc.LODModels[0].first, LodDesc.LODModels[0].second,
	//		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&pModel))))
	//	{
	//		MSG_BOX("Failed CModel_LOD Add_Component : (Com_Model)");
	//		return E_FAIL;
	//	}
	//}
	
	return S_OK;
}

CModel_LOD* CModel_LOD::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* szModelFilePath, _uint iNumModel)
{
	CModel_LOD* pInstance = New CModel_LOD(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
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
}
