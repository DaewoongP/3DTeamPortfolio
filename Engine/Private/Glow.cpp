#include "Glow.h"
#include "RenderTarget_Manager.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include"Texture.h"

CGlow::CGlow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :CComponent(pDevice,pContext)
{
}

HRESULT CGlow::Initialize_Prototype(const _tchar* pTargetTag)
{
	lstrcpy(m_szTargetTag, pTargetTag);



	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);


	m_pTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/glowmap.dds"));
	m_pTexture2 = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/test.dds"));

	_uint				iNumViews = { 1 };
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Glow"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_Glow"), TEXT("Target_Glow"))))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	Safe_Release(pRenderTarget_Manager);

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlow::Render()
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);
	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Glow"))))
		return E_FAIL;

	
	m_pTexture->Bind_ShaderResource(m_pShader, "g_AlphaTexture");
	m_pTexture2->Bind_ShaderResource(m_pShader, "g_GlowTexture");


	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	/*if (FAILED(m_pShader->Render()))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Blur_Bloom"))))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Bloom"), m_pShader, "g_DoBlurTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("BlurX")))
		return E_FAIL;
	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_WhiteBloom"))))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Blur_Bloom"), m_pShader, "g_DoBlurTexture")))
		return E_FAIL;
	if (FAILED(m_pShader->Begin("BlurY")))
		return E_FAIL;*/




	if (FAILED(m_pShader->Bind_RawValue("g_fGlowPower", &m_fGlowPower, sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin("Lightning");

	m_pBuffer->Render();
	
	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;
	
	Safe_Release(pRenderTarget_Manager);


	return S_OK;
}

HRESULT CGlow::Add_Components()
{
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Glow.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pBuffer)
		return E_FAIL;

	return S_OK;
}

CGlow* CGlow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag)
{
	CGlow* pInstance = New CGlow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTargetTag)))
	{
		MSG_BOX("Failed to Created CGlow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGlow::Free()
{
	__super::Free();
	Safe_Release(m_pTexture);
	Safe_Release(m_pTexture2);


	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
}

