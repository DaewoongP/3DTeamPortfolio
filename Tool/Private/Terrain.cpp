#include "..\Public\Terrain.h"
#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject(rhs)
{
	ZEROMEM(m_vBrushPos);
	ZEROMEM(m_fBrushRange);
	ZEROMEM(m_iBrushIndex);
}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_isRendering = true;

	return S_OK;
}

void CTerrain::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//m_pBuffer->Culling(m_pTransform->Get_WorldMatrix());

	if (nullptr != m_pRenderer)
	{
		m_eRenderCount = RT_NORMAL;

		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_BRUSHING, this);
#endif // _DEBUG		
	}		
}

HRESULT CTerrain::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	// 쉐이더에 브러쉬 위치값 던져줌
	if (FAILED(SetUp_ShaderDynamicResources()))
		return E_FAIL;

	// 일반 그리기
	if (RT_NORMAL == m_eRenderCount)
	{
		m_pShader->Begin("Terrain_Brush");

		if (FAILED(m_pBuffer->Render()))
			return E_FAIL;

		m_eRenderCount = RT_BRUSHING;
	}

#ifdef _DEBUG
	// 브러싱용 그리기
	else if (RT_BRUSHING == m_eRenderCount)
	{
		BEGININSTANCE;

		_uint				iNumViews = { 1 };
		D3D11_VIEWPORT		ViewportDesc;

		m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

		_float4x4 WorldMatrix, ViewMatrix, ProjMatrix;

		_float3 vEye = { (m_pBuffer->Get_TerrainSizeX() * 0.5f) + 1,
			250.f, (m_pBuffer->Get_TerrainSizeZ() * 0.5f) + 1 };
		_float3 vAt = { (m_pBuffer->Get_TerrainSizeX() * 0.5f) + 1,
			0.f, (m_pBuffer->Get_TerrainSizeZ() * 0.5f) + 1 };
		_float3 vUp = { 0.f, 0.f, 1.f };

		ViewMatrix = XMMatrixLookAtLH(vEye, vAt, vUp);

		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;

		m_pShader->Begin("Terrain_Brush");

		if (FAILED(m_pBuffer->Render()))
			return E_FAIL;

		m_eRenderCount = RT_END;

		ENDINSTANCE;
	}
#endif // _DEBUG

	return S_OK;
}

HRESULT CTerrain::Picking_On_Terrain(_Inout_ _float4* pPickPosition)
{
	_float4 vRayPos = { 0.f, 0.f, 0.f, 1.f };
	_float4 vRayDir = { 0.f, 0.f, 0.f, 0.f };

	BEGININSTANCE;
	pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vRayPos, &vRayDir);
	ENDINSTANCE;

	_float fDist = FLT_MAX; // 피킹 연산 후 최종 거리값

	_bool bResult = m_pBuffer->IsPicked(vRayPos, vRayDir, fDist);

	// 결과가 나올 경우 RayDir에 거리값을 곱해 최종 위치 산출
	if (true == bResult)
	{
		if (FLT_MAX > fDist)
		{
			_float4 vFinalPos;

			vRayDir *= fDist;
			vFinalPos = vRayPos + vRayDir;
			memcpy(pPickPosition, &vFinalPos, sizeof(_float4));

			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CTerrain::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CTerrain Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Terrain"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CTerrain Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* Com_Buffer */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer))))
	{
		MSG_BOX("Failed CTerrain Add_Component : (Com_Buffer)");
		return E_FAIL;
	}

	/* Com_Texture */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Ground"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTexture))))
	{
		MSG_BOX("Failed CTerrain Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	ENDINSTANCE;

	if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "g_DiffuseTexture", m_iDiffuseTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pTexture->Bind_ShaderResources(m_pShader, "g_BrushTexture")))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderDynamicResources()
{
	// 현재 브러쉬 커서의 텍스처 번호
	if (FAILED(m_pShader->Bind_RawValue("g_iBrushTextureIndex", &m_iBrushTextureIndex, sizeof(_uint))))
		return E_FAIL;

	// 현재 브러쉬 커서의 위치
	if (FAILED(m_pShader->Bind_RawValue("g_vBrushCurrentPos", &m_vBrushCurrentPos, sizeof(_float3))))
		return E_FAIL;

	// 현재 브러쉬 커서의 범위
	if (FAILED(m_pShader->Bind_RawValue("g_fBrushCurrentRange", &m_fBrushCurrentRange, sizeof(_float))))
		return E_FAIL;

	// 쉐이더로 던져줄 브러쉬 위치의 개수
	if (FAILED(m_pShader->Bind_RawValue("g_iBrushPosCnt", &m_iBrushPosCnt, sizeof(_uint))))
		return E_FAIL;

	// 쉐이더로 던져줄 브러쉬 위치 
	if(FAILED(m_pShader->Bind_Vectors("g_vBrushPos", m_vBrushPos, MAX_SHADERVECTOR)))
		return E_FAIL;

	// 쉐이더로 던져줄 브러쉬 범위
	if (FAILED(m_pShader->Bind_FloatValues("g_fBrushRange", m_fBrushRange, MAX_SHADERVECTOR)))
		return E_FAIL;

	// 쉐이더로 던져줄 브러쉬 범위
	if (FAILED(m_pShader->Bind_IntValues("g_iBrushIndex", m_iBrushIndex, MAX_SHADERVECTOR)))
		return E_FAIL;

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = New CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = New CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pTexture);
	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pRenderer);
}
