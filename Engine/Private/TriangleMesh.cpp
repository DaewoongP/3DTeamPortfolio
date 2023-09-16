#include "TriangleMesh.h"
#include "PipeLine.h"
#include "PhysX_Manager.h"
#include "PhysXConverter.h"
#include "Mesh.h"

#ifdef _DEBUG
#include "Shader.h"
#include "VIBuffer_Line.h"
#include "VIBuffer_Triangle.h"
#endif // _DEBUG

CTriangleMesh::CTriangleMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CTriangleMesh::CTriangleMesh(const CTriangleMesh& rhs)
	: CComposite(rhs)
{
}

HRESULT CTriangleMesh::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTriangleMesh::Initialize(void* pArg)
{
	if (FAILED(Read_File_MeshCol(static_cast<_tchar*>(pArg))))
		return E_FAIL;

	return S_OK;
}

void CTriangleMesh::Tick(_float fTimeDelta)
{
}

void CTriangleMesh::Late_Tick(_float fTimeDelta)
{

}


HRESULT CTriangleMesh::Read_File_MeshCol(_tchar* pModelFilePath)
{
	HANDLE hFile = CreateFile(pModelFilePath,
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	_ulong	dwStrByte = 0;

	// Meshes NumMeshes
	_uint iNumMeshes = { 0 };
	ReadFile(hFile, &iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		COLMESH Mesh;
		// Mesh NumVertices
		ReadFile(hFile, &(Mesh.iNumVertices), sizeof(_uint), &dwByte, nullptr);

		// Mesh NumFaces
		ReadFile(hFile, &(Mesh.iNumFaces), sizeof(_uint), &dwByte, nullptr);

		Mesh.Faces = new FACE[Mesh.iNumFaces];
		ZeroMemory(Mesh.Faces, sizeof(FACE) * (Mesh.iNumFaces));

		for (_uint j = 0; j < Mesh.iNumFaces; ++j)
		{
			FACE Face;
			ZEROMEM(&Face);

			// Face NumIndices
			ReadFile(hFile, &(Face.iNumIndices), sizeof(_uint), &dwByte, nullptr);

			// Face Indices
			Face.iIndices = new _uint[Face.iNumIndices];
			ZeroMemory(Face.iIndices, sizeof(_uint) * (Face.iNumIndices));
			ReadFile(hFile, Face.iIndices, sizeof(_uint) * (Face.iNumIndices), &dwByte, nullptr);

			Mesh.Faces[j] = Face;
		}

		// Mesh Positions
		Mesh.vPositions = new _float3[Mesh.iNumVertices];
		ZeroMemory(Mesh.vPositions, sizeof(_float3) * (Mesh.iNumVertices));
		ReadFile(hFile, Mesh.vPositions, sizeof(_float3) * (Mesh.iNumVertices), &dwByte, nullptr);
		
		Create_ConvexMeshActor(&Mesh);
		Release_Mesh(&Mesh);

	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CTriangleMesh::Create_ConvexMeshActor(COLMESH* PMesh)
{
	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	PxPhysics* pPhysX = pPhysX_Manager->Get_Physics();
	m_pScene = pPhysX_Manager->Get_PhysxScene();

	//그걸 이용해 정점과 인덱스를 만들어줍니다.
	_uint verticesCount = PMesh->iNumVertices;
	PxVec3* vertices = new PxVec3[verticesCount]{};

	_uint indicesCount = PMesh->iNumFaces*3;
	PxU32* indices = new PxU32[indicesCount]{};

	//vertices에 정점을 채워줍니다.
	for (int i = 0; i < verticesCount; i++)
	{
		memcpy(&vertices[i], &(PMesh->vPositions[i]), sizeof(_float3));
	}

	_uint iIndicesIndex = { 0 };
	for (int i = 0; i < indicesCount/3;i++)
	{
		memcpy(&indices[iIndicesIndex++], &PMesh->Faces[i].iIndices[0], sizeof(PxU32));
		memcpy(&indices[iIndicesIndex++], &PMesh->Faces[i].iIndices[1], sizeof(PxU32));
		memcpy(&indices[iIndicesIndex++], &PMesh->Faces[i].iIndices[2], sizeof(PxU32));
	}

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = verticesCount;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = vertices;

	meshDesc.triangles.count = indicesCount/3;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = indices;
	
	PxTolerancesScale scale;
	PxCookingParams params(scale);
	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
	if (!status)
		return E_FAIL;

	m_pActor = pPhysX->createRigidStatic(PxTransform(PxVec3(0, 0, 0)));
	m_pMaterial = pPhysX->createMaterial(1.f, 0.1f, 0.1f);

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* triangleMesh = pPhysX->createTriangleMesh(readBuffer);

	//PxTriangleMesh* aTriangleMesh = PxCreateTriangleMesh(params, meshDesc,
	//	pPhysX->getPhysicsInsertionCallback());

	PxShape* aTriangleShape = PxRigidActorExt::createExclusiveShape(*m_pActor,
		PxTriangleMeshGeometry(triangleMesh), *m_pMaterial);

	Safe_Delete_Array(vertices);
	Safe_Delete_Array(indices);

	Safe_Release(pPhysX_Manager);
	m_pScene->addActor(*m_pActor);

	return S_OK;
}

HRESULT CTriangleMesh::Release_Mesh(COLMESH* Mesh)
{
	for (_uint j = 0; j < Mesh->iNumFaces; ++j)
	{
		Safe_Delete_Array(Mesh->Faces[j].iIndices);
	}
	Safe_Delete_Array(Mesh->Faces);
	Safe_Delete_Array(Mesh->vPositions);
	return S_OK;
}


CTriangleMesh* CTriangleMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTriangleMesh* pInstance = new CTriangleMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTriangleMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTriangleMesh::Clone(void* pArg)
{
	CTriangleMesh* pInstance = new CTriangleMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTriangleMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTriangleMesh::Free()
{
	__super::Free();
}
