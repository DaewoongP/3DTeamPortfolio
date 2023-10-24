#include "..\Public\FireHouse.h"
#include "GameInstance.h"

CFireHouse::CFireHouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CFireHouse::CFireHouse(const CFireHouse& rhs)
	: CMapObject(rhs)
{
}

HRESULT CFireHouse::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFireHouse::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CFireHouse Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFireHouse::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	// �ǹ� ���� �ľ�
	wstring wsTargetName(TEXT("Prototype_Component_Model_"));
	wstring wsModelName(m_ObjectDesc.wszTag);

	_uint iLength = wsTargetName.size();
	wsModelName = wsModelName.substr(iLength);

	wstring wsF(TEXT("SM_HM_Gen_F"));
	wstring wsG(TEXT("SM_HM_Gen_G"));
	wstring wsHoney(TEXT("SM_HM_Honeydukes"));
	wstring wsQuill(TEXT("SM_HM_Quill"));
	wstring wsTea(TEXT("SM_HM_TeaShop"));
	wstring wsDB(TEXT("SM_HM_DB_GR"));

	if (0 == lstrcmp(wsModelName.c_str(), wsF.c_str()))
	{
		m_eHouseType = HOUSE_GEN_F;
	}
	else if (0 == lstrcmp(wsModelName.c_str(), wsG.c_str()))
	{
		m_eHouseType = HOUSE_GEN_G;
	}
	else if (0 == lstrcmp(wsModelName.c_str(), wsHoney.c_str()))
	{
		m_eHouseType = HOUSE_HONEY;
	}
	else if (0 == lstrcmp(wsModelName.c_str(), wsQuill.c_str()))
	{
		m_eHouseType = HOUSE_QUILL;
	}
	else if (0 == lstrcmp(wsModelName.c_str(), wsTea.c_str()))
	{
		m_eHouseType = HOUSE_TEASHOP;
	}
	else if (0 == lstrcmp(wsModelName.c_str(), wsDB.c_str()))
	{
		m_eHouseType = HOUSE_DB_GR;
	}

	return S_OK;
}

void CFireHouse::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CFireHouse::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}
}

HRESULT CFireHouse::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModel->Bind_Material(m_pShader, "g_NormalTexture", iMeshCount, NORMALS);

		m_pShader->Begin("Mesh_No_Cull");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

CFireHouse* CFireHouse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFireHouse* pInstance = New CFireHouse(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CFireHouse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFireHouse::Clone(void* pArg)
{
	CFireHouse* pInstance = New CFireHouse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CFireHouse");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFireHouse::Free()
{
	__super::Free();
}