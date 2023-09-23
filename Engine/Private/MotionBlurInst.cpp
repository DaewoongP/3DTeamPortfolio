#include "MotionBlurInst.h"
#include "GameInstance.h"
#include "RenderTarget_Manager.h"

CMotionBlurInstance::CMotionBlurInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMotionBlurInstance::CMotionBlurInstance(const CMotionBlurInstance& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_WorldMatrix(rhs.m_WorldMatrix)
	, m_ViewMatrix(rhs.m_ViewMatrix)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
}

HRESULT CMotionBlurInstance::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL; 

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	m_iLevel = iLevel;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VelocityMapInst.hlsl"),
		VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	m_pBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);

	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	_uint				iNumViews = { 1 };
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Velocity"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Fin_MontionBlurInstance"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;          

	if (FAILED(pRenderTarget_Manager->Ready_Debug(TEXT("Target_Velocity"), 240.f, 560.f, 160.f, 160.f)))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Ready_Debug(TEXT("Target_Fin_MontionBlurInstance"), 400.f, 80.f, 160.f, 160.f)))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_Velocity"), TEXT("Target_Velocity"))))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_Fin_MontionBlurInstance"), TEXT("Target_Fin_MontionBlurInstance"))))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	Safe_Release(pRenderTarget_Manager);
	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CMotionBlurInstance::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMotionBlurInstance::Render()
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Fin_MontionBlurInstance"))))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Velocity"), m_pShader, "g_VelocityTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pShader->Begin("Velocity");

	m_pBuffer->Render();

	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	// RawValues
	Safe_Release(pRenderTarget_Manager);

	return S_OK;
}

HRESULT CMotionBlurInstance::Begin_MRT()
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Velocity"))))
	{
		Safe_Release(pRenderTarget_Manager);
		return E_FAIL;
	}

	Safe_Release(pRenderTarget_Manager);
}

HRESULT CMotionBlurInstance::End_MRT()
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);
	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext)))
	{
		Safe_Release(pRenderTarget_Manager);
		return E_FAIL;
	}

	Safe_Release(pRenderTarget_Manager);
	return S_OK;
}

CMotionBlurInstance* CMotionBlurInstance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CMotionBlurInstance* pInstance = New CMotionBlurInstance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CMotionBlurInstance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMotionBlurInstance::Clone(void* pArg)
{
	AddRef();

	return this;
}

void CMotionBlurInstance::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
}