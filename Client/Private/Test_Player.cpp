#include "..\Public\Test_Player.h"
#include "GameInstance.h"
#include "PhysXConverter.h"

CTest_Player::CTest_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTest_Player::CTest_Player(const CTest_Player& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTest_Player::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTest_Player::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Speed(10.f);

	return S_OK;
}

void CTest_Player::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pScene = pGameInstance->Get_PhysxScene();
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

	__super::Tick(fTimeDelta);
}

void CTest_Player::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	m_ConvMatrix = XMMatrixTranspose(XMMatrixInverse(0, m_pTransform->Get_WorldMatrix()));
	

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

void CTest_Player::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
}

void CTest_Player::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
}

void CTest_Player::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
}

HRESULT CTest_Player::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE);
		
		m_pShaderCom->Begin("AnimMesh");

		m_pModelCom->Render(i);
	}


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pDebugShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pDebugShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pDebugShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	m_pDebugShader->Begin("Debug");

	m_pLine->Render();

	m_pTriangle->Render();

	return S_OK;
}

HRESULT CTest_Player::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Fiona"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_Model)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pScene = pGameInstance->Get_PhysxScene();
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
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Line"),
			TEXT("Com_Line"), reinterpret_cast<CComponent**>(&m_pLine), &LineDesc)))
		{
			MSG_BOX("Failed CTest_Player Add_Component : (Com_Line)");
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
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Triangle"),
			TEXT("Com_Triangle"), reinterpret_cast<CComponent**>(&m_pTriangle), &TriangleDesc)))
		{
			MSG_BOX("Failed CTest_Player Add_Component : (Com_Triangle)");
			return E_FAIL;
		}
	}

	/* Com_DebugShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Debug"),
		TEXT("Com_DebugShader"), reinterpret_cast<CComponent**>(&m_pDebugShader))))
	{
		MSG_BOX("Failed CTerrain Add_Component : (Com_DebugShader)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTest_Player::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

XMMATRIX CTest_Player::ConvertLHtoRH(const XMMATRIX& lhMatrix)
{
	XMMATRIX rhMatrix = lhMatrix;

	// Negate the Z-axis (third row/column)
	rhMatrix.r[2] = XMVectorNegate(rhMatrix.r[2]);

	// You may need to adjust the W-axis if necessary
	// rhMatrix.r[3] = XMVectorNegate(rhMatrix.r[3]);

	return rhMatrix;
}

CTest_Player* CTest_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTest_Player* pInstance = new CTest_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTest_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTest_Player::Clone(void* pArg)
{
	CTest_Player* pInstance = new CTest_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTest_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTest_Player::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pDebugShader);
	Safe_Release(m_pTriangle);
	Safe_Release(m_pLine);
}
