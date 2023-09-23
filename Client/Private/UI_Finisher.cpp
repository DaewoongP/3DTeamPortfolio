#include "UI_Finisher.h"
#include "GameInstance.h"

#include "UI_Progress1.h"

CUI_Finisher::CUI_Finisher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Finisher::CUI_Finisher(const CUI_Finisher& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Finisher::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Make_Matrix(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CUI_Finisher::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Finisher::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CUI_Finisher::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin("Finisher_Progress")))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_Finisher::Set_Gauge(_float fGauge, CUI_Progress::GAUGE eType)
{
	if (nullptr != m_pProgressCom)
	{
		m_pProgressCom->Set_Gauge(fGauge, eType);
	}
}


HRESULT CUI_Finisher::Add_Components()
{
	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CUI_Finisher Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CUI_Finisher Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* Com_Buffer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		MSG_BOX("Failed CUI_Finisher Add_Component : (Com_Buffer)");
		return E_FAIL;
	}

	_float3 vProgress = _float3(100.f, 100.f, 100.f);
	/* Com_Progress */
	if (FAILED(CComposite::Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_Component_UI_Progress"),
		TEXT("Com_Progress"), reinterpret_cast<CComponent**>(&m_pProgressCom), &vProgress)))
	{
		MSG_BOX("Failed CUI_Finisher Add_Component : (Com_Progress)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Finisher::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float fPercent = m_pProgressCom->Get_Gauge_Percent();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fPercent", &fPercent, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_Textures[m_iTextureIndex]->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;


	return S_OK;
}

CUI_Finisher* CUI_Finisher::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Finisher* pInstance = New CUI_Finisher(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Finisher");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Finisher::Clone(void* pArg)
{
	CUI_Finisher* pInstance = New CUI_Finisher(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Finisher");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Finisher::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pProgressCom);
}
