#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "RenderTarget_Manager.h"
#include "Light_Manager.h"
#include "PipeLine.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"

#include "Blur.h"
#include"Bloom.h"
#include"Distortion.h"
#include"Glow.h"

#ifdef _DEBUG
#include "Input_Device.h"
#include "Font_Manager.h"
#endif // _DEBUG

const _char* CRenderer::pRenderGroup[RENDER_END] = { "Render_Priority", "Render_Depth", "Render_Nonblend"
, "Render_NonLight", "Render_Blend", "Render_Picking", "Render_Brushing", "Render_UI", "Render_UITexture" };

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
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
		TEXT("Target_Shadow_Depth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Shadow"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_SoftShadow"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_SSAO"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_FinBloom"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


#ifdef _DEBUG
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Picking"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL; // ¸Ê ¿ÀºêÁ§ÅÍ Fast PickingÀ» À§ÇÑ ·»´õ Å¸°Ù
	//if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
	//	TEXT("Target_MapBrushing"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
	//	return E_FAIL; // ¸Ê ºê·¯½Ì °á°ú ÀúÀåÀ» À§ÇÑ ·»´õ Å¸°Ù

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
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_Shadow"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_SoftShadow"), TEXT("Target_SoftShadow"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_SSAO"), TEXT("Target_SSAO"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_FinBloom"), TEXT("Target_FinBloom"))))
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
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Specular"), 240.f, 240.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Shadow"), 240.f, 400.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Glow"), 80.f, 560.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_SSAO"), 240.f, 560.f, 160.f, 160.f)))
		return E_FAIL;

	/*if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_SSAO"), 300.f, 300.f, 600.f, 600.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Blur"), 900.f, 300.f, 600.f, 600.f)))
		return E_FAIL;*/
	/*if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_PostProcessing"), 240.f, 560.f, 160.f, 160.f)))
		return E_FAIL;*/

	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Picking"), 1200.f, 80.f, 160.f, 160.f)))
		return E_FAIL; // ¸Ê ¿ÀºêÁ§ÅÍ Fast PickingÀ» À§ÇÑ ·»´õ Å¸°Ù
	//if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_MapBrushing"), 1040.f, 80.f, 160.f, 160.f)))
	//	return E_FAIL; // ¸Ê ºê·¯½Ì °á°ú ÀúÀåÀ» À§ÇÑ ·»´õ Å¸°Ù
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_UI"), 1200.f, 300.f, 160.f, 160.f)))
		return E_FAIL;

#endif // _DEBUG

	m_pTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/noise01.dds"));
	m_pTexture2 = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/alpha01.dds"));
	m_pTexture3 = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/fire01.dds"));


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
	if (FAILED(Render_Depth()))
		return E_FAIL;
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_PostBlend()))
		return E_FAIL;
	if (FAILED(Render_Lights()))
		return E_FAIL;
	if (FAILED(Render_Bloom()))
		return E_FAIL;
	if (FAILED(Render_Shadow()))
		return E_FAIL;
	//if (FAILED(m_pShadowBlur->Render()))
	//	return E_FAIL;
	if (FAILED(Render_SoftShadow()))
		return E_FAIL;
	if (FAILED(Render_BlurShadow()))
		return E_FAIL;
	if (FAILED(m_pSSAOBlur->Render()))
		return E_FAIL;
	if (FAILED(Render_Deferred()))
		return E_FAIL;

	if (FAILED(Render_SSAO()))
		return E_FAIL;
	
	
	if (FAILED(m_pGlow->Render()))
		return E_FAIL;
		
	if (FAILED(Render_NonLight()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	
	if (FAILED(m_pRenderTarget_Manager->End_PostProcessingRenderTarget(m_pContext)))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Render_Picking())) 	// ¸Ê ¿ÀºêÁ§ÅÍ Fast PickingÀ» À§ÇÑ ·»´õ Å¸°Ù
		return E_FAIL;
	//if (FAILED(Render_Brushing())) 	// ¸Ê ºê·¯½Ì °á°ú ÀúÀåÀ» À§ÇÑ ·»´õ Å¸°Ù
	//	return E_FAIL;
