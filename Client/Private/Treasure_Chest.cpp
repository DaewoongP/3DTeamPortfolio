#include "..\Public\Treasure_Chest.h"
#include "GameInstance.h"

CTreasure_Chest::CTreasure_Chest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTreasure_Chest::CTreasure_Chest(const CTreasure_Chest& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTreasure_Chest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTreasure_Chest::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CTreasure_Chest Argument is NULL");
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

HRESULT CTreasure_Chest::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CTreasure_Chest Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	// 리지드 바디 설정

	return S_OK;
}

void CTreasure_Chest::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (nullptr != m_pModel)
		m_pModel->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
		
}

void CTreasure_Chest::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	BEGININSTANCE; 
	
	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
#ifdef _DEBUG
		//m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
	
	ENDINSTANCE;
}

HRESULT CTreasure_Chest::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", iMeshCount);

		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModel->Bind_Material(m_pShader, "g_NormalTexture", iMeshCount, NORMALS);

		m_pShader->Begin("AnimMesh");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTreasure_Chest::Render_Depth()
{
	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_BoneMatrices(m_pShadowShader, "g_BoneMatrices", iMeshCount);

		if (FAILED(m_pShadowShader->Begin("Shadow")))
			return E_FAIL;

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTreasure_Chest::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CTreasure_Chest Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CTreasure_Chest Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShader))))
	{
		MSG_BOX("Failed CTreasure_Chest Add_Component : (Com_ShadowShader)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTreasure_Chest::SetUp_ShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

HRESULT CTreasure_Chest::SetUp_ShadowShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShadowShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_LightTransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_LightTransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

void CTreasure_Chest::Check_MinMaxPoint(_float3 vPoint)
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

CTreasure_Chest* CTreasure_Chest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTreasure_Chest* pInstance = New CTreasure_Chest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTreasure_Chest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTreasure_Chest::Clone(void* pArg)
{
	CTreasure_Chest* pInstance = New CTreasure_Chest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTreasure_Chest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTreasure_Chest::Free()
{
	__super::Free();

	//Safe_Release(m_pRigidBody);
	Safe_Release(m_pShadowShader);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
