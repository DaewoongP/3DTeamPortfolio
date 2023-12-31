#include "RectDebug.h"
#include "GameInstance.h"

CRectDebug::CRectDebug(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CRectDebug::CRectDebug(const CRectDebug& rhs)
	: CGameObject(rhs)
	, m_isRendering(rhs.m_isRendering)
{
}

HRESULT CRectDebug::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRectDebug::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_isRendering = true;

	return S_OK;
}

void CRectDebug::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	BEGININSTANCE;
	if (pGameInstance->Get_DIKeyState(DIK_L))
	{
		m_isRendering = !m_isRendering;
	}
	ENDINSTANCE;
}

void CRectDebug::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CRectDebug::Render()
{
	if (!m_isRendering)
		return S_OK;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin("Terrain");

	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

void CRectDebug::Set_Scale(_int2 value)
{
	m_pTransform->Set_Scale(_float3(value.x, 1, value.y));
}

HRESULT CRectDebug::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CRectDebug Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Terrain"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CRectDebug Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* Com_Buffer */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer))))
	{
		MSG_BOX("Failed CRectDebug Add_Component : (Com_Buffer)");
		return E_FAIL;
	}

	/* Com_Texture */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTexture))))
	{
		MSG_BOX("Failed CRectDebug Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRectDebug::SetUp_ShaderResources()
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

	if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "g_BrushTexture", 1)))
		return E_FAIL;

	return S_OK;
}

CRectDebug* CRectDebug::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRectDebug* pInstance = New CRectDebug(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRectDebug");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRectDebug::Clone(void* pArg)
{
	CRectDebug* pInstance = New CRectDebug(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CRectDebug");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRectDebug::Free()
{
	__super::Free();

	Safe_Release(m_pTexture);
	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pRenderer);
}
