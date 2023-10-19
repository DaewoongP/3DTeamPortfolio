#include "..\Public\TrollHouse.h"
#include "GameInstance.h"

CTrollHouse::CTrollHouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTrollHouse::CTrollHouse(const CTrollHouse& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTrollHouse::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrollHouse::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CTrollHouse Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_ObjectDesc = *reinterpret_cast<MAPOBJECTDESC*>(pArg);
	m_pTransform->Set_WorldMatrix(m_ObjectDesc.WorldMatrix);

	return S_OK;
}

HRESULT CTrollHouse::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CTrollHouse Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

void CTrollHouse::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (nullptr != m_pModel)
		//m_pModel->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
		m_pModel->Play_Animation(0.f, CModel::UPPERBODY, m_pTransform);
}

void CTrollHouse::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}
}

HRESULT CTrollHouse::Render()
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

HRESULT CTrollHouse::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	if (FAILED(SetUp_ShadowShaderResources(LightViewMatrix, LightProjMatrix)))
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

HRESULT CTrollHouse::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CCat Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}


	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CCat Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShader))))
	{
		MSG_BOX("Failed CCat Add_Component : (Com_ShadowShader)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTrollHouse::SetUp_ShaderResources()
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

HRESULT CTrollHouse::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	BEGININSTANCE;

	if (FAILED(m_pShadowShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ViewMatrix", &LightViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ProjMatrix", &LightProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

CTrollHouse* CTrollHouse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrollHouse* pInstance = New CTrollHouse(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTrollHouse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrollHouse::Clone(void* pArg)
{
	CTrollHouse* pInstance = New CTrollHouse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CGameObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTrollHouse::Free()
{
	__super::Free();

	Safe_Release(m_pShadowShader);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
