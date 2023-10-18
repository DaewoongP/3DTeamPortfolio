#include "..\Public\Renderer.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "RenderTarget_Manager.h"
#include "Timer_Manager.h"
#include "Light_Manager.h"
#include "PipeLine.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"

#include "Blur.h"
#include "Bloom.h"
#include "Glow.h"
#include "Shadow.h"
#include "DOF.h"
#include "Flowmap.h"

#ifdef _DEBUG
#include "Input_Device.h"
#include "Font_Manager.h"
#endif // _DEBUG

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
		TEXT("Target_Priority"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
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
		TEXT("Target_Emissive"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Specular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Deferred"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Shadow_Depth"), (_uint)ViewportDesc.Width * 6, (_uint)ViewportDesc.Height * 6, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f), true)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Shadow_Depth1"), (_uint)ViewportDesc.Width * 6, (_uint)ViewportDesc.Height * 6, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f), true)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_SSAO"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_SSAO_BlurX"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_SSAO_Blured"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Distortion"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_HDR"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Effect"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_PostProcessing"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_DistortionRender"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_RadialBlur"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Final"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Fog"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Picking"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_UI"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
#endif // _DEBUG

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Priority"), TEXT("Target_Priority"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Specular"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Deferred"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Shadow_Depth"), TEXT("Target_Shadow_Depth"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Shadow_Depth1"), TEXT("Target_Shadow_Depth1"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_SSAO"), TEXT("Target_SSAO"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_SSAO_BlurX"), TEXT("Target_SSAO_BlurX"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_SSAO_Blured"), TEXT("Target_SSAO_Blured"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_HDR"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_PostProcessing"), TEXT("Target_PostProcessing"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_DistortionRender"), TEXT("Target_DistortionRender"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_RadialBlur"), TEXT("Target_RadialBlur"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Final"), TEXT("Target_Final"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Fog"), TEXT("Target_Fog"))))
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
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Deferred"), 80.f, 80.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_DOF"), 80.f, 240.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_SSAO"), 240.f, 80.f, 160.f, 160.f)))
		return E_FAIL;

	m_isDebugRender = false;
	m_isMRTRender = false;	
#endif // _DEBUG
	m_isSSAO = true;
	m_fGlowPower = 7.5f;
	m_fHDR = 0.5f;
	m_fRadialBlurWidth = 0.0f;
	m_isScreenRadial = false;
	m_fRadialTimeAcc = 1.f;

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

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugGroup(CComponent* pDebugCom)
{
	if (false == m_isDebugRender)
		return S_OK;

	m_DebugObject.push_back(pDebugCom);

	Safe_AddRef(pDebugCom);

	return S_OK;
}
#endif // _DEBUG

HRESULT CRenderer::Draw_RenderGroup()
{
	// 스카이박스 렌더링
	if (FAILED(Render_Priority()))
		return E_FAIL;

#pragma region MRT_Deferred
	// 객체 전부를 그린 렌더타겟 저장
	// 빛연산 포함. (객체와 깊이관련 연산은 전부 이안에서 처리)
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Deferred"))))
		return E_FAIL;
	if (FAILED(Render_Depth()))
		return E_FAIL;
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Lights()))
		return E_FAIL;
	if (FAILED(m_pShadow->Render()))
		return E_FAIL;
	if (FAILED(Render_Deferred()))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Deferred"))))
		return E_FAIL;
#pragma endregion

#pragma region Render Targets
	if (FAILED(m_pDOF->Render(TEXT("Target_Deferred"))))
		return E_FAIL;
	if (FAILED(Render_HDR()))
		return E_FAIL;

	if (FAILED(Sort_Render(RENDER_GLOW)))
		return E_FAIL;
	if (FAILED(m_pGlow->Render(m_RenderObjects[RENDER_GLOW], m_fGlowPower)))
		return E_FAIL;
	if (FAILED(Render_SSAO()))
		return E_FAIL;
	if (FAILED(Render_Fog()))
		return E_FAIL;
	
#pragma endregion

#pragma region MRT_PostProcessing
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_PostProcessing"))))
		return E_FAIL;
	if (FAILED(Render_PostProcessing()))
		return E_FAIL;
	if (FAILED(Render_NonLight()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_PostProcessing"))))
		return E_FAIL;
#pragma endregion

	// After Effect
	if (FAILED(Render_Distortion()))
		return E_FAIL;
	
	if (FAILED(Render_RadialBlur()))
		return E_FAIL;

	// Screen Shading
	if (FAILED(Render_Screen()))
		return E_FAIL;
	if (FAILED(Render_ScreenRadial()))
		return E_FAIL;

	// UI 렌더링
	if (FAILED(Render_UI()))
		return E_FAIL;

#pragma region Debugs
#ifdef _DEBUG
	if (FAILED(Render_Picking()))
		return E_FAIL;
	//if (FAILED(Render_Brushing()))
	//	return E_FAIL;
	if (FAILED(Render_UITexture()))
		return E_FAIL;

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

	Safe_Release(pFont_Manager);
#endif // _DEBUG
#pragma endregion

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Priority"))))
		return E_FAIL;
	
	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Priority"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Depth()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	if (0 == m_pLight_Manager->Get_CurrentLightShadowNum())
	{
		for (auto& pGameObject : m_RenderObjects[RENDER_DEPTH])
		{
			Safe_Release(pGameObject);
		}

		m_RenderObjects[RENDER_DEPTH].clear();

		return S_OK;
	}

	// 1번 그림자 뎁스

	const _bool* pShadowRender = m_pLight_Manager->Get_IsShadowRender();

	if (true == pShadowRender[0])
	{
		if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Shadow_Depth"), true)))
			return E_FAIL;

		_float4x4 LightViewMatrix = *m_pLight_Manager->Get_LightViewMatrix(0);
		_float4x4 LightProjMatrix = *m_pLight_Manager->Get_LightProjMatrix(0);

		for (auto& pGameObject : m_RenderObjects[RENDER_DEPTH])
		{
			if (nullptr != pGameObject)
				pGameObject->Render_Depth(LightViewMatrix, LightProjMatrix);
			// 다음렌더를 안하면 지운다
			if (false == pShadowRender[1])
				Safe_Release(pGameObject);
		}

		if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Shadow_Depth"), true)))
			return E_FAIL;
	}

	// 2번 그림자 뎁스
	// 2개일때만 활성화 해서 그림.
	if (true == pShadowRender[1])
	{
		if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Shadow_Depth1"), true)))
			return E_FAIL;

		_float4x4 LightViewMatrix = *m_pLight_Manager->Get_LightViewMatrix(1);
		_float4x4 LightProjMatrix = *m_pLight_Manager->Get_LightProjMatrix(1);
		for (auto& pGameObject : m_RenderObjects[RENDER_DEPTH])
		{
			if (nullptr != pGameObject)
				pGameObject->Render_Depth(LightViewMatrix, LightProjMatrix);

			// if (false == pShadowRender[2])
			Safe_Release(pGameObject);
		}

		if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Shadow_Depth1"), true)))
			return E_FAIL;
	}

	m_RenderObjects[RENDER_DEPTH].clear();

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

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_GameObjects"))))
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

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Picking"))))
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

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Brushing"))))
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

	if (FAILED(m_pLightShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pLightShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pLightShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pLightShader->Bind_Matrix("g_ViewMatrixInv", pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pLightShader->Bind_Matrix("g_ProjMatrixInv", pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pLightShader->Bind_RawValue("g_vCamPosition", pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pLightShader->Bind_RawValue("g_fCamFar", pPipeLine->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Normal"), m_pLightShader, "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pLightShader, "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pLight_Manager->Render_Lights(m_pLightShader, m_pRectBuffer)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Lights"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_SSAO()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	// SSAO
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

	if (FAILED(m_pSSAOShader->Begin("SSAO")))
		return E_FAIL;

	if (FAILED(m_pRectBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_SSAO"))))
		return E_FAIL;
	
	// Blur
	if (FAILED(m_pBlur->Render(TEXT("MRT_SSAO_BlurX"), TEXT("Target_SSAO"), CBlur::BLUR_X)))
		return E_FAIL;
	if (FAILED(m_pBlur->Render(TEXT("MRT_SSAO_Blured"), TEXT("Target_SSAO_BlurX"), CBlur::BLUR_Y)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Diffuse"), m_pDeferredShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shade"), m_pDeferredShader, "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shadow_Blured"), m_pDeferredShader, "g_ShadowTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Specular"), m_pDeferredShader, "g_SpecularTexture")))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Begin("Deferred")))
		return E_FAIL;

	if (FAILED(m_pRectBuffer->Render()))
		return E_FAIL;

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
	if (FAILED(Sort_Render(RENDER_BLEND)))
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

HRESULT CRenderer::Render_HDR()
{
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_HDR"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Priority"), m_pPostProcessingShader, "g_SkyTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Deferred"), m_pPostProcessingShader, "g_DeferredTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_DOF_Blured"), m_pPostProcessingShader, "g_DOFTexture")))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_RawValue("g_fHDRPower", &m_fHDR, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pPostProcessingShader->Begin("HDR")))
		return E_FAIL;

	if (FAILED(m_pRectBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_HDR"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Fog()
{
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Fog"))))
		return E_FAIL;

	if (FAILED(m_pNoiseTexture->Bind_ShaderResource(m_pFogShader, "g_NoiseTexture")))
		return E_FAIL;

	if (FAILED(m_pFogShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pFogShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pFogShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pFogShader->Begin("Fog")))
		return E_FAIL;

	if (FAILED(m_pRectBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Fog"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_PostProcessing()
{
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_HDR"), m_pPostProcessingShader, "g_HDRTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Glowed"), m_pPostProcessingShader, "g_GlowTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_SSAO_Blured"), m_pPostProcessingShader, "g_SSAOTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pPostProcessingShader, "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ViewMatrixInv", pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ProjMatrixInv", pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_RawValue("g_fCamFar", pPipeLine->Get_CamFar(), sizeof(_float))))
		return E_FAIL;
	Safe_Release(pPipeLine);

	// fog
	if (FAILED(m_pPostProcessingShader->Bind_RawValue("g_vFogColor", &m_vFogColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_RawValue("g_vCircleFogCenter", &m_vFogCenterPos, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_RawValue("g_fCircleFogRadius", &m_fFogRadius, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_RawValue("g_isCircleFog", &m_isCircleFog, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pPostProcessingShader->Bind_RawValue("g_isSSAO", &m_isSSAO, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pPostProcessingShader->Begin("PostProcessing")))
		return E_FAIL;

	if (FAILED(m_pRectBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	// Rendering
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Distortion"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_DISTORTION])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_DISTORTION].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Distortion"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_DistortionRender"))))
		return E_FAIL;

	// Shader
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Distortion"), m_pDistortionShader, "g_DistortionTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_PostProcessing"), m_pDistortionShader, "g_PostProcessingTexture")))
		return E_FAIL;
	if (FAILED(m_pDistortionShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pDistortionShader->Begin("Distortion")))
		return E_FAIL;

	if (FAILED(m_pRectBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_DistortionRender"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_RadialBlur()
{
	// Rendering
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_RadialBlur"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_RADIALBLUR])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_RADIALBLUR].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_RadialBlur"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Final"))))
		return E_FAIL;
	// Shader
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_RadialBlur"), m_pRadialBlurShader, "g_RadialBlurTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_DistortionRender"), m_pRadialBlurShader, "g_TargetTexture")))
		return E_FAIL;
	if (FAILED(m_pRadialBlurShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pRadialBlurShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pRadialBlurShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRadialBlurShader->Begin("Radial")))
		return E_FAIL;

	if (FAILED(m_pRectBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Final"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Screen()
{
	if (FAILED(Sort_Z(RENDER_SCREEN)))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_SCREEN])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_SCREEN].clear();

	return S_OK;
}

HRESULT CRenderer::Render_ScreenRadial()
{
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Final"), m_pRadialBlurShader, "g_TargetTexture")))
		return E_FAIL;
	if (FAILED(m_pRadialBlurShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pRadialBlurShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pRadialBlurShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (0.f < m_fRadialTime)
	{
		CTimer_Manager* pTimer_Manager = CTimer_Manager::GetInstance();
		Safe_AddRef(pTimer_Manager);
		m_fRadialTimeAcc += pTimer_Manager->Get_World_Tick();
		Safe_Release(pTimer_Manager);
	}

	_float fRadialBlurWidth = 0.f;
	if (m_fRadialTimeAcc > m_fRadialTime)
	{
		m_isScreenRadial = false;
	}
	else
	{
		fRadialBlurWidth = Lerp(m_fRadialBlurWidth, 0.f, m_fRadialTimeAcc / m_fRadialTime);
		m_isScreenRadial = true;
	}

	if (FAILED(m_pRadialBlurShader->Bind_RawValue("g_isScreenRadial", &m_isScreenRadial, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pRadialBlurShader->Bind_RawValue("g_fBlurWidth", &fRadialBlurWidth, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pRadialBlurShader->Begin("RadialScreen")))
		return E_FAIL;

	if (FAILED(m_pRectBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	if (FAILED(Sort_Z(RENDER_UI)))
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

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_UI"))))
		return E_FAIL;

	return S_OK;
}
#endif // _DEBUG

HRESULT CRenderer::Sort_Render(RENDERGROUP eGroup)
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

HRESULT CRenderer::Sort_Z(RENDERGROUP eGroup)
{
	m_RenderObjects[eGroup].sort([](const CGameObject* pSour, const CGameObject* pDest) {
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
	m_pRectBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pRectBuffer)
		return E_FAIL;

	m_pDeferredShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pDeferredShader)
		return E_FAIL;

	m_pLightShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Light.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pLightShader)
		return E_FAIL;

	m_pPostProcessingShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PostProcessing.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pPostProcessingShader)
		return E_FAIL;
	
	m_pDistortionShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Distortion.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pDistortionShader)
		return E_FAIL;

	m_pSSAOShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_SSAO.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pSSAOShader)
		return E_FAIL;
	
	m_pRadialBlurShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_RadialBlur.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pRadialBlurShader)
		return E_FAIL;
	
	m_pFogShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Fog.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pFogShader)
		return E_FAIL;

	m_pNoiseTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Noises/VFX_T_Cloud_Noise_Tile_D.png"));
	if (nullptr == m_pNoiseTexture)
		return E_FAIL;

	m_pShadow = CShadow::Create(m_pDevice, m_pContext, m_pRectBuffer);
	if (nullptr == m_pShadow)
		return E_FAIL;
	
	m_pBloom = CBloom::Create(m_pDevice, m_pContext, m_pRectBuffer);
	if (nullptr == m_pBloom)
		return E_FAIL;

	m_pBlur = CBlur::Create(m_pDevice, m_pContext, m_pRectBuffer);
	if (nullptr == m_pBlur)
		return E_FAIL;

	m_pGlow = CGlow::Create(m_pDevice, m_pContext, m_pRectBuffer);
	if (nullptr == m_pGlow)
		return E_FAIL;
	
	m_pDOF = CDOF::Create(m_pDevice, m_pContext, m_pRectBuffer);
	if (nullptr == m_pDOF)
		return E_FAIL;
	
	m_pFlowmap = CFlowmap::Create(m_pDevice, m_pContext, m_pRectBuffer);
	if (nullptr == m_pFlowmap)
		return E_FAIL;
	
	return S_OK;
}
#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugObject)
	{
		if (nullptr != pDebugCom)
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

	if (FAILED(m_pRenderTarget_Manager->Render_Debug(TEXT("MRT_GameObjects"), m_pDeferredShader, m_pRectBuffer)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Render_Debug(TEXT("MRT_Lights"), m_pDeferredShader, m_pRectBuffer)))
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

	Safe_Release(m_pDeferredShader);
	Safe_Release(m_pLightShader);
	Safe_Release(m_pPostProcessingShader);
	Safe_Release(m_pSSAOShader);
	Safe_Release(m_pDistortionShader);
	Safe_Release(m_pRadialBlurShader);
	Safe_Release(m_pFogShader);
	Safe_Release(m_pNoiseTexture);

	Safe_Release(m_pBlur);
	Safe_Release(m_pBloom);
	Safe_Release(m_pShadow);
	Safe_Release(m_pGlow);
	Safe_Release(m_pDOF);
	Safe_Release(m_pFlowmap);

	Safe_Release(m_pRectBuffer);
}
