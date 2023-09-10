#include "..\Public\PhysXRender.h"
#include "GameInstance.h"
#include "PhysXConverter.h"

CPhysXRender::CPhysXRender(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPhysXRender::CPhysXRender(const CPhysXRender& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPhysXRender::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysXRender::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	m_pScene = pGameInstance->Get_PhysxScene();

	Safe_Release(pGameInstance);

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CPhysXRender::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPhysXRender::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Make_Buffers();

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CPhysXRender::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Debug")))
		return E_FAIL;

	if (FAILED(m_pLine->Render()))
		return E_FAIL;

	if (FAILED(m_pTriangle->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysXRender::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CPhysXRender Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Debug"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CPhysXRender Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	// 버퍼 가져오기 전 갱신
	m_pScene->simulate(1 / 60.f);
	m_pScene->fetchResults(true);
	const PxRenderBuffer* pBuffer = &m_pScene->getRenderBuffer();

	CVIBuffer_Line::LINEDESC LineDesc;
	ZEROMEM(&LineDesc);
	LineDesc.iNum = pBuffer->getNbLines();

	const PxDebugLine* pLines = pBuffer->getLines();
	vector<_float3> Lines;
	for (_uint i = 0; i < LineDesc.iNum; ++i)
	{
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pLines[i].pos0));
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pLines[i].pos1));
	}
	LineDesc.pLines = Lines.data();

	if (0 < LineDesc.iNum &&
		nullptr != LineDesc.pLines)
	{
		/* For.Com_Line */
		if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Line"),
			TEXT("Com_Line"), reinterpret_cast<CComponent**>(&m_pLine), &LineDesc)))
		{
			MSG_BOX("Failed CPhysXRender Add_Component : (Com_Line)");
			return E_FAIL;
		}
	}

	CVIBuffer_Triangle::TRIANGLEDESC TriangleDesc;
	ZEROMEM(&TriangleDesc);
	TriangleDesc.iNum = pBuffer->getNbTriangles();
	const PxDebugTriangle* pDebugTriangles = pBuffer->getTriangles();
	vector<_float3> Triangles;

	for (_uint i = 0; i < TriangleDesc.iNum; ++i)
	{
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos0));
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos1));
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos2));
	}
	TriangleDesc.pTriangles = Triangles.data();
	if (0 < TriangleDesc.iNum &&
		nullptr != TriangleDesc.pTriangles)
	{
		/* For.Com_Triangle */
		if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Triangle"),
			TEXT("Com_Triangle"), reinterpret_cast<CComponent**>(&m_pTriangle), &TriangleDesc)))
		{
			MSG_BOX("Failed CPhysXRender Add_Component : (Com_Triangle)");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CPhysXRender::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float4 vColor = _float4(0, 1, 0, 1);
	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor,sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPhysXRender::Make_Buffers()
{
	const PxRenderBuffer* pBuffer = &m_pScene->getRenderBuffer();

	CVIBuffer_Triangle::TRIANGLEDESC TriangleDesc;
	ZEROMEM(&TriangleDesc);
	TriangleDesc.iNum = pBuffer->getNbTriangles();
	const PxDebugTriangle* pDebugTriangles = pBuffer->getTriangles();
	vector<_float3> Triangles;
	for (_uint i = 0; i < TriangleDesc.iNum; ++i)
	{
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos0));
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos1));
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos2));
	}

	TriangleDesc.pTriangles = Triangles.data();

	m_pTriangle->Tick(TriangleDesc);

	CVIBuffer_Line::LINEDESC LineDesc;
	ZEROMEM(&LineDesc);
	LineDesc.iNum = pBuffer->getNbLines();
	const PxDebugLine* pDebugLines = pBuffer->getLines();
	vector<_float3> Lines;
	for (_uint i = 0; i < LineDesc.iNum; ++i)
	{
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pDebugLines[i].pos0));
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pDebugLines[i].pos1));
	}

	LineDesc.pLines = Lines.data();

	m_pLine->Tick(LineDesc);

	return S_OK;
}

CPhysXRender* CPhysXRender::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPhysXRender* pInstance = New CPhysXRender(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPhysXRender");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPhysXRender::Clone(void* pArg)
{
	CPhysXRender* pInstance = New CPhysXRender(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPhysXRender");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysXRender::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pLine);
	Safe_Release(m_pTriangle);

}