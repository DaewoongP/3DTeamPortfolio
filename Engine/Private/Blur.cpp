#include "..\Public\Blur.h"
#include "RenderTarget_Manager.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CBlur::CBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CBlur::Initialize(CVIBuffer_Rect* pRectBuffer)
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	_uint				iNumViews = { 1 };
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	m_vWinSize.x = ViewportDesc.Width;
	m_vWinSize.y = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	Safe_Release(pRenderTarget_Manager);

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Blur.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pBuffer = pRectBuffer;
	Safe_AddRef(m_pBuffer);
	
	return S_OK;
}

HRESULT CBlur::Render(const _tchar* pMRTTag, const _tchar* pTargetTag, BLUR eBlurType, _uint iSampleCnt)
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, pMRTTag)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(pTargetTag, m_pShader, "g_TargetTexture")))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fWinSizeX", &m_vWinSize.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fWinSizeY", &m_vWinSize.y, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_iSampleCnt", &iSampleCnt, sizeof(_uint))))
		return E_FAIL;

	if (BLUR_X == eBlurType)
	{
		if (FAILED(m_pShader->Begin("BlurX")))
			return E_FAIL;
		if (FAILED(m_pBuffer->Render()))
			return E_FAIL;
	}
	else if (BLUR_Y == eBlurType)
	{
		if (FAILED(m_pShader->Begin("BlurY")))
			return E_FAIL;
		if (FAILED(m_pBuffer->Render()))
			return E_FAIL;
	}

	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext, pMRTTag)))
		return E_FAIL;

	Safe_Release(pRenderTarget_Manager);

	return S_OK;
}

CBlur* CBlur::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CVIBuffer_Rect* pRectBuffer)
{
	CBlur* pInstance = New CBlur(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pRectBuffer)))
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
