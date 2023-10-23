#include "UI_Dissolve.h"
#include "GameInstance.h"

CUI_Dissolve::CUI_Dissolve(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Dissolve::CUI_Dissolve(const CUI_Dissolve& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Dissolve::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Dissolve::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Make_Matrix(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CUI_Dissolve::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_isPlayDissolve)
		m_fDissolveAmount += fTimeDelta / 5.f;

}

void CUI_Dissolve::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CUI_Dissolve::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin("Dissolve")))
		return E_FAIL;
	
	if (m_AlphaTextures.size() > 0)
	{
		if (FAILED(m_pShaderCom->Begin("Alpha")))
			return E_FAIL;
	}


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Dissolve::Add_Components()
{
	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CLogo Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CLogo Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Buffer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		MSG_BOX("Failed CLogo Add_Component : (Com_Buffer)");
		__debugbreak();
		return E_FAIL;
	}

	m_pDissolveTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/VFX/Textures/Noises/VFX_T_NoiseGreypack02_D.png"));

	return S_OK;
}

HRESULT CUI_Dissolve::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_Textures[m_iTextureIndex]->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (nullptr != m_pDissolveTexture)
	{
		if (FAILED(m_pDissolveTexture->Bind_ShaderResources(m_pShaderCom, "g_DissolveTexture")))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
			return E_FAIL;
	}

	if (m_AlphaTextures.size() > 0)
	{
		if (FAILED(m_AlphaTextures[m_iTextureIndex]->Bind_ShaderResources(m_pShaderCom, "g_AlphaTexture")))
			return E_FAIL;
	}

	return S_OK;
}

CUI_Dissolve* CUI_Dissolve::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Dissolve* pInstance = New CUI_Dissolve(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Dissolve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Dissolve::Clone(void* pArg)
{
	CUI_Dissolve* pInstance = New CUI_Dissolve(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Dissolve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Dissolve::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pDissolveTexture);
}