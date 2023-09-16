#include "..\Public\Blur.h"
#include "RenderTarget_Manager.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CBlur::CBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CBlur::Initialize_Prototype(const _tchar* pTargetTag, BLUR eBlurOption)
{
	lstrcpy(m_szTargetTag, pTargetTag);
	m_eBlurOption = eBlurOption;

	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	_uint				iNumViews = { 1 };
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Blur"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_Blur"), TEXT("Target_Blur"))))
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

HRESULT CBlur::Render()
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Blur"))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(m_szTargetTag, m_pShader, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (BLUR_X == m_eBlurOption)
	{
		if (FAILED(m_pShader->Begin("BlurX")))
			return E_FAIL;
		if (FAILED(m_pBuffer->Render()))
			return E_FAIL;
	}
	else if (BLUR_Y == m_eBlurOption)
	{
		if (FAILED(m_pShader->Begin("BlurY")))
			return E_FAIL;
		if (FAILED(m_pBuffer->Render()))
			return E_FAIL;
	}
	else
	{
		MSG_BOX("Blur Option is Null");
	}

	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	Safe_Release(pRenderTarget_Manager);

	return S_OK;
}

HRESULT CBlur::Add_Components()
{
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Blur.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pBuffer)
		return E_FAIL;

	return S_OK;
}

CBlur* CBlur::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
	const _tchar* pTargetTag, BLUR eBlurOption)
{
	CBlur* pInstance = New CBlur(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTargetTag, eBlurOption)))
	{
		MSG_BOX("Failed to Created CBlur");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlur::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
}
