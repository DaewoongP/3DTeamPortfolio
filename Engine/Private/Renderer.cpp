#include "..\Public\Renderer.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "RenderTarget_Manager.h"
#include "Light_Manager.h"
#include "PipeLine.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"

#include "Blur.h"
#include "Bloom.h"
#include "Distortion.h"
#include "Glow.h"
#include "Shadow.h"
#include "MotionBlur.h"
#include"FlowMap.h"	
#include"DOF.h"

#ifdef _DEBUG
#include "Input_Device.h"
#include "Font_Manager.h"
#endif // _DEBUG

const _char* CRenderer::pRenderGroup[RENDER_END] = { "Render_Priority", "Render_Depth", "Render_Nonblend"
, "Render_NonLight", "Render_Blend", "Render_Picking", "Render_Brushing", "Render_UI", "Render_UITexture" };

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_pRenderTarget_Manager(CRenderTarget_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
{
	Safe_AddRef(m_pRenderTarget_Manager);
	Safe_AddRef(m_pLight_Manager);
}


HRESULT CRenderer::Initialize_Prototype()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;
	_uint				iNumViews = { 1 };
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Normal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Depth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Specular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_PostProcessing"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Shadow_Depth"), (_uint)ViewportDesc.Width * 12, (_uint)ViewportDesc.Height * 12, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f), true)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Combine"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_SSAO"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Distortion"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_SSAOBluring"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_FinBloom"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Picking"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_UI"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
#endif // _DEBUG


	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Specular"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_PostProcessing"), TEXT("Target_PostProcessing"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Shadow_Depth"), TEXT("Target_Shadow_Depth"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_SSAO"), TEXT("Target_SSAO"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_FinBloom"), TEXT("Target_FinBloom"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Combine"), TEXT("Target_Combine"))))
		return E_FAIL;
#ifdef _DEBUG
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Picking"), TEXT("Target_Picking"))))
		return E_FAIL;
	/*if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Brushing"), TEXT("Target_MapBrushing"))))
		return E_FAIL;*/
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_UI"), TEXT("Target_UI"))))  // UI
		return E_FAIL;
#endif // _DEBUG

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

#ifdef _DEBUG 
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Diffuse"), 80.f, 80.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Normal"), 80.f, 240.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Depth"), 80.f, 400.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Shade"), 240.f, 80.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_FlowMap"), 240.f, 240.f, 160.f, 160.f)))
		return E_FAIL;
#endif // _DEBUG
	m_pNoiseTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/whitenoise.png"));

	return S_OK;
}

void CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pGameObject)
{
	if (eRenderGroup < RENDERGROUP::RENDER_PRIORITY ||
		eRenderGroup >= RENDERGROUP::RENDER_END ||
		nullptr == pGameObject)
	{
		MSG_BOX("Failed Add RenderGroup");
		return;
	}

	m_RenderObjects[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);
}

void CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pGameObject, _float LightPower)
{
	if (eRenderGroup < RENDERGROUP::RENDER_PRIORITY ||
		eRenderGroup >= RENDERGROUP::RENDER_END ||
		nullptr == pGameObject)
	{
		MSG_BOX("Failed Add RenderGroup");
		return;
	}
	
	m_RenderObjects[eRenderGroup].push_back(pGameObject);
	m_fGlowPower = LightPower;
	Safe_AddRef(pGameObject);
}

void CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pGameObject, _tchar* pShaderPass, CTexture* pOriTexture, CTexture*  pNoisetexture, CTexture* pAlphaTexture)
{
	Add_RenderGroup(eRenderGroup, pGameObject);
	m_pDistortion->Set_Textures(pShaderPass, pOriTexture, pNoisetexture, pAlphaTexture);
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugGroup(CComponent* pDebugCom)
{
	m_DebugObject.push_back(pDebugCom);

	Safe_AddRef(pDebugCom);

	return S_OK;
}
#endif // _DEBUG

HRESULT CRenderer::Draw_RenderGroup()
{
	if (FAILED(m_pRenderTarget_Manager->Begin_PostProcessingRenderTarget(m_pContext, TEXT("MRT_PostProcessing"))))
		return E_FAIL;

	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_Depth()))
		return E_FAIL;
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Lights()))
		return E_FAIL; 
	if (FAILED(Render_EffectType()))
		return E_FAIL;
	if (FAILED(m_pShadow->Render()))
		return E_FAIL;
	if (FAILED(m_pFlowMap->Render()))
		return E_FAIL;
	if (FAILED(Render_SSAO()))
		return E_FAIL;
	if (FAILED(m_pDistortion->Render()))
		return E_FAIL;
	if (FAILED(m_pSSAOBlur->Render()))
		return E_FAIL;
	if (FAILED(Render_Combine()))
		return E_FAIL;

	if (FAILED(Render_Deferred()))
		return E_FAIL;

	if (FAILED(Render_MotionBlurInst()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(m_pBloom->Render()))
		return E_FAIL;

	if (FAILED(m_pGlow->Render()))
		return E_FAIL;
	if (FAILED(m_pDOF->Render()))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->End_PostProcessingRenderTarget(m_pContext)))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Render_Picking()))
		return E_FAIL;
	//if (FAILED(Render_Brushing()))
	//	return E_FAIL;
#endif // _DEBUG
	
	if (FAILED(Render_PostProcessing()))
		return E_FAIL;
	


	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	/*if (FAILED(Render_UITexture()))
		return E_FAIL;*/
#endif // _DEBUG

#ifdef _DEBUG
	CFont_Manager* pFont_Manager = CFont_Manager::GetInstance();
	Safe_AddRef(pFont_Manager);

	Is_DebugRender();
	
	if (FAILED(Render_Debug()))
		return E_FAIL;

	if (true == m_isDebugRender)
	{
		if (FAILED(pFont_Manager->Render_Font(TEXT("Font_135"), TEXT("Debug Render"), _float2(1120.f, 690.f),
			_float4(0.f, 1.f, 0.f, 1.f), 0.f, _float2(), 0.5f)))
			return E_FAIL;
	}
	
	if (true == Is_MRTRender())
	{
		if (FAILED(Render_MRTs()))
			return E_FAIL;
		if (FAILED(pFont_Manager->Render_Font(TEXT("Font_135"), TEXT("Target Render"), _float2(1120.f, 660.f),
			_float4(1.f, 0.f, 0.f, 1.f), 0.f, _float2(), 0.5f)))
			return E_FAIL;
	}
	if (true == Is_Render_Distortion())
	{
		if (FAILED(Render_Distortion()))
			return E_FAIL;
	}
	Safe_Release(pFont_Manager);
