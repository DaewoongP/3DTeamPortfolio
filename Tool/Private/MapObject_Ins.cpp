#include "..\Public\MapObject_Ins.h"
#include "GameInstance.h"

CMapObject_Ins::CMapObject_Ins(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMapObject_Ins::CMapObject_Ins(const CMapObject_Ins& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMapObject_Ins::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject_Ins::Initialize(void* pArg)
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

void CMapObject_Ins::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CMapObject_Ins::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_eRenderCount = RT_NORMAL;

		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_PICKING, this);
#endif // _DEBUG
	}
}

HRESULT CMapObject_Ins::Render()
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
	else if (RT_PICKING == m_eRenderCount)
	{
		m_pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4));

		_uint		iNumMeshes = m_pModel->Get_NumMeshes();

		for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; iMeshCount++)
		{
			m_pShader->Begin("Picking");

			if (FAILED(m_pModel->Render(iMeshCount)))
				return E_FAIL;
		}

		m_eRenderCount = RT_END;
	}
#endif // _DEBUG	

	return S_OK;
}

HRESULT CMapObject_Ins::Add_Model_Component(const _tchar* wszModelTag)
{
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, wszModelTag,
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Buffer)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMapObject_Ins::Add_Shader_Component(const _tchar* wszShaderTag)
{
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, wszShaderTag,
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Shader)");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMapObject_Ins::Add_Components()
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

HRESULT CMapObject_Ins::SetUp_ShaderResources()
{
	_float4x4 vMat = m_pTransform->Get_WorldMatrix();

	BEGININSTANCE; if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL; ENDINSTANCE;

	return S_OK;
}

CMapObject_Ins* CMapObject_Ins::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapObject_Ins* pInstance = New CMapObject_Ins(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMapObject_Ins");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapObject_Ins::Clone(void* pArg)
{
	CMapObject_Ins* pInstance = New CMapObject_Ins(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMapObject_Ins");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMapObject_Ins::Free()
{
	__super::Free();
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
