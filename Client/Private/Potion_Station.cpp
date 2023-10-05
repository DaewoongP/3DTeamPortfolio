#include "..\Public\Potion_Station.h"
#include "GameInstance.h"

CPotion_Station::CPotion_Station(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPotion_Station::CPotion_Station(const CPotion_Station& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPotion_Station::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPotion_Station::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CPotion_Station Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_ObjectDesc = *reinterpret_cast<MAPOBJECTDESC*>(pArg);
	m_pTransform->Set_WorldMatrix(m_ObjectDesc.WorldMatrix);

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPotion_Station::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CPotion_Station Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	// 리지드 바디 설정

	return S_OK;
}

void CPotion_Station::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPotion_Station::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	BEGININSTANCE;

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		//m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}

	ENDINSTANCE;
}

HRESULT CPotion_Station::Render()
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

		m_pShader->Begin("Mesh");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPotion_Station::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CPotion_Station Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CPotion_Station Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}
}

HRESULT CPotion_Station::SetUp_ShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

void CPotion_Station::Check_MinMaxPoint(_float3 vPoint)
{
	if (m_vMinPoint.x > vPoint.x)
		m_vMinPoint.x = vPoint.x;
	if (m_vMinPoint.y > vPoint.y)
		m_vMinPoint.y = vPoint.y;
	if (m_vMinPoint.z > vPoint.z)
		m_vMinPoint.z = vPoint.z;

	if (m_vMaxPoint.x < vPoint.x)
		m_vMaxPoint.x = vPoint.x;
	if (m_vMaxPoint.y < vPoint.y)
		m_vMaxPoint.y = vPoint.y;
	if (m_vMaxPoint.z < vPoint.z)
		m_vMaxPoint.z = vPoint.z;
}

CPotion_Station* CPotion_Station::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPotion_Station* pInstance = New CPotion_Station(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPotion_Station");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPotion_Station::Clone(void* pArg)
{
	CPotion_Station* pInstance = New CPotion_Station(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPotion_Station");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPotion_Station::Free()
{
	__super::Free();

	//Safe_Release(m_pRigidBody);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
