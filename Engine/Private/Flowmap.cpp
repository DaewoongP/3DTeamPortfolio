#include "..\Public\Flowmap.h"
#include "GameInstance.h"
#include "RenderTarget_Manager.h"

CFlowmap::CFlowmap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CFlowmap::Initialize(CVIBuffer_Rect* pRectBuffer)
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	_uint				iNumViews = { 1 };
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Flowmap"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Flowmap_Reflection"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Flowmap_Refraction"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_Flowmap"), TEXT("Target_Flowmap"))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_Flowmap_Reflection"), TEXT("Target_Flowmap_Reflection"))))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Add_MRT(TEXT("MRT_Flowmap_Refraction"), TEXT("Target_Flowmap_Refraction"))))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	Safe_Release(pRenderTarget_Manager);

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pBuffer = pRectBuffer;
	Safe_AddRef(m_pBuffer);

	return S_OK;
}

HRESULT CFlowmap::Render()
{
	// Height보다 밑에있는 애들을 렌더타겟으로 찍음
	if (FAILED(Render_Flowmap()))
		return E_FAIL;
	
	if (FAILED(Render_Reflection()))
		return E_FAIL;
	
	if (FAILED(Render_Refraction()))
		return E_FAIL;


	return S_OK;
}

HRESULT CFlowmap::Render_Flowmap()
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Flowmap"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pPipeLine->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	Safe_Release(pPipeLine);
	
	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fWaterHeight", &m_fHeight, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Flowmap")))
		return E_FAIL;

	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Flowmap"))))
		return E_FAIL;

	Safe_Release(pRenderTarget_Manager);

	return S_OK;
}

HRESULT CFlowmap::Render_Reflection()
{
	// 이거 하려면 reflection 렌더타겟으로 처리해야함.
	// view행렬 다른거 더받아서 client shader 단에서 더 넘겨줘야함.
	if (FAILED(Calculate_ReflectionViewMatrix()))
		return E_FAIL;

	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	if (FAILED(pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Flowmap_Reflection"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ReflectionViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Reflection")))
		return E_FAIL;

	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	if (FAILED(pRenderTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Flowmap_Reflection"))))
		return E_FAIL;

	Safe_Release(pRenderTarget_Manager);

	return S_OK;
}

HRESULT CFlowmap::Render_Refraction()
{
	CRenderTarget_Manager* pRenderTarget_Manager = CRenderTarget_Manager::GetInstance();
	Safe_AddRef(pRenderTarget_Manager);

	Safe_Release(pRenderTarget_Manager);

	return S_OK;
}

HRESULT CFlowmap::Add_Components()
{
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Flowmap.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Liquid/VFX_T_WaterTurbulentTileFreq_01_D.png"));
	if (nullptr == m_pTexture)
		return E_FAIL;

	return S_OK;
}

HRESULT CFlowmap::Calculate_ReflectionViewMatrix()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float3 vCamPos = pPipeLine->Get_CamPosition()->xyz();
	_float3 vCamLook = *pPipeLine->Get_CamLook();
	// x,z대입
	_float3 vReflectionPos = vCamPos;
	// y값 계산
	vReflectionPos.y = -1.f * vCamPos.y + 2.f * m_fHeight;

	_float4 vWaterPlane = XMPlaneFromPointNormal(_float3(0.f, m_fHeight, 0.f), _float3(0.f, 1.f, 0.f));

	// 적당히 큰 값 줘서 intersect 처리로 위치벡터 구하기
	_float fMaxLine = *pPipeLine->Get_CamFar();
	_float3 vWaterPos = XMPlaneIntersectLine(vWaterPlane, vCamPos, vCamPos + fMaxLine * vCamLook);

	Safe_Release(pPipeLine);

	_float3 vReflectionLook = vWaterPos - vReflectionPos;
	
	m_ReflectionViewMatrix = XMMatrixLookAtLH(vReflectionPos, vReflectionPos + vReflectionLook, _float3(0.f, 1.f, 0.f));

	return S_OK;
}

CFlowmap* CFlowmap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CVIBuffer_Rect* pRectBuffer)
{
	CFlowmap* pInstance = New CFlowmap(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pRectBuffer)))
	{
		MSG_BOX("Failed to Created CFlowmap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlowmap::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pBuffer);
}
