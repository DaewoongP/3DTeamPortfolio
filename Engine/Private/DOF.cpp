#include "DOF.h"
#include "RenderTarget_Manager.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include"Texture.h"
#include"PipeLine.h"

CDOF::CDOF(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent(pDevice, pContext)
{
}

HRESULT CDOF::Initialize_Prototype(const _tchar* pTargetTag, _float LightPower)
{
	lstrcpy(m_szTargetTag, pTargetTag);


	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);




	_uint				iNumViews = { 1 };
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_DOF"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_DOF"), TEXT("Target_DOF"))))
		return E_FAIL;
	

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	Safe_Release(pRenderTarget_Manager);

	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed to Create CDOF Shader");
		__debugbreak;
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CDOF::Render()
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_DOF"))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(m_szTargetTag, m_pShader, "g_PostProcessingTexture")))
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
	m_fFocusDis = 10.f;
	m_fFocusRange = 10.f;
	m_fMaxBlur = 10.f;
	if (FAILED(m_pShader->Bind_RawValue("g_fFocusDis", &m_fFocusDis, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fFocusRange", &m_fFocusRange, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("gMaxBlur", &m_fMaxBlur, sizeof(_float))))
		return E_FAIL;
	

	m_pShader->Begin("DOF");

	m_pBuffer->Render();


	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	Safe_Release(pRenderTarget_Manager);


	return S_OK;
}

HRESULT CDOF::Add_Components()
{
	if(FAILED(m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_DOF.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements)))
	{
		MSG_BOX("Failed Create DOF Shader");
			__debugbreak;
			return E_FAIL;
	}

	if (nullptr == m_pShader)
		return E_FAIL;

	m_pBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pBuffer)
		return E_FAIL;

	return S_OK;
}

CDOF* CDOF::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _float LightPower)
{
	CDOF* pInstance = New CDOF(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTargetTag, LightPower)))
	{
		MSG_BOX("Failed to Created CDOF");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDOF::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
}

