#include "UI_Progress.h"
#include "GameInstance.h"
#include "UI_Group.h"

CUI_Progress::CUI_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Progress::CUI_Progress(const CUI_Progress& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Progress::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Progress::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		CUI_Group::UI_DATA* UIDesc = (CUI_Group::UI_DATA*)pArg;

		if (UIDesc->isSave)
		{
			m_vCombinedXY = UIDesc->vCombinedXY;
			m_fX = UIDesc->m_fX;
			m_fY = UIDesc->m_fY;
			m_fZ = UIDesc->m_fZ;
			m_fSizeX = UIDesc->m_fSizeX;
			m_fSizeY = UIDesc->m_fSizeY;

			lstrcpy(m_wszTextureName, UIDesc->m_wszTextureName);
			lstrcpy(m_wszTexturePath, UIDesc->m_wszTexturePath);

			//m_eUIType = UIDesc->m_eType;

			m_isParent = UIDesc->m_isParent;

			m_isAlpha = UIDesc->m_isAlpha;
			m_vColor = UIDesc->m_vColor;
			lstrcpy(m_wszAlphaTexturePrototypeTag, UIDesc->m_wszAlphaTexturePrototypeTag);
			lstrcpy(m_wszAlphaTextureFilePath, UIDesc->m_wszAlphaTextureFilePath);
		}
		else
		{
			_float2 fSize = _float2(UIDesc->m_fSizeX, UIDesc->m_fSizeY);
			Set_Size(fSize.x, fSize.y);

			lstrcpy(m_wszTextureName, UIDesc->m_wszTextureName);
			lstrcpy(m_wszTexturePath, UIDesc->m_wszTexturePath);
		}
	}
	else
	{
		return E_FAIL;
	}

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

void CUI_Progress::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Progress::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Progress::Render()
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

HRESULT CUI_Progress::Add_Components()
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

HRESULT CUI_Progress::Add_AlphaTexture()
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

HRESULT CUI_Progress::SetUp_ShaderResources()
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

CUI_Progress* CUI_Progress::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Progress* pInstance = new CUI_Progress(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Progress");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Progress::Clone(void* pArg)
{
	CUI_Progress* pInstance = new CUI_Progress(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Progress");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Progress::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pAlphaTextureCom);
}
