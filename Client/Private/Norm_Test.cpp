#include "..\Public\Norm_Test.h"
#include "GameInstance.h"

CNorm_Test::CNorm_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CNorm_Test::CNorm_Test(const CNorm_Test& rhs)
	: CGameObject(rhs)
{
}

HRESULT CNorm_Test::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Position(_float3(25.f, 3.f, 22.5f));

	return S_OK;
}

void CNorm_Test::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CNorm_Test::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DISTORTION, this);
	}
}

HRESULT CNorm_Test::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Point")))
		return E_FAIL;

	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNorm_Test::Add_Components()
{
	/* For. Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("CNorm_Test Failed Clone Component : Com_Renderer");
		return E_FAIL;
	}

	/* For. Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("CNorm_Test Failed Clone Component : Com_Shader");
		return E_FAIL;
	}

	_uint iNum = 1;
	/* For. Com_Buffer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer), &iNum)))
	{
		MSG_BOX("CNorm_Test Failed Clone Component : Com_Buffer");
		return E_FAIL;
	}

	m_pTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/T_FX_Distortion_N.png"));

	return S_OK;
}

HRESULT CNorm_Test::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	
	if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CNorm_Test* CNorm_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNorm_Test* pInstance = New CNorm_Test(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CNorm_Test");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNorm_Test::Clone(void* pArg)
{
	CNorm_Test* pInstance = New CNorm_Test(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CNorm_Test");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNorm_Test::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pBuffer);
}
