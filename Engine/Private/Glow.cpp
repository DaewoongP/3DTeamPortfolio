#include "Glow.h"
#include "RenderTarget_Manager.h"
#include "GameObject.h"
#include "Blur.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CGlow::CGlow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :CComponent(pDevice,pContext)
{
}

HRESULT CGlow::Initialize(CVIBuffer_Rect* pRectBuffer)
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	_uint				iNumViews = { 1 };
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Glow"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Glow_BlurX"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Glow_Blured"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Glowed"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_Glow"), TEXT("Target_Glow"))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_Glow_BlurX"), TEXT("Target_Glow_BlurX"))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_Glow_Blured"), TEXT("Target_Glow_Blured"))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_Glowed"), TEXT("Target_Glowed"))))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	Safe_Release(pRenderTarget_Manager);

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Glow.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pBlur = CBlur::Create(m_pDevice, m_pContext, pRectBuffer);
	if (nullptr == m_pBlur)
		return E_FAIL;

	m_pBuffer = pRectBuffer;
	Safe_AddRef(m_pBuffer);
	
	return S_OK;
}

HRESULT CGlow::Render(list<CGameObject*>& GlowRenderObjects, _float fGlowPower)
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Glow"))))
		return E_FAIL;

	// 글로우 먹일 객체 텍스처 저장
	for (auto& pGameObject : GlowRenderObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	GlowRenderObjects.clear();

	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Glow"))))
		return E_FAIL;

	// 블러처리
	if (FAILED(m_pBlur->Render(TEXT("MRT_Glow_BlurX"), TEXT("Target_Glow"), CBlur::BLUR_X)))
		return E_FAIL;
	if (FAILED(m_pBlur->Render(TEXT("MRT_Glow_Blured"), TEXT("Target_Glow_BlurX"), CBlur::BLUR_Y)))
		return E_FAIL;

	// 글로우 적용한 텍스처 저장
	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Glowed"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fGlowPower", &fGlowPower, sizeof(_float))))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Glow"), m_pShader, "g_TargetTexture")))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Glow_Blured"), m_pShader, "g_TargetBluredTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Glow")))
		return E_FAIL;
	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Glowed"))))
		return E_FAIL;

	Safe_Release(pRenderTarget_Manager);

	return S_OK;
}

CGlow* CGlow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CVIBuffer_Rect* pRectBuffer)
{
	CGlow* pInstance = New CGlow(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pRectBuffer)))
	{
		MSG_BOX("Failed to Created CGlow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGlow::Free()
{
	__super::Free();

	Safe_Release(m_pBlur);
	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
}