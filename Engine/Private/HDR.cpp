#include "..\Public\HDR.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "RenderTarget_Manager.h"

CHDR::CHDR(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CHDR::Initialize(CVIBuffer_Rect* pRectBuffer)
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	_uint				iNumViews = { 1 };
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_HDR"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_HDR"))))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	Safe_Release(pRenderTarget_Manager);

	m_pBuffer = pRectBuffer;
	Safe_AddRef(m_pBuffer);

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Shadow.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	NULL_CHECK_RETURN(m_pShader, E_FAIL);

	return S_OK;
}

HRESULT CHDR::Render()
{
	return S_OK;
}

CHDR* CHDR::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CVIBuffer_Rect* pRectBuffer)
{
	return nullptr;
}

void CHDR::Free()
{
}