#endif // _DEBUG

	if (FAILED(Render_PostProcessing()))
		return E_FAIL;

	if (FAILED(m_pBloom->Render()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Render_UITexture()))
		return E_FAIL;
#endif // _DEBUG

#ifdef _DEBUG
	CFont_Manager* pFont_Manager = CFont_Manager::GetInstance();
	Safe_AddRef(pFont_Manager);

	if (true == Is_DebugRender())
	{
		if (FAILED(Render_Debug()))
			return E_FAIL;
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

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Shadow_Depth"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_DEPTH])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_Depth();
		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_DEPTH].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
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
HRESULT CRenderer::Render_PostBlend()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	//if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_GameObjects"))))
	//	return E_FAIL;

	//for (auto& pGameObject : m_RenderObjects[RENDER_POST_BLEND])
	//{
	//	if (nullptr != pGameObject)
	//		pGameObject->Render();

	//	Safe_Release(pGameObject);
	//}

	//m_RenderObjects[RENDER_POST_BLEND].clear();

	//if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
	//	return E_FAIL;

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

	if (FAILED(m_pSSAOShader->Bind_Matrix("g_ViewMatrixInv", pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->Bind_Matrix("g_ProjMatrixInv", pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->Bind_RawValue("g_fCamFar", pPipeLine->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	_float4x4	ViewMatrix, ProjMatrix;
	//if (!m_pLight_Manager->Light_NullCheck())
	{
		//_float4* Pos = m_pLight_Manager->Get_LightPosition();
		ViewMatrix = XMMatrixLookAtLH(_float4(0.f,10.f,0.f,1.f), _float4(3.f, 0.f, 3.f, 1.f), _float4(0.f, 1.f, 0.f, 0.f));
		ProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), _float(1280) / 720.f, 1.f, 100.f);
	}
	/*else
	{
		_float4* LightPos = m_pLight_Manager->Get_LightPosition();

		_float4 LightDir = m_pLight_Manager->Get_Light(1)->vDir;
		ViewMatrix = XMMatrixLookAtLH(*LightPos, _float4(3.f, 0.f, 3.f, 1.f), LightDir);
		ProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), _float(1280) / 720.f, 1.f, 100.f);
	}*/




	if (FAILED(m_pSSAOShader->Bind_Matrix("g_vLightView", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Bind_Matrix("g_vLightProj", &ProjMatrix)))
		return E_FAIL;
	//ºûÀÇ°ªÀ» ´øÁ®ÁÖ±â
	/*if (FAILED(m_pSSAOShader->Bind_Matrix("g_vLightView", m_pLight_Manager->Get_LightView())))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Bind_Matrix("g_vLightProj", m_pLight_Manager->Get_LightProj())))
		return E_FAIL;*/

	Safe_Release(pPipeLine);


	m_pSSAOShader->Begin("Shadow");

	m_pSSAOBuffer->Render();



	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_SoftShadow()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_SoftShadow"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shade"), m_pShadeTypeShader, "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shadow_Depth"), m_pShadeTypeShader, "g_ShadowTexture")))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pShadeTypeShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShadeTypeShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShadeTypeShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShadeTypeShader->Bind_RawValue("g_fCamFar", pPipeLine->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	_float4x4	ViewMatrix, ProjMatrix;
	ViewMatrix = XMMatrixLookAtLH(_float4(0.f, 10.f, 0.f, 1.f), _float4(3.f, 0.f, 3.f, 1.f), _float4(0.f, 1.f, 0.f, 0.f));
	ProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), _float(1280) / 720.f, 1.f, 100.f);
	if (FAILED(m_pShadeTypeShader->Bind_Matrix("g_vLightView", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShadeTypeShader->Bind_Matrix("g_vLightProj", &ProjMatrix)))
		return E_FAIL;

	_float4 Position = _float4(0.f, 10.f, 0.f, 1.f);
	if (FAILED(m_pShadeTypeShader->Bind_RawValue("g_vLightPos", &Position, sizeof(_float4))))
		return E_FAIL;


	Safe_Release(pPipeLine);


	m_pShadeTypeShader->Begin("Softshadow");

	m_pShadeTypeBuffer->Render();

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
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Blur"), m_pDeferredShader, "g_BlurTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shadow"), m_pDeferredShader, "g_ShadowTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Specular"), m_pDeferredShader, "g_SpecularTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_SoftShadow"), m_pDeferredShader, "g_SoftShadowTexuture")))
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

HRESULT CRenderer::Render_BlurShadow()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	/*if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Blur"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shadow"), m_pSSAOShader, "g_SSAOTexture")))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Begin("BlurX")))
		return E_FAIL;
	if (FAILED(m_pSSAOBuffer->Render()))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Begin("BlurY")))
		return E_FAIL;
	if (FAILED(m_pSSAOBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;*/
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

HRESULT CRenderer::Render_Bloom()
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
	
	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	m_pDistortion->Render();
	//m_pTexture->Bind_ShaderResource(m_pAfterShader, "g_NoiseTexture");
	//m_pTexture2->Bind_ShaderResource(m_pAfterShader, "g_vAlphaTexture");
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

	// ³»¸²Â÷¼ø (¸Ö¸®ÀÖ´Â°ÅºÎÅÍ ±×¸².)
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
	// ³»¸²Â÷¼ø (¸Ö¸®ÀÖ´Â°ÅºÎÅÍ ±×¸².)
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

	m_pAfterShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_AfterShade.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pAfterShader)
		return E_FAIL;

	m_pAfterShaderBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pAfterShaderBuffer)
		return E_FAIL;

	m_pSSAOBlur = CBlur::Create(m_pDevice, m_pContext, TEXT("Target_SSAO"), CBlur::BLUR_XY);
	if (nullptr == m_pSSAOBlur)
		return E_FAIL;
	//m_pShadowBlur= CBlur::Create(m_pDevice, m_pContext, TEXT("Target_Shadow"), CBlur::BLUR_XY);
	//if (nullptr == m_pShadowBlur)
	//	return E_FAIL;

	m_pBloom = CBloom::Create(m_pDevice, m_pContext, TEXT("Target_FinBloom"));
	if (nullptr == m_pBloom)
		return E_FAIL;

	m_pDistortion = CDistortion::Create(m_pDevice, m_pContext, TEXT("Target_Distortion"));
	if (nullptr == m_pDistortion)
		return E_FAIL;

	m_pGlow = CGlow::Create(m_pDevice, m_pContext, TEXT("Target_Glow"));
	if (nullptr == m_pGlow)
		return E_FAIL;
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

_bool CRenderer::Is_DebugRender()
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

	return m_isDebugRender;
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

	Safe_Release(m_pTexture);
	Safe_Release(m_pTexture2);
	Safe_Release(m_pTexture3);

	Safe_Release(m_pAfterShaderBuffer);
	Safe_Release(m_pAfterShader);

	Safe_Release(m_pSSAOBuffer);
	Safe_Release(m_pSSAOShader);
	Safe_Release(m_pShadeTypeShader);
	Safe_Release(m_pShadeTypeBuffer);
	Safe_Release(m_pDeferredShader);
	Safe_Release(m_pDeferredBuffer);
	Safe_Release(m_pPostProcessingShader);
	Safe_Release(m_pPostProcessingBuffer);

	Safe_Release(m_pBloom);
	Safe_Release(m_pSSAOBlur);
	Safe_Release(m_pDistortion);
	Safe_Release(m_pGlow);
}
