#include "ConvexMesh.h"
#include "PipeLine.h"
#include "PhysX_Manager.h"
#include "PhysXConverter.h"
#include "Mesh.h"

CConvexMesh::CConvexMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CConvexMesh::CConvexMesh(const CConvexMesh& rhs)
	: CComposite(rhs)
{
}

HRESULT CConvexMesh::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CConvexMesh::Initialize(void* pArg)
{
	if (FAILED(Create_ConvexMeshActor(pArg)))
		return E_FAIL;

	return S_OK;
}

void CConvexMesh::Tick(_float fTimeDelta)
{
}

void CConvexMesh::Late_Tick(_float fTimeDelta)
{
}

HRESULT CConvexMesh::Create_ConvexMeshActor(void* pArg)
{
	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	PxPhysics* pPhysX = pPhysX_Manager->Get_Physics();
	m_pScene = pPhysX_Manager->Get_PhysxScene();

	CMesh* mesh = static_cast<CMesh*>(pArg);
	if (mesh != nullptr)
	{
		// mesh에서 정점을 가져옵니다.
		vector<_float3>*  meshVertices = mesh->Get_VerticesPositionVec();
		vector<PxU32>* meshIndices = mesh->Get_IndicesVec();
		//그걸 이용해 정점을 만들어줍니다.
		_uint verticesCount = mesh->Get_NumVertices();
		PxVec3* vertices = new PxVec3[verticesCount]{};
		_uint indicesCount = meshIndices->size();
		PxU32* indices = new PxU32[indicesCount]{};
		//vertices에 정점을 채워줍니다.
		for (int i = 0; i < verticesCount; i++)
		{
			memcpy(&vertices[i], &(*meshVertices)[i], sizeof(_float3));
		}
		for (int i = 0; i < indicesCount; i++)
		{
			memcpy(&indices[i], &(*meshIndices)[i], sizeof(PxU32));
		}
		//기본
		{
			////Convex Hull을 만들어줍니다.
			//PxConvexMeshDesc convexDesc = {};
			//convexDesc.points.data = vertices;
			//convexDesc.points.count = verticesCount;
			//convexDesc.points.stride = sizeof(PxVec3); // 정점 간격 설정
			//convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

			//PxTolerancesScale scale;
			//PxCookingParams params(scale);

			//PxDefaultMemoryOutputStream buf;
			//PxConvexMeshCookingResult::Enum result;
			//if (!PxCookConvexMesh(params, convexDesc, buf, &result))
			//	return NULL;
			//m_pActor = pPhysX->createRigidStatic(PxTransform(PxVec3(0, 0, 0)));
			//m_pMaterial = pPhysX->createMaterial(1.f, 0.1f, 0.1f);

			//PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
			//PxConvexMesh* convexMesh = pPhysX->createConvexMesh(input);

			//PxShape* aConvexShape = PxRigidActorExt::createExclusiveShape(*m_pActor,
			//	PxConvexMeshGeometry(convexMesh), *m_pMaterial);
		}
		//
		{
			PxHullPolygon hullPolygons[10]; // 1개의 폴리곤을 가진 Convex Hull

			// 폴리곤의 정점 인덱스 설정
			hullPolygons[0].mIndexBase = verticesCount / 10*0;
			hullPolygons[1].mIndexBase = verticesCount / 10*1;
			hullPolygons[2].mIndexBase = verticesCount / 10*2;
			hullPolygons[3].mIndexBase = verticesCount / 10*3;
			hullPolygons[4].mIndexBase = verticesCount / 10*4;
			hullPolygons[5].mIndexBase = verticesCount / 10*5;
			hullPolygons[6].mIndexBase = verticesCount / 10*6;
			hullPolygons[7].mIndexBase = verticesCount / 10*7;
			hullPolygons[8].mIndexBase = verticesCount / 10*8;
			hullPolygons[9].mIndexBase = verticesCount / 10*9;

			hullPolygons[0].mNbVerts = verticesCount / 10;
			hullPolygons[1].mNbVerts = verticesCount / 10;
			hullPolygons[2].mNbVerts = verticesCount / 10;
			hullPolygons[3].mNbVerts = verticesCount / 10;
			hullPolygons[4].mNbVerts = verticesCount / 10;
			hullPolygons[5].mNbVerts = verticesCount / 10;
			hullPolygons[6].mNbVerts = verticesCount / 10;
			hullPolygons[7].mNbVerts = verticesCount / 10;
			hullPolygons[8].mNbVerts = verticesCount / 10; // 3개의 정점을 가지는 폴리곤
			hullPolygons[9].mNbVerts = verticesCount / 10;

			PxConvexMeshDesc convexDesc;
			convexDesc.points.count = verticesCount;
			convexDesc.points.stride = sizeof(PxVec3);
			convexDesc.points.data = vertices;
			
			//convexDesc.polygons.count = 1; // 사용하는 폴리곤 수에 맞게 조정
			//convexDesc.polygons.stride = sizeof(PxHullPolygon);
			//convexDesc.polygons.data = hullPolygons;
			convexDesc.flags =// PxConvexFlag::eCOMPUTE_CONVEX;;
				PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eDISABLE_MESH_VALIDATION | PxConvexFlag::eFAST_INERTIA_COMPUTATION;

			PxTolerancesScale scale;
			PxCookingParams cookingParams(scale);

			cookingParams.meshWeldTolerance = 0.000000001f;
			cookingParams.convexMeshCookingType = PxConvexMeshCookingType::eQUICKHULL;


			PxDefaultMemoryOutputStream buf;
			PxConvexMeshCookingResult::Enum result;

			m_pActor = pPhysX->createRigidStatic(PxTransform(PxVec3(0, 0, 0)));
			m_pMaterial = pPhysX->createMaterial(1.f, 0.1f, 0.1f);

			PxDefaultMemoryInputData input(buf.getData(), buf.getSize());

			PxConvexMesh* aConvexMesh = PxCreateConvexMesh(cookingParams, convexDesc,
				pPhysX->getPhysicsInsertionCallback());

			PxShape* aConvexShape = PxRigidActorExt::createExclusiveShape(*m_pActor,
				PxConvexMeshGeometry(aConvexMesh), *m_pMaterial);
		}
	}

#ifdef _DEBUG
	m_pScene->simulate(1 / 60.f);
	m_pScene->fetchResults(true);

#endif // _DEBUG

	Safe_Release(pPhysX_Manager);

	
#ifdef _DEBUG
	PxShape* pPlaneShape = pPhysX->createShape(PxPlaneGeometry(), *m_pMaterial, false, PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE);
#else
	PxShape* pPlaneShape = pPhysX->createShape(PxPlaneGeometry(), *m_pMaterial, false, PxShapeFlag::eSIMULATION_SHAPE);
#endif // _DEBUG

	m_pActor->attachShape(*pPlaneShape);
	m_pScene->addActor(*m_pActor);

	return S_OK;
}

CConvexMesh* CConvexMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CConvexMesh* pInstance = new CConvexMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CConvexMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CConvexMesh::Clone(void* pArg)
{
	CConvexMesh* pInstance = new CConvexMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CConvexMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CConvexMesh::Free()
{
	__super::Free();
}
