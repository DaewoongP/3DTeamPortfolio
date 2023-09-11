#include "UI_Back.h"
#include "GameInstance.h"
#include "UI_Group.h"

CUI_Back::CUI_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Back::CUI_Back(const CUI_Back& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Back::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype(g_iWinSizeX, g_iWinSizeY)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Back::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	Ready_Texture();

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (m_isAlpha)
	{
		Add_AlphaTexture();
	}

	m_pTransform->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransform->Set_Position(_float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_fZ));

	return S_OK;

}

void CUI_Back::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Back::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CUI_Back::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_isAlpha && nullptr != m_pAlphaTextureCom)
	{
		m_pShaderCom->Begin("UIAlpha");
	}
	else
	{
		m_pShaderCom->Begin("UI");
	}

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Back::Add_Components()
{	
	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CDummy_UI Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* Com_Texture */
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, m_wszTextureName,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed CDummy_UI Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CDummy_UI Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* Com_Buffer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		MSG_BOX("Failed CDummy_UI Add_Component : (Com_Buffer)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Back::Add_AlphaTexture()
{
	BEGININSTANCE

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, m_wszAlphaTexturePrototypeTag,
			CTexture::Create(m_pDevice, m_pContext, m_wszAlphaTextureFilePath))))
		{
			MSG_BOX("Failed Create Texture Component");
		}

	ENDINSTANCE

		if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, m_wszAlphaTexturePrototypeTag,
			TEXT("Com_AlphaTexture"), reinterpret_cast<CComponent**>(&m_pAlphaTextureCom))))
		{
			MSG_BOX("Failed CDummy_UI Add_Component : (Com_Texture)");
			return E_FAIL;
		}

	return S_OK;
}

HRESULT CUI_Back::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (m_isAlpha && nullptr != m_pAlphaTextureCom)
	{
		if (FAILED(m_pAlphaTextureCom->Bind_ShaderResources(m_pShaderCom, "g_AlphaTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;

	}

	return S_OK;
}

HRESULT CUI_Back::Ready_Texture()
{
	BEGININSTANCE

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, m_wszTextureName,
			CTexture::Create(m_pDevice, m_pContext, m_wszTexturePath))))
		{
			MSG_BOX("Failed Create Texture Component");
		}


	if (m_isAlpha)
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, m_wszAlphaTexturePrototypeTag,
			CTexture::Create(m_pDevice, m_pContext, m_wszAlphaTextureFilePath))))
		{
			MSG_BOX("Failed Create Texture Component");
		}
	}

	ENDINSTANCE

		return S_OK;
}

CUI_Back* CUI_Back::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Back* pInstance = new CUI_Back(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Progress");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Back::Clone(void* pArg)
{
	CUI_Back* pInstance = new CUI_Back(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Progress");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Back::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pAlphaTextureCom);
}
