#include "..\Public\Plane.h"
#include "PipeLine.h"
#include "PhysX_Manager.h"
#include "PhysXConverter.h"

#ifdef _DEBUG
#include "Shader.h"
#include "VIBuffer_Line.h"
#include "VIBuffer_Triangle.h"
#endif // _DEBUG

CPlane::CPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CPlane::CPlane(const CPlane& rhs)
	: CComposite(rhs)
#ifdef _DEBUG
	, m_vColor(_float4(0.f, 1.f, 0.f, 1.f))
#endif // _DEBUG
{
}

HRESULT CPlane::Initialize_Prototype(const _tchar* szHeightMapPath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Create_HeightMap(szHeightMapPath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlane::Initialize(void* pArg)
{
	if (FAILED(Create_PlaneActor()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Add_Components()))
		return E_FAIL;
#endif // DEBUG

	return S_OK;
}

void CPlane::Tick(_float fTimeDelta)
{
}

void CPlane::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	Make_Buffers();
#endif // _DEBUG
}

#ifdef _DEBUG
HRESULT CPlane::Render()
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
#endif // _DEBUG

HRESULT CPlane::Create_HeightMap(const _tchar* szHeightMapPath)
{
	
	return S_OK;
}

HRESULT CPlane::Create_PlaneActor()
{
	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	PxPhysics* pPhysX = pPhysX_Manager->Get_Physics();
	m_pScene = pPhysX_Manager->Get_PhysxScene();

#ifdef _DEBUG
	m_pScene->simulate(1 / 60.f);
	m_pScene->fetchResults(true);

	const PxRenderBuffer* pBuffer = pPhysX_Manager->Get_RenderBuffer();
	m_iStartLineBufferIndex = pBuffer->getNbLines();
	m_iStartTriangleBufferIndex = pBuffer->getNbTriangles();
#endif // _DEBUG

	Safe_Release(pPhysX_Manager);
	_int numCols = 100;
	_int numRows = 50;

	PxHeightFieldSample* samples = new PxHeightFieldSample[sizeof(PxHeightFieldSample) * (numCols * numRows)];
	_uint index = 0;
	for (_uint i = 0; i < numCols * numRows; ++i)
	{
		samples[i].height = Random_Generator<_float>(0.f, 2.f);
	}


	PxHeightFieldDesc hfDesc;
	hfDesc.format = PxHeightFieldFormat::eS16_TM;
	hfDesc.nbColumns = numCols;
	hfDesc.nbRows = numRows;
	hfDesc.samples.data = samples;
	hfDesc.samples.stride = sizeof(PxHeightFieldSample);

	PxHeightField* aHeightField = PxCreateHeightField(hfDesc,
		pPhysX->getPhysicsInsertionCallback());

	_float heightScale = 1.f;
	_float rowScale = 1.f;
	_float colScale = 1.f;
	PxHeightFieldGeometry hfGeom(aHeightField, PxMeshGeometryFlags(), heightScale, rowScale,
		colScale);
	PxVec3 vLocal = PxVec3(0.f, 0.f, 0.f);
	PxTransform localTm(vLocal);
	m_pActor = pPhysX->createRigidStatic(localTm);
	m_pMaterial = pPhysX->createMaterial(1.f, 0.1f, 0.1f);

	PxShape* aHeightFieldShape = pPhysX->createShape(hfGeom, *m_pMaterial, PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE);
#ifdef _DEBUG
	PxShape* pPlaneShape = pPhysX->createShape(PxPlaneGeometry(), *m_pMaterial, false, PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE);
#else
	PxShape* pPlaneShape = pPhysX->createShape(PxPlaneGeometry(), *m_pMaterial, false, PxShapeFlag::eSIMULATION_SHAPE);
#endif // _DEBUG
	Safe_Delete_Array(samples);

	m_pActor->attachShape(*aHeightFieldShape);
	m_pScene->addActor(*m_pActor);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CPlane::Add_Components()
{
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Debug.hlsl"), VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
	m_Components.emplace(TEXT("Com_Shader"), m_pShader);
	Safe_AddRef(m_pShader);

	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	// Plane 생성이후 개수 확인
	m_pScene->simulate(1 / 60.f);
	m_pScene->fetchResults(true);
	const PxRenderBuffer* pBuffer = pPhysX_Manager->Get_RenderBuffer();
	m_iNumLineBuffer = pBuffer->getNbLines() - m_iStartLineBufferIndex;
	m_iNumTriangleBuffer = pBuffer->getNbTriangles() - m_iStartTriangleBufferIndex;

	CVIBuffer_Line::LINEDESC LineDesc;
	ZEROMEM(&LineDesc);

	LineDesc.iNum = m_iNumLineBuffer;
	const PxDebugLine* pLines = pBuffer->getLines();

	vector<_float3> Lines;
	for (_uint i = pBuffer->getNbLines() - m_iNumLineBuffer + 1; i < pBuffer->getNbLines(); ++i)
	{
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pLines[i].pos0));
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pLines[i].pos1));
	}
	LineDesc.pLines = Lines.data();

	if (0 < LineDesc.iNum &&
		nullptr != LineDesc.pLines)
	{
		/* For.Com_Line */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_VIBuffer_Line"),
			TEXT("Com_Line"), reinterpret_cast<CComponent**>(&m_pLine), &LineDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Line)");
			return E_FAIL;
		}
	}

	CVIBuffer_Triangle::TRIANGLEDESC TriangleDesc;
	ZEROMEM(&TriangleDesc);

	TriangleDesc.iNum = m_iNumTriangleBuffer;
	const PxDebugTriangle* pDebugTriangles = pBuffer->getTriangles();

	vector<_float3> Triangles;
	for (_uint i = pBuffer->getNbTriangles() - m_iNumTriangleBuffer + 1; i < pBuffer->getNbTriangles(); ++i)
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
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_VIBuffer_Triangle"),
			TEXT("Com_Triangle"), reinterpret_cast<CComponent**>(&m_pTriangle), &TriangleDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Triangle)");
			return E_FAIL;
		}
	}

	pPhysX_Manager->Clear_BufferIndex();
	Safe_Release(pPhysX_Manager);

	return S_OK;
}

