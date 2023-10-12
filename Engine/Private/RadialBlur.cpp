#include "..\Public\RadialBlur.h"
#include "GameInstance.h"

CRadialBlur::CRadialBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
	m_pBuffer = CVIBuffer_Point_Instance::Create(pDevice, pContext);
}

CRadialBlur::CRadialBlur(const CRadialBlur& rhs)
	: CGameObject(rhs)
{
	_uint iNum = 1;
	m_pBuffer = static_cast<CVIBuffer_Point_Instance*>(rhs.m_pBuffer->Clone(&iNum));
}

HRESULT CRadialBlur::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	FAILED_CHECK(Add_Components());

	return S_OK;
}

void CRadialBlur::Late_Tick(_float fTimeDelta)
{
	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_RADIALBLUR, this);
}

HRESULT CRadialBlur::Render()
{
	FAILED_CHECK(SetUp_ShaderResources());

	FAILED_CHECK(m_pShader->Begin("Point"));

	FAILED_CHECK(m_pBuffer->Render());

	return S_OK;
}

HRESULT CRadialBlur::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CRadialBlur Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"),
		VTXPOINTINSTANCE_DECL::Elements, VTXPOINTINSTANCE_DECL::iNumElements);

	m_pTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Default_Normal.png"));

	return S_OK;
}

HRESULT CRadialBlur::SetUp_ShaderResources()
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

CRadialBlur* CRadialBlur::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRadialBlur* pInstance = New CRadialBlur(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRadialBlur");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRadialBlur::Clone(void* pArg)
{
	CRadialBlur* pInstance = New CRadialBlur(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CRadialBlur");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRadialBlur::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pTexture);
}
