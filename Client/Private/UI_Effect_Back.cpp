#include "UI_Effect_Back.h"
#include "GameInstance.h"

CUI_Effect_Back::CUI_Effect_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Effect_Back::CUI_Effect_Back(const CUI_Effect_Back& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Effect_Back::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Make_Matrix(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CUI_Effect_Back::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CUI_Effect_Back::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CUI_Effect_Back::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin("UI")))
		return E_FAIL;	

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Effect_Back::Add_Components()
{
	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CUI_Effect_Back Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CUI_Effect_Back Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* Com_Buffer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		MSG_BOX("Failed CUI_Effect_Back Add_Component : (Com_Buffer)");
		return E_FAIL;
	}

	CUI_Image::IMAGEDESC pDesc;
	pDesc.vCombinedXY = m_vCombinedXY;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fZ = m_fZ;
	pDesc.fSizeX = m_fSizeX;
	pDesc.fSizeY = m_fSizeY;
	/* Com_Image */
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_UI_Image"),
		TEXT("Com_Image"), reinterpret_cast<CComponent**>(&m_pImageCom), &pDesc)))
	{
		MSG_BOX("Failed CUI_Effect_Back Add_Component : (Com_Image)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Effect_Back::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_Textures[m_iTextureIndex]->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

void CUI_Effect_Back::Set_Texture(CTexture* pTexture)
{
	m_pImageCom->Set_Texture(pTexture);
}

void CUI_Effect_Back::Set_ImageCom(CUI_Image::IMAGEDESC desc)
{
	m_pImageCom->Set_Desc(desc);
}

void CUI_Effect_Back::Set_Rotation(_float3 vAxis, _float fRadian)
{
	m_pTransform->Rotation(vAxis, fRadian);
}


CUI_Effect_Back* CUI_Effect_Back::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Effect_Back* pInstance = new CUI_Effect_Back(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Progress");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Effect_Back::Clone(void* pArg)
{
	CUI_Effect_Back* pInstance = new CUI_Effect_Back(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Progress");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Effect_Back::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pImageCom);
}