#include "..\Public\DummyParticle.h"
#include "GameInstance.h"
CDummyParticle::CDummyParticle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CDummyParticle::CDummyParticle(const CDummyParticle& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CDummyParticle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	// 툴이라 여기에다 해놓음
	if (FAILED(Add_Components()))
		return E_FAIL;
	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));

	return S_OK;
}

HRESULT CDummyParticle::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CDummyParticle::Tick(_float _fTimeDelta)
{
	m_pParticleSystem->Tick(_fTimeDelta, m_pBuffer);
}

void CDummyParticle::Late_Tick(_float _fTimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CDummyParticle::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin("Default");

	m_pBuffer->Render();

	return S_OK;
}

HRESULT CDummyParticle::Add_Components()
{
	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxRectColInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CDummyParticle Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* Com_Buffer */
	_int iNumInstance = 30;
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Rect_Color_Instance"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer), &iNumInstance)))
	{
		MSG_BOX("Failed CDummyParticle Add_Component : (Com_Buffer)");
		return E_FAIL;
	}

	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer")
		, TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
	{
		MSG_BOX("Failed Renderer Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* Com_Particle */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Default_ParticleSystem")
		, TEXT("Com_Particle"), (CComponent**)&m_pParticleSystem)))
	{
		MSG_BOX("Failed CDummyParticle Add_Component : (Com_Particle)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDummyParticle::SetUp_ShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pParticleSystem->Bind_ParticleValue(m_pShader)))
		return E_FAIL;

	ENDINSTANCE;
	return S_OK;
}

CDummyParticle* CDummyParticle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDummyParticle* pInstance = New CDummyParticle(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Faild to Created CDummyParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDummyParticle::Clone(void* _pArg)
{
	CDummyParticle* pInstance = New CDummyParticle(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Faild to Created CDummyParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDummyParticle::Free(void)
{
	__super::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pParticleSystem);
	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
}