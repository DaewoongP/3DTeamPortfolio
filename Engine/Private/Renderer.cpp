#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "RenderTarget_Manager.h"
#include "Light_Manager.h"
#include "PipeLine.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

#ifdef _DEBUG
#include "Input_Device.h"
#endif // _DEBUG


CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pRenderTarget_Manager( CRenderTarget_Manager::GetInstance() )
	, m_pLight_Manager( CLight_Manager::GetInstance() )
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
	_uint2 SizeShadow = { 8000,6000 };
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Shadow_Depth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f),true)))
		return E_FAIL;
	
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Shadow"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_SSAO"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Blur"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Picking"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f), true)))
		return E_FAIL; // 맵 오브젝터 Fast Picking을 위한 렌더 타겟

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
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_Shadow"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Shadow_Depth"), TEXT("Target_Shadow_Depth"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_SSAO"), TEXT("Target_SSAO"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Blur"), TEXT("Target_Blur"))))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Picking"), TEXT("Target_Picking"))))  // 맵 오브젝터 Fast Picking을 위한 렌더 타겟
		return E_FAIL;
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
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Specular"), 240.f, 240.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Shadow"), 240.f, 400.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Shadow_Depth"), 80.f, 560.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Blur"), 240.f, 560.f, 160.f, 160.f)))
		return E_FAIL;

	/*if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_SSAO"), 300.f, 300.f, 600.f, 600.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Blur"), 900.f, 300.f, 600.f, 600.f)))
		return E_FAIL;*/


	/*if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_PostProcessing"), 240.f, 560.f, 160.f, 160.f)))
		return E_FAIL;*/

	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Picking"), 1200.f, 80.f, 160.f, 160.f)))
		return E_FAIL; // 맵 오브젝터 Fast Picking을 위한 렌더 타겟
	
#endif // _DEBUG

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
	
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Lights()))
		return E_FAIL;
#ifdef _DEBUG
	if (FAILED(Render_Picking())) 	// 맵 오브젝터 Fast Picking을 위한 렌더 타겟
		return E_FAIL;
#endif // _DEBUG
	
	if (FAILED(Render_Shadow()))
		return E_FAIL;
	if (FAILED(Render_SSAO()))
		return E_FAIL;
	if (FAILED(Render_Blur()))
		return E_FAIL;
	
	if (FAILED(Render_Deferred()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->End_PostProcessingRenderTarget(m_pContext)))
		return E_FAIL;
	
	if (FAILED(Render_PostProcessing()))
		return E_FAIL;	
	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Render_UITexture()))
		return E_FAIL;
#endif // _DEBUG


#ifdef _DEBUG
	CInput_Device* pInput_Device = CInput_Device::GetInstance();
	Safe_AddRef(pInput_Device);

	if (pInput_Device->Get_DIKeyState(DIK_F1, CInput_Device::KEY_DOWN))
	{
		if (true == m_isDebugRender)
			m_isDebugRender = false;
		else
			m_isDebugRender = true;
	}

	if (true == m_isDebugRender)
	{
		if (FAILED(Render_Debug()))
			return E_FAIL;
	}

	Safe_Release(pInput_Device);
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

HRESULT CRenderer::Render_NonBlend()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Shadow_Depth"))))
		return E_FAIL;
	//m_pDevice->

	for (auto& pGameObject : m_RenderObjects[RENDER_NONBLEND])
	{

		if (nullptr != pGameObject)
			pGameObject->Render_Depth();
	}


	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
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

HRESULT CRenderer::Render_Shadow()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Shadow"))))
		return E_FAIL;
	
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pSSAOShader, "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shadow_Depth"), m_pSSAOShader, "g_vLightDepthTexture")))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pSSAOShader->Bind_Matrix("g_ViewMatrixInv",pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->Bind_Matrix("g_ProjMatrixInv",pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Bind_RawValue("g_fCamFar", pPipeLine->Get_CamFar(), sizeof(_float))))
		return E_FAIL;
	//빛의값을 던져주기
	if (FAILED(m_pSSAOShader->Bind_Matrix("g_vLightView", m_pLight_Manager->Get_LightView())))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Bind_Matrix("g_vLightProj", m_pLight_Manager->Get_LightProj())))
		return E_FAIL;

	Safe_Release(pPipeLine);


	m_pSSAOShader->Begin("Shadow");

	m_pSSAOBuffer->Render();



	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	// 객체를 받아오고 빛과 연산을 해야하지않을지>?
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

	if (FAILED(m_pSSAOShader->Begin("SSAO")))
		return E_FAIL;

	if (FAILED(m_pSSAOBuffer->Render()))
		return E_FAIL;

	if(FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Diffuse"), m_pDeferredShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shade"), m_pDeferredShader, "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Blur"), m_pDeferredShader, "g_BlurTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shadow"), m_pDeferredShader, "g_ShadowTexture")))
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

	if (FAILED(m_pDeferredBuffer->Render()))
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
	for (auto& pGameObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blur()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Blur"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_SSAO"), m_pSSAOShader, "g_SSAOTexture")))
		return E_FAIL;
	

	if (FAILED(m_pSSAOShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->Begin("BlurX")))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;
	
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Blur"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Blur"), m_pShadeTypeShader, "g_BlurTexture")))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->Begin("BlurY")))
		return E_FAIL;

	if (FAILED(m_pSSAOBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;
	return S_OK;
}

HRESULT CRenderer::Render_PostProcessing()
{
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_PostProcessing"), m_pPostProcessingShader, "g_PostProcessingTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pPostProcessingShader->Begin("PostProcessing");

	m_pPostProcessingBuffer->Render();

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

		// 내림차순 (멀리있는거부터 그림.)
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
		// 내림차순 (멀리있는거부터 그림.)
		if (fSourZ > fDestZ)
			return true;
		return false;
		});

	return S_OK;
}

HRESULT CRenderer::Add_Components()
{
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

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	for (auto& pDebugCom : m_DebugObject)
	{
		if (nullptr != pDebugCom &&
			true == m_isDebugRender)
			pDebugCom->Render();

		Safe_Release(pDebugCom);
	}

	m_DebugObject.clear();

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

#endif // _DEBUG

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
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

	Safe_Release(m_pSSAOBuffer);
	Safe_Release(m_pSSAOShader);
	Safe_Release(m_pShadeTypeShader);
	Safe_Release(m_pShadeTypeBuffer);
	Safe_Release(m_pDeferredShader);
	Safe_Release(m_pDeferredBuffer);
	Safe_Release(m_pPostProcessingShader);
	Safe_Release(m_pPostProcessingBuffer);
}
