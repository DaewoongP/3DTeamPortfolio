#include "DOF.h"
#include "RenderTarget_Manager.h"
#include "Shader.h"
#include "Blur.h"
#include "VIBuffer_Rect.h"
#include "PipeLine.h"

CDOF::CDOF(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent(pDevice, pContext)
{
}

void CDOF::Default()
{
	m_fFocusDistance = 25.f;
	m_fFocusRange = m_fFocusDistance - 1.f;
	m_isDof = true;
}

HRESULT CDOF::Initialize(CVIBuffer_Rect* pRectBuffer)
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	_uint				iNumViews = { 1 };
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_DOF"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_DOF_BlurX"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_DOF_Blured"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_DOF"), TEXT("Target_DOF"))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_DOF_BlurX"), TEXT("Target_DOF_BlurX"))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_DOF_Blured"), TEXT("Target_DOF_Blured"))))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_pBuffer = pRectBuffer;
	Safe_AddRef(m_pBuffer);

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_DOF.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pBlur = CBlur::Create(m_pDevice, m_pContext, pRectBuffer);
	if (nullptr == m_pBlur)
		return E_FAIL;

	Safe_Release(pRenderTarget_Manager);

	m_fFocusDistance = 25.f;
	m_fFocusRange = m_fFocusDistance - 1.f;

	return S_OK;
}

HRESULT CDOF::Render(const _tchar* pTargetTag)
{
	if (false == m_isDof)
		return S_OK;

	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_DOF"))))
		return E_FAIL;
	
	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(pTargetTag, m_pShader, "g_TargetTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pPipeLine->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	if (FAILED(m_pShader->Bind_RawValue("g_fFocusDistance", &m_fFocusDistance, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fFocusRange", &m_fFocusRange, sizeof(_float))))
		return E_FAIL;
	
	if (FAILED(m_pShader->Begin("DOF")))
		return E_FAIL;

	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_DOF"))))
		return E_FAIL;

	if (FAILED(m_pBlur->Render(TEXT("MRT_DOF_BlurX"), TEXT("Target_DOF"), CBlur::BLUR_X)))
		return E_FAIL;
	if (FAILED(m_pBlur->Render(TEXT("MRT_DOF_Blured"), TEXT("Target_DOF_BlurX"), CBlur::BLUR_Y)))
		return E_FAIL;

	Safe_Release(pRenderTarget_Manager);

	return S_OK;
}

CDOF* CDOF::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CVIBuffer_Rect* pRectBuffer)
{
	CDOF* pInstance = New CDOF(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pRectBuffer)))
	{
		MSG_BOX("Failed to Created CDOF");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDOF::Free()
{
	__super::Free();

	Safe_Release(m_pBlur);
	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
}

