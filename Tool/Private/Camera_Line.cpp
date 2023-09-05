#include "Camera_Line.h"
#include "GameInstance.h"

CCamera_Line::CCamera_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice,pContext)
{
}

CCamera_Line::CCamera_Line(const CCamera_Line& rhs)
	: CGameObject(rhs)
{
}


HRESULT CCamera_Line::Initialize_Prototype()
{
	CGameObject::Initialize_Prototype();
	return S_OK;
}

HRESULT CCamera_Line::Initialize(void* pArg)
{
	CGameObject::Initialize(pArg);

	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Ready Components");
		return E_FAIL;
	}

	return S_OK;
}

void CCamera_Line::Tick(_float _TimeDelta)
{
#ifdef _DEBUG

#endif // _DEBUG
}

void CCamera_Line::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	//라인 버퍼를 채웠다면
	if (nullptr != m_pLineBuffer)
	{
		m_pRenderer->Add_DebugGroup(this);
	}
#endif // _DEBUG
}

HRESULT CCamera_Line::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Debug")))
		return E_FAIL;

	if (FAILED(m_pLineBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Line::LineBufferInitialize(void* _pArg)
{
	CVIBuffer_Line::LINEDESC* LineDesc = (CVIBuffer_Line::LINEDESC*)_pArg;
	if (0 < LineDesc->iNum &&
		nullptr != LineDesc->pLines)
	{
		/* For.Com_Line */
		if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Line"),
			TEXT("Com_Line"), reinterpret_cast<CComponent**>(&m_pLineBuffer), LineDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Line)");
			return E_FAIL;
		}

		m_pLineBuffer->Tick(*LineDesc);
	}
}

void CCamera_Line::LineBufferClear()
{
	Safe_Release(m_pLineBuffer);
	m_pLineBuffer = nullptr;
}

HRESULT CCamera_Line::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CCamera_Line Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Debug"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CCamera_Line Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCamera_Line::SetUp_ShaderResources()
{
	BEGININSTANCE;
	
	_float4x4 WorldMatrix = XMMatrixIdentity();

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	ENDINSTANCE;

	return S_OK;
}

#ifdef _DEBUG

#endif // _DEBUG

CCamera_Line* CCamera_Line::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Line* pInstance = New CCamera_Line(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCamera_Line");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_Line::Clone(void* pArg)
{
	CCamera_Line* pInstance = New CCamera_Line(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCamera_Line");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Line::Free()
{
	CGameObject::Free();
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pLineBuffer);
}
