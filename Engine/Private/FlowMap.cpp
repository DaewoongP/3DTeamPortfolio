#include "FlowMap.h"
#include"Texture.h"	
#include"RenderTarget_Manager.h"
#include"Shader.h"	
#include"VIBuffer_Rect.h"
#include"GameInstance.h"
#include"Timer_Manager.h"

CFlowMap::CFlowMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :CComponent(pDevice, pContext)
{
}

HRESULT CFlowMap::Initialize_Prototype(const _tchar* pTargetTag)
{
	lstrcpy(m_szTargetTag, pTargetTag);

	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	_uint				iNumViews = { 1 };



	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);


	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_FlowMap"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;


	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_FlowMap"), TEXT("Target_FlowMap"))))
		return E_FAIL;



	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	Safe_Release(pRenderTarget_Manager);

	if (FAILED(Add_Components()))
		return E_FAIL;


	m_fHalfCycle = m_fHalfCycle * 0.5f;
	m_fFlowMapOffset0 = 0.f;
	m_fFlowMapOffset1 = m_fHalfCycle;



	m_pFlowTexture1 = CTexture::Create(m_pDevice, m_pContext, (TEXT("../../Resources/effects/Textures/Flowmaps/T_FX_Flowmap_FM.png")));
	m_pFlowTexture2 = CTexture::Create(m_pDevice, m_pContext, (TEXT("../../Resources/effects/Textures/Flowmaps/T_Sanc_SwirlFlowmap_01_FM.png")));



	return S_OK;
}

HRESULT CFlowMap::Render()
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);
	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_FlowMap"))))
		return E_FAIL;

	if (FAILED(m_pFlowTexture1->Bind_ShaderResource(m_pShader, "g_FlowTexture1")))
		return E_FAIL;
	if (FAILED(m_pFlowTexture2->Bind_ShaderResource(m_pShader, "g_FlowTexture2")))
		return E_FAIL;


	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	CTimer_Manager* pTime = CTimer_Manager::GetInstance();
	Safe_AddRef(pTime);
	m_fFrameTime += pTime->Get_World_Tick();

	Safe_Release(pTime);
	
	if (FAILED(m_pShader->Bind_RawValue("g_FrameTime", &m_fFrameTime, sizeof(_float))))
		return E_FAIL;

	m_fFlowPower = 1.3f;
	if (FAILED(m_pShader->Bind_RawValue("g_fFlowPower", &m_fFlowPower, sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin("Flow");

	m_pBuffer->Render();
	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_FlowMap"))))
		return E_FAIL;


	Safe_Release(pRenderTarget_Manager);
	return S_OK;
}

HRESULT CFlowMap::Add_Components()
{
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_FlowMap.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pBuffer)
		return E_FAIL;
	 
	return S_OK;
}


CFlowMap* CFlowMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag)
{
	CFlowMap* pInstance = New CFlowMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTargetTag)))
	{
		MSG_BOX("Failed to Created CFlowMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlowMap::Free()
{
	__super::Free();

	Safe_Release(m_pFlowTexture1);
	Safe_Release(m_pFlowTexture2);


	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);


}
