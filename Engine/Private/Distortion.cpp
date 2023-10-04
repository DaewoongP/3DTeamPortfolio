#include "Distortion.h"
#include "RenderTarget_Manager.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"

CDistortion::CDistortion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent(pDevice, pContext)
{
}

void CDistortion::Set_Textures(_tchar* pShaderPass, CTexture* pOriTexture, CTexture* pNoisetexture, CTexture* pAlphaTexture)
{
	lstrcpy(ShaderPass, pShaderPass);
	m_pOriginTexture = pOriTexture;
	m_pNoiseTexture = pNoisetexture;
	m_pAlphaTexture = pAlphaTexture;
}

HRESULT CDistortion::Initialize_Prototype(const _tchar* pTargetTag)
{

	lstrcpy(m_szTargetTag, pTargetTag);

	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	_uint				iNumViews = { 1 };



	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);


	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_MapEffect"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_MapEffect"), TEXT("Target_MapEffect"))))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	Safe_Release(pRenderTarget_Manager);

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pNoiseTexture = CTexture::Create(m_pDevice, m_pContext, (TEXT("../../Resources/effects/Textures/T_Repairo_Wisp_05_D.png")));


	return S_OK;
}

HRESULT CDistortion::Render()
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);
	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_MapEffect"))))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Distortion"), m_pShader, "g_OriTexture")))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Diffuse"), m_pShader, "g_AlphaTexture")))
		return E_FAIL;

	if (FAILED(m_pNoiseTexture->Bind_ShaderResource(m_pShader, "g_NoiseTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	m_fFrameTime += 0.01f;
	if (m_fFrameTime > 1000.f)
		m_fFrameTime = 0.f;

	if (FAILED(m_pShader->Bind_RawValue("g_FrameTime", &m_fFrameTime, sizeof(_float))))
		return E_FAIL;
	_float3 Speed = { 1.3f, 2.1f, 2.3f };
	if (FAILED(m_pShader->Bind_RawValue("g_ScrollSpeed", &Speed, sizeof(_float3))))
		return E_FAIL;
	_float3 Scale = { 1.f, 2.f, 3.f };
	if (FAILED(m_pShader->Bind_RawValue("g_Scales", &Scale, sizeof(_float3))))
		return E_FAIL;

	m_pShader->Begin("DistortionX");

	m_pBuffer->Render();
	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_MapEffect"))))
		return E_FAIL;


	Safe_Release(pRenderTarget_Manager);
	return S_OK;

}

HRESULT CDistortion::Add_Components()
{
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Distortion.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pBuffer)
		return E_FAIL;

	return S_OK;
}

CDistortion* CDistortion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag)
{
	CDistortion* pInstance = New CDistortion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTargetTag)))
	{
		MSG_BOX("Failed to Created CDistortion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDistortion::Free()
{
	__super::Free();

	Safe_Release(m_pOriginTexture);
	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pAlphaTexture);


	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
}
