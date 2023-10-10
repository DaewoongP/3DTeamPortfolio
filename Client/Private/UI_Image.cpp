#include "UI_Image.h"
#include "GameInstance.h"

CUI_Image::CUI_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}
CUI_Image::CUI_Image(const CUI_Image& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Image::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CUI_Image::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		IMAGEDESC* pDesc = (IMAGEDESC*)pArg;

		m_vCombinedXY = pDesc->vCombinedXY;
		m_fX = pDesc->fX;
		m_fY = pDesc->fY;
		m_fZ = pDesc->fZ;
		m_fSizeX = pDesc->fSizeX;
		m_fSizeY = pDesc->fSizeY;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 100.f));

	return S_OK;
}

void CUI_Image::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pParent == nullptr)
	{
		m_vCombinedXY.x = m_fX;
		m_vCombinedXY.y = m_fY;
	}
	else
	{
		_float2 vParentPos = m_pParent->Get_XY();

		m_vCombinedXY.x = vParentPos.x + m_fX;
		m_vCombinedXY.y = vParentPos.y + m_fY;
	}
}

void CUI_Image::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	Change_Scale(m_fSizeX , m_fSizeY);
	Change_Position(m_vCombinedXY.x, m_vCombinedXY.y);

	if (nullptr != m_pRendererCom && nullptr != m_pTextureCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CUI_Image::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	switch (m_eShadertype)
	{
	case Client::CUI_Image::MINIMAP:
		m_pShaderCom->Begin("MiniMap");
		break;
	case Client::CUI_Image::SKILL:
		m_pShaderCom->Begin("GrayCool");
		break;
	case Client::CUI_Image::SHADERTYPE_END:
		m_pShaderCom->Begin("UI");
		break;
	default:
		break;
	}

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Image::Add_Components()
{
	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CUI_Image Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CUI_Image Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Buffer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		MSG_BOX("Failed CUI_Image Add_Component : (Com_Buffer)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Image::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (nullptr != m_pTextureCom)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			return E_FAIL;
	}

	if (m_eShadertype == MINIMAP)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vPlayerPos", &vPlayerPos, sizeof(_float3))))
			return E_FAIL;		
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vMiniMapSize", &vMiniMapSize, sizeof(_float2))))
			return E_FAIL;
	}

	if (m_eShadertype == SKILL && m_isCool)
	{
		_float fCool = *m_pCoolTime;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vPlayerPos", &m_pCoolTime, sizeof(_float))))
			return E_FAIL;

	}


	return S_OK;
}

HRESULT CUI_Image::Set_Texture(CTexture* pTexture)
{
	Safe_Release(m_pTextureCom);

	m_pTextureCom = pTexture;
	Safe_AddRef(m_pTextureCom);

	return S_OK;
}

HRESULT CUI_Image::Set_Desc(IMAGEDESC desc)
{
	m_vCombinedXY = desc.vCombinedXY;
	m_fX = desc.fX;
	m_fY = desc.fY;
	m_fZ = desc.fZ;
	m_fSizeX = desc.fSizeX;
	m_fSizeY = desc.fSizeY;
	m_eShadertype = desc.eShadertype;

	return S_OK;
}

HRESULT CUI_Image::Change_Position(_float fX, _float fY)
{
	m_pTransform->Set_Position(
		XMVectorSet(m_vCombinedXY.x - g_iWinSizeX * 0.5f, -m_vCombinedXY.y + g_iWinSizeY * 0.5f, m_fZ, 1.f));

	return S_OK;
}

HRESULT CUI_Image::Change_Scale(_float fX, _float fY)
{
	//	m_fSizeX = fX;
	//	m_fSizeY = fY;

	m_pTransform->Set_Scale(_float3(fX, fY, 1.f));

	return S_OK;
}


CUI_Image* CUI_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Image* pInstance = New CUI_Image(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Image");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CUI_Image* CUI_Image::Clone(void* pArg)
{
	CUI_Image* pInstance = New CUI_Image(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Image");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Image::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