#endif // _DEBUG

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}
HRESULT CRenderer::Render_Depth()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Shadow_Depth"), true)))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_DEPTH])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_Depth();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_DEPTH].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, true)))
		return E_FAIL;

	return S_OK;
}
HRESULT CRenderer::Render_NonBlend()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONBLEND].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Picking()
{
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Picking"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_PICKING])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_PICKING].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Brushing()
{
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Brushing"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_BRUSHING])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_BRUSHING].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}
#endif // _DEBUG


HRESULT CRenderer::Render_Lights()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Lights"))))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ViewMatrixInv", pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ProjMatrixInv", pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Bind_RawValue("g_vCamPosition", pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_RawValue("g_fCamFar", pPipeLine->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Normal"), m_pDeferredShader, "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pDeferredShader, "g_DepthTexture")))
		return E_FAIL;

	m_pLight_Manager->Render_Lights(m_pDeferredShader, m_pDeferredBuffer);

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_SSAO()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_SSAO"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pSSAOShader, "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Normal"), m_pSSAOShader, "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pSSAOShader->Bind_RawValue("g_fCamFar", pPipeLine->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	m_pNoiseTexture->Bind_ShaderResource(m_pSSAOShader, "g_NoiseTexture");

	if (FAILED(m_pSSAOShader->Begin("SSAO")))
		return E_FAIL;

	if (FAILED(m_pSSAOBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Diffuse"), m_pDeferredShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shade"), m_pDeferredShader, "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_SSAO"), m_pDeferredShader, "g_SSAOTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shadow"), m_pDeferredShader, "g_ShadowTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Specular"), m_pDeferredShader, "g_SpecularTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_MapEffect"), m_pDeferredShader, "g_MapEffectTexture")))
		return E_FAIL;


	if (FAILED(m_pDeferredShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Begin("Deferred")))
		return E_FAIL;

	if (FAILED(m_pDeferredBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Combine()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Combine"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Diffuse"), m_pDeferredShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shade"), m_pDeferredShader, "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_SSAO"), m_pDeferredShader, "g_SSAOTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shadow"), m_pDeferredShader, "g_ShadowTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Specular"), m_pDeferredShader, "g_SpecularTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_MapEffect"), m_pDeferredShader, "g_MapEffectTexture")))
		return E_FAIL;


	if (FAILED(m_pDeferredShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Begin("Deferred")))
		return E_FAIL;

	if (FAILED(m_pDeferredBuffer->Render()))
		return E_FAIL;

	if (m_pRenderTarget_Manager->End_MRT(m_pContext))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_MotionBlurInst()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	//if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Blend"))))
	//	return E_FAIL;

	//for (auto& pGameObject : m_RenderObjects[RENDER_MOTIONBLUR])
	//{
	//	if (nullptr != pGameObject)
	//		pGameObject->Render();

	//	Safe_Release(pGameObject);
	//}

	//m_RenderObjects[RENDER_MOTIONBLUR].clear();

	//if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	if (FAILED(Sort_Blend()))
		return E_FAIL;
	
	for (auto& pGameObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_PostProcessing()
{
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_PostProcessing"), m_pPostProcessingShader, "g_PostProcessingTexture")))
		return E_FAIL;
	/*if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_FinBloom"), m_pPostProcessingShader, "g_BloomTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_FinGlow"), m_pPostProcessingShader, "g_GlowTexture")))
		return E_FAIL;*/
	
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_DOFBlurY"), m_pPostProcessingShader, "g_DOFTexture")))
		return E_FAIL; 
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_FinBloom"), m_pPostProcessingShader, "g_BloomTexture")))
		return E_FAIL; 
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_FinGlow"), m_pPostProcessingShader, "g_GlowTexture")))
		return E_FAIL;
	
	m_pPostProcessingShader->Begin("PostProcessing");

	m_pPostProcessingBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_EffectType()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_FinBloom"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_BLOOM])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_BLOOM].clear();
	
	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Distortion"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_DISTORTION])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_DISTORTION].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_FinGlow"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_GLOW])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_GLOW].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_MotionBlur"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_MOTIONBLUR])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_MOTIONBLUR].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	m_pMotionBlurInstance->Begin_MRT();
	for (auto& pGameObject : m_RenderObjects[RENDER_MOTIONBLUR])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_MOTIONBLUR].clear();
	m_pMotionBlurInstance->End_MRT();

	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	
	//m_pTexture->Bind_ShaderResource(m_pAfterShader, "g_NoiseTexture");
	//m_pTexture2->Bind_ShaderResource(m_pAfterShader, "g_AlphaTexture");
	//m_pTexture3->Bind_ShaderResource(m_pAfterShader, "g_PostProcessingTexture");

	///*if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_PostProcessing"), m_pAfterShader, "g_PostProcessingTexture")))
	//	return E_FAIL;*/


	//if (FAILED(m_pAfterShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pAfterShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pAfterShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	//	return E_FAIL;
	//m_fFrameTime += 0.01f;
	//if (m_fFrameTime > 1000.f)
	//	m_fFrameTime = 0.f;
	//if (FAILED(m_pAfterShader->Bind_RawValue("g_FrameTime", &m_fFrameTime, sizeof(_float))))
	//	return E_FAIL;
	//_float3 Speed = { 1.3f, 2.1f, 2.3f };
	//if (FAILED(m_pAfterShader->Bind_RawValue("g_ScrollSpeed", &Speed, sizeof(_float3))))
	//	return E_FAIL;
	//_float3 Scale = { 1.f, 2.f, 3.f };
	//if (FAILED(m_pAfterShader->Bind_RawValue("g_Scales", &Scale, sizeof(_float3))))
	//	return E_FAIL;
	//m_pAfterShader->Begin("Distortion");

	//m_pAfterShaderBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	if (FAILED(Sort_UI()))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_UITexture()
{
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_UI"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_UITEXTURE])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UITEXTURE].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}
#endif // _DEBUG

HRESULT CRenderer::Sort_Blend()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float4 vCamPos = *pPipeLine->Get_CamPosition();

	Safe_Release(pPipeLine);

	m_RenderObjects[RENDER_BLEND].sort([vCamPos](const CGameObject* pSour, const CGameObject* pDest) {
		_float3 vSourPos = pSour->Get_Transform()->Get_Position();
		_float3 vDestPos = pDest->Get_Transform()->Get_Position();

		_float4 vSour = vSourPos - vCamPos;
		_float4 vDest = vDestPos - vCamPos;

		if (vSour.Length() > vSour.Length())
			return true;

		return false;
	});

	return S_OK;
}