HRESULT CPlane::SetUp_ShaderResources()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float4x4 WorldMatrix = XMMatrixIdentity();

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof(m_vColor))))
		return E_FAIL;

	Safe_Release(pPipeLine);
	return S_OK;
}

void CPlane::Make_Buffers()
{
	CVIBuffer_Line::LINEDESC LineDesc;
	ZEROMEM(&LineDesc);

	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	const PxRenderBuffer* pBuffer = pPhysX_Manager->Get_RenderBuffer();

	Safe_Release(pPhysX_Manager);

	const PxDebugLine* pDebugLines = pBuffer->getLines();
	_uint iNumLines = pBuffer->getNbLines();
	vector<_float3> Lines;
	for (_uint i = iNumLines - m_iNumLineBuffer; i < iNumLines; ++i)
	{
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pDebugLines[i].pos0));
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pDebugLines[i].pos1));
	}

	LineDesc.iNum = m_iNumLineBuffer;
	LineDesc.pLines = Lines.data();

	m_pLine->Tick(LineDesc);

	CVIBuffer_Triangle::TRIANGLEDESC TriangleDesc;
	ZEROMEM(&TriangleDesc);

	const PxDebugTriangle* pDebugTriangles = pBuffer->getTriangles();
	_uint iNumTriangles = pBuffer->getNbTriangles();
	vector<_float3> Triangles;
	for (_uint i = iNumTriangles - m_iNumTriangleBuffer; i < iNumTriangles; ++i)
	{
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos0));
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos1));
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos2));
	}
	TriangleDesc.iNum = m_iNumTriangleBuffer;
	TriangleDesc.pTriangles = Triangles.data();

	m_pTriangle->Tick(TriangleDesc);
}
#endif // _DEBUG

CPlane* CPlane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* szHeightMapPath)
{
	CPlane* pInstance = new CPlane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(szHeightMapPath)))
	{
		MSG_BOX("Failed to Created CPlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CPlane::Clone(void* pArg)
{
	CPlane* pInstance = new CPlane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlane::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pTriangle);
	Safe_Release(m_pShader);
	Safe_Release(m_pLine);
#endif // _DEBUG
}
