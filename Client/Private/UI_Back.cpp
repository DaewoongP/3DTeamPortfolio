#include "UI_Back.h"
#include "GameInstance.h"

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
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Back::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	Make_Matrix(g_iWinSizeX, g_iWinSizeY);

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
	
	if (m_isRemoveBlack)
	{
		if (FAILED(m_pShaderCom->Begin("UI_Remove_Black")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Begin("UI")))
			return E_FAIL;
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

HRESULT CUI_Back::SetUp_ShaderResources()
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

void CUI_Back::Set_Rotation(_float3 vAxis, _float fRadian)
{
	m_pTransform->Rotation(vAxis, fRadian);
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
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
