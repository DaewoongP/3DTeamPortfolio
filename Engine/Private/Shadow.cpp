#include "..\Public\Shadow.h"
#include "Blur.h"
#include "Shader.h"
#include "PipeLine.h"
#include "VIBuffer_Rect.h"
#include "RenderTarget_Manager.h"

CShadow::CShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CShadow::Initialize(CVIBuffer_Rect* pRectBuffer)
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	_uint				iNumViews = { 1 };
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Shadow"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	/*if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Shadow_DownSample"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;*/

	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_Shadow"))))
		return E_FAIL;
	/*if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_Shadow_DownSample"), TEXT("Target_Shadow_DownSample"))))
		return E_FAIL;*/

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_vWinSize.x = m_WorldMatrix._11 = ViewportDesc.Width;
	m_vWinSize.y = m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	Safe_Release(pRenderTarget_Manager);

	m_pBuffer = pRectBuffer;
	Safe_AddRef(m_pBuffer);

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Shadow.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	NULL_CHECK_RETURN(m_pShader, E_FAIL);

	m_pBlur = CBlur::Create(m_pDevice, m_pContext, pRectBuffer);
	NULL_CHECK_RETURN(m_pBlur, E_FAIL);

	return S_OK;
}

HRESULT CShadow::Render()
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	if (nullptr == pRenderTarget_Manager)
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Shadow"))))
		return E_FAIL;
	// ½ÇÁ¦ °´Ã¼ µª½º
	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;
	// ºûÀÇ ºäÇà·Ä ±âÁØ µª½º
	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shadow_Depth"), m_pShader, "g_LightDepthTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", pPipeLine->Get_LightTransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", pPipeLine->Get_LightTransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pPipeLine->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Shadow")))
		return E_FAIL;

	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Shadow"))))
		return E_FAIL;

	// Down Sampling
	/*if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Shadow_DownSample"))))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shadow"), m_pShader, "g_ShadowTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fWinSizeX", &m_vWinSize.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fWinSizeY", &m_vWinSize.y, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("DownSampling")))
		return E_FAIL;

	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Shadow_DownSample"))))
		return E_FAIL;*/

	Safe_Release(pPipeLine);
	Safe_Release(pRenderTarget_Manager);

	return S_OK;
}

CShadow* CShadow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CVIBuffer_Rect* pRectBuffer)
{
	CShadow* pInstance = New CShadow(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pRectBuffer)))
	{
		MSG_BOX("Failed to Created CShadow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShadow::Free()
{
	__super::Free();

	Safe_Release(m_pBlur);
	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
}
