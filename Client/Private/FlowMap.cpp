#include "../Public/FlowMap.h"
#include "GameInstance.h"

CFlowMap::CFlowMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CFlowMap::CFlowMap(const CFlowMap& rhs)
    : CGameObject(rhs)
{
}

HRESULT CFlowMap::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Scale(_float3(100.f, 100.f, 100.f));
	m_pTransform->Rotation(_float3(XMConvertToRadians(90.f), 0.f, 0.f));
	m_pTransform->Set_Position(_float3(50.f, 0.f, 50.f));

    return S_OK;
}

void CFlowMap::Tick(_float fTimeDelta)
{
	m_fFrame += fTimeDelta;
}

void CFlowMap::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}
}

HRESULT CFlowMap::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;
	_float fPower = 1.3f;
	if (FAILED(m_pShader->Bind_RawValue("g_fFlowPower", &fPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_FrameTime", &m_fFrame, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Flow")))
		return E_FAIL;

	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

    return S_OK;
}

HRESULT CFlowMap::Add_Components()
{
	/* For. Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("CFlowMap Failed Clone Component : Com_Renderer");
		return E_FAIL;
	}

	/* For. Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_FlowMap"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("CFlowMap Failed Clone Component : Com_Shader");
		return E_FAIL;
	}
	
	/* For. Com_Buffer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer))))
	{
		MSG_BOX("CFlowMap Failed Clone Component : Com_Buffer");
		return E_FAIL;
	}

	m_pTexture1 = CTexture::Create(m_pDevice, m_pContext, (TEXT("../../Resources/effects/Textures/VFX_T_Wavy_N.png")));
	m_pTexture2 = CTexture::Create(m_pDevice, m_pContext, (TEXT("../../Resources/effects/Textures/Liquid/VFX_T_WaterTurbulentTile_01_D.png")));

	return S_OK;
}

HRESULT CFlowMap::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTexture1->Bind_ShaderResource(m_pShader, "g_FlowTexture1")))
		return E_FAIL;
	
	if (FAILED(m_pTexture2->Bind_ShaderResource(m_pShader, "g_FlowTexture2")))
		return E_FAIL;

	Safe_Release(pGameInstance);

    return S_OK;
}

CFlowMap* CFlowMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFlowMap* pInstance = New CFlowMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CFlowMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFlowMap::Clone(void* pArg)
{
	CFlowMap* pInstance = New CFlowMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CFlowMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlowMap::Free()
{
    __super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pTexture1);
	Safe_Release(m_pTexture2);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pBuffer);
}
