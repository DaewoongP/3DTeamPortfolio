#include "UI_HP.h"
#include "GameInstance.h"

CUI_HP::CUI_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_HP::CUI_HP(const CUI_HP& rhs)
	: CUI(rhs)
{
}

void CUI_HP::Set_HP(_float fGauge)
{
	m_fPercent = fGauge;
}

void CUI_HP::Set_HP(_float fGauge, CUI_Progress::GAUGE eType)
{
	m_pProgressCom->Set_Gauge(fGauge, eType);
}

HRESULT CUI_HP::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Make_Matrix(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CUI_HP::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_HP::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CUI_HP::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	if (m_eHPType == PLAYER)
	{
		if (FAILED(m_pShaderCom->Begin("HP_Progress")))
			return E_FAIL;
	}
	else if(m_eHPType == MONSTER)
	{
		if (FAILED(m_pShaderCom->Begin("HP_ProgressRED")))
			return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_HP::Add_Components()
{
	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CUI_HP Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CUI_HP Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Buffer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		MSG_BOX("Failed CUI_HP Add_Component : (Com_Buffer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Progress */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_UI_Progress"),
		TEXT("Com_Progress"), reinterpret_cast<CComponent**>(&m_pProgressCom))))
	{
		MSG_BOX("Failed CUI_HP Add_Component : (Com_Progress)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_HP::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_Textures[m_iTextureIndex]->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fPercent", &m_fPercent, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_HP::Initialize_Gauge(_float fMin, _float fMax, _float fCurrent)
{
	if (nullptr == m_pProgressCom)
	{
		__debugbreak();
		return E_FAIL;
	}

	m_pProgressCom->Initailize_Gauge(fMin, fMax, fCurrent);	

	return S_OK;
}

CUI_HP* CUI_HP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_HP* pInstance = New CUI_HP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_HP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_HP::Clone(void* pArg)
{
	CUI_HP* pInstance = New CUI_HP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_HP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_HP::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pProgressCom);
}
