#include "..\Public\MapObject.h"
#include "GameInstance.h"

CMapObject::CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMapObject::CMapObject(const CMapObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMapObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_float4x4* vWorldMatrix = (_float4x4*)pArg;
		m_pTransform->Set_WorldMatrix(*vWorldMatrix);
	}

	return S_OK;
}

void CMapObject::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CMapObject::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_eRenderCount = RT_NORMAL;

		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);

#ifdef _DEBUG
	//	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_PICKING, this);
#endif // _DEBUG		
	}
}

HRESULT CMapObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (nullptr == m_pShader ||
		nullptr == m_pModel)
		return S_OK;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	// 일반 그리기 
	if (RT_NORMAL == m_eRenderCount)
	{
		_uint		iNumMeshes = m_pModel->Get_NumMeshes();

		for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; iMeshCount++)
		{
			m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);

			m_pShader->Begin("Default");

			if (FAILED(m_pModel->Render(iMeshCount)))
				return E_FAIL;
		}

		m_eRenderCount = RT_PICKING;

		return S_OK;
	}

#ifdef _DEBUG
	// 피킹용 그리기
	//else if (RT_PICKING == m_eRenderCount)
	//{
	//	m_pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4));

	//	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	//	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; iMeshCount++)
	//	{
	//		m_pShader->Begin("Picking");

	//		if (FAILED(m_pModel->Render(iMeshCount)))
	//			return E_FAIL;
	//	}

	//	m_eRenderCount = RT_END;
	//}
#endif // _DEBUG

	return S_OK;
}

HRESULT CMapObject::Render_Depth()
{
	if (FAILED(__super::Render_Depth()))
		return E_FAIL;

	if (nullptr == m_pShader ||
		nullptr == m_pModel)
		return S_OK;

	BEGININSTANCE
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	_float4x4	ViewMatrix, ProjMatrix;
	ViewMatrix = XMMatrixLookAtLH(_float4(0.f, 50.f, 0.f, 1.f), _float4(50.f, 0.f, 50.f, 1.f), _float4(0.f, 1.f, 0.f, 0.f));
	ProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), _float(g_iWinSizeX) / g_iWinSizeY, 1.f, 100.f);
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;
	ENDINSTANCE

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; iMeshCount++)
	{
		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);

		m_pShader->Begin("Shadow");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CMapObject::Add_Model_Component(const _tchar* wszModelTag)
{
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, wszModelTag,
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Buffer)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMapObject::Add_Shader_Component(const _tchar* wszShaderTag)
{
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, wszShaderTag,
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Shader)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMapObject::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Renderer)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMapObject::SetUp_ShaderResources()
{
	BEGININSTANCE; if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL; ENDINSTANCE;

	return S_OK;
}

CMapObject* CMapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapObject* pInstance = New CMapObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapObject::Clone(void* pArg)
{
	CMapObject* pInstance = New CMapObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMapObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMapObject::Free()
{
	__super::Free();
	Safe_Release(m_pTransform);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