HRESULT CRenderer::Sort_UI()
{
	m_RenderObjects[RENDER_UI].sort([](const CGameObject* pSour, const CGameObject* pDest) {
		_float fSourZ = XMVectorGetZ(pSour->Get_Transform()->Get_Position());
		_float fDestZ = XMVectorGetZ(pDest->Get_Transform()->Get_Position());
	
		if (fSourZ > fDestZ)
			return true;
		return false;
	});

	return S_OK;
}

HRESULT CRenderer::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pDeferredShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pDeferredShader)
		return E_FAIL;

	m_pDeferredBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pDeferredBuffer)
		return E_FAIL;

	m_pPostProcessingShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PostProcessing.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pPostProcessingShader)
		return E_FAIL;

	m_pPostProcessingBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pPostProcessingBuffer)
		return E_FAIL;

	m_pSSAOShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_SSAO.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pSSAOShader)
		return E_FAIL;

	m_pSSAOBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pSSAOBuffer)
		return E_FAIL;

	m_pShadeTypeShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Type.hlsl"), VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements);
	if (nullptr == m_pShadeTypeShader)
		return E_FAIL;

	m_pShadeTypeBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pShadeTypeBuffer)
		return E_FAIL;

	m_pAfterShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_AfterShade.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pAfterShader)
		return E_FAIL;

	m_pAfterShaderBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pAfterShaderBuffer)
		return E_FAIL;

	m_pSSAOBlur = CBlur::Create(m_pDevice, m_pContext, TEXT("Target_SSAO"), CBlur::BLUR_XY);
	if (nullptr == m_pSSAOBlur)
		return E_FAIL;
	
	//m_pEffectBlur = CBlur::Create(m_pDevice, m_pContext, TEXT("Target_EffectBlur"), CBlur::BLUR_XY);
	//if (nullptr == m_pEffectBlur)
	//	return E_FAIL;

	//m_pShadowBlur= CBlur::Create(m_pDevice, m_pContext, TEXT("Target_Shadow"), CBlur::BLUR_XY);
	//if (nullptr == m_pShadowBlur)
	//	return E_FAIL;

	m_pBloom = CBloom::Create(m_pDevice, m_pContext, TEXT("Target_FinBloom"));
	if (nullptr == m_pBloom)
		return E_FAIL;

	m_pDistortion = CDistortion::Create(m_pDevice, m_pContext, TEXT("Target_MapEffect"));
	if (nullptr == m_pDistortion)
		return E_FAIL;

	m_pMotionBlurInstance = CMotionBlurInstance::Create(m_pDevice, m_pContext, 0);
	if (nullptr == m_pMotionBlurInstance)
		return E_FAIL;
	m_pGlow = CGlow::Create(m_pDevice, m_pContext, TEXT("Target_FinGlow"), 1.f);//파티클이 갖고있는 정보하나주면 힘을 변경하도록 하면될것같음
	if (nullptr == m_pGlow)
		return E_FAIL;
	m_pMotionBlur = CMotionBlur::Create(m_pDevice, m_pContext, TEXT("Target_FinGlow"));
	if (nullptr == m_pMotionBlur)
		return E_FAIL;

	m_pShadow = CShadow::Create(m_pDevice, m_pContext);
	if (nullptr == m_pShadow)
		return E_FAIL;

	m_pFlowMap = CFlowMap::Create(m_pDevice, m_pContext,TEXT("Target_FlowMap"));
	if (nullptr == m_pFlowMap)
		return E_FAIL;
	m_pDOF = CDOF::Create(m_pDevice, m_pContext, TEXT("Target_PostProcessing"));
	if (nullptr == m_pDOF)
		return E_FAIL;


	Safe_Release(pGameInstance);
	return S_OK;
}
#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{	
	for (auto& pDebugCom : m_DebugObject)
	{
		if (nullptr != pDebugCom &&
			true == m_isDebugRender)
			pDebugCom->Render();

		Safe_Release(pDebugCom);
	}

	m_DebugObject.clear();

	return S_OK;
}

