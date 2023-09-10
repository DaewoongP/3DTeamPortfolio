#include "..\Public\Terrain.h"
#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CTerrain::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
#ifdef _DEBUG
		//m_pRenderer->Add_DebugGroup(m_pPlane);
#endif // _DEBUG
	}
}

HRESULT CTerrain::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin("Terrain");

	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CTerrain Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Terrain"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CTerrain Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* Com_Buffer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer))))
	{
		MSG_BOX("Failed CTerrain Add_Component : (Com_Buffer)");
		return E_FAIL;
	}
	
	/* Com_Plane */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Plane"),
		TEXT("Com_Plane"), reinterpret_cast<CComponent**>(&m_pPlane))))
	{
		MSG_BOX("Failed CTerrain Add_Component : (Com_Buffer)");
		return E_FAIL;
	}

	/* Com_Texture */
	/*if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTexture))))
	{
		MSG_BOX("Failed CTerrain Add_Component : (Com_Texture)");
		return E_FAIL;
	}*/

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = New CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = New CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pPlane);
	Safe_Release(m_pTexture);
	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pRenderer);
}