HRESULT CRenderer::Render_MRTs()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Render_Debug(TEXT("MRT_GameObjects"), m_pDeferredShader, m_pDeferredBuffer)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Render_Debug(TEXT("MRT_Lights"), m_pDeferredShader, m_pDeferredBuffer)))
		return E_FAIL;

	return S_OK;
}

void CRenderer::Is_DebugRender()
{
	CInput_Device* pInput_Device = CInput_Device::GetInstance();
	Safe_AddRef(pInput_Device);

	if (pInput_Device->Get_DIKeyState(DIK_F1, CInput_Device::KEY_DOWN))
	{
		if (true == m_isDebugRender)
			m_isDebugRender = false;
		else
			m_isDebugRender = true;
	}
	Safe_Release(pInput_Device);
}

_bool CRenderer::Is_MRTRender()
{
	CInput_Device* pInput_Device = CInput_Device::GetInstance();
	Safe_AddRef(pInput_Device);

	if (pInput_Device->Get_DIKeyState(DIK_F2, CInput_Device::KEY_DOWN))
	{
		if (true == m_isMRTRender)
			m_isMRTRender = false;
		else
			m_isMRTRender = true;
	}
	Safe_Release(pInput_Device);

	return m_isMRTRender;
}

_bool CRenderer::Is_Render_Distortion()
{
	CInput_Device* pInput_Device = CInput_Device::GetInstance();
	Safe_AddRef(pInput_Device);

	if (pInput_Device->Get_DIKeyState(DIK_F3, CInput_Device::KEY_DOWN))
	{
		if (true == m_isDistortion)
			m_isDistortion = false;
		else
			m_isDistortion = true;
	}
	Safe_Release(pInput_Device);

	return m_isDistortion;
}
#endif // _DEBUG

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = New CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRenderer::Clone(void* pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	for (auto& RenderList : m_RenderObjects)
	{
		for (auto& pGameObject : RenderList)
			Safe_Release(pGameObject);

		RenderList.clear();
	}
#ifdef _DEBUG
	for (auto& pDebugCom : m_DebugObject)
		Safe_Release(pDebugCom);

	m_DebugObject.clear();
#endif // _DEBUG


	Safe_Release(m_pRenderTarget_Manager);
	Safe_Release(m_pLight_Manager);

	Safe_Release(m_pTexture);
	Safe_Release(m_pTexture2);
	Safe_Release(m_pTexture3);
	Safe_Release(m_pNoiseTexture);


	Safe_Release(m_pAfterShaderBuffer);
	Safe_Release(m_pAfterShader);

	Safe_Release(m_pMotionBlur);
	Safe_Release(m_pSSAOBuffer);
	Safe_Release(m_pSSAOShader);
	Safe_Release(m_pShadeTypeShader);
	Safe_Release(m_pShadeTypeBuffer);
	Safe_Release(m_pDeferredShader);
	Safe_Release(m_pDeferredBuffer);
	Safe_Release(m_pPostProcessingShader);
	Safe_Release(m_pPostProcessingBuffer);
	
	
	
	Safe_Release(m_pDOF);
	Safe_Release(m_pShadow);
	Safe_Release(m_pBloom);
	Safe_Release(m_pSSAOBlur);
	Safe_Release(m_pDistortion);
	Safe_Release(m_pGlow);
	Safe_Release(m_pMotionBlurInstance);
	Safe_Release(m_pFlowMap);
}
