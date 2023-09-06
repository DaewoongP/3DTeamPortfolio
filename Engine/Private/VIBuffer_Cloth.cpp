#include "../Public/VIBuffer_Cloth.h"
#include "PhysX_Manager.h"
#include "PhysXConverter.h"

CVIBuffer_Cloth::CVIBuffer_Cloth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Cloth::CVIBuffer_Cloth(const CVIBuffer_Cloth& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Cloth::Initialize(void* pArg)
{
	m_iNumVerticesX = 20;
	m_iNumVerticesZ = 20;

	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSTEX) };
	m_iNumVertices = { m_iNumVerticesX * m_iNumVerticesZ };
	m_iIndexStride = { sizeof(_ulong) };
	m_iNumIndices = { (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3 };
	m_eFormat = { DXGI_FORMAT_R32_UINT };
	m_eTopology = { D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

#pragma region VERTEX_BUFFER

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			// 일단 체크용 x,y에 띄워서 처리
			pVertices[iIndex].vPosition = _float3(j - m_iNumVerticesX / 2.f, i - m_iNumVerticesZ / 2.f, 0.f);
			pVertices[iIndex].vTexCoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}

#pragma endregion


#pragma region INDEX_BUFFER

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];
		}
	}

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	m_Vertices.resize(m_iNumVertices);
	m_Indices.resize(m_iNumIndices);

	memcpy(m_Vertices.data(), pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);
	memcpy(m_Indices.data(), pIndices, sizeof(_ulong) * m_iNumIndices);

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

#pragma endregion

	if (FAILED(Initialize_ClothMesh()))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Cloth::Tick(_float fTimeDelta)
{
	m_pCloth->setWindVelocity(PxVec3(10.f, 0.f, -10.f));
	m_pSolver->beginSimulation(fTimeDelta);

	for (_int i = 0; i < m_pSolver->getSimulationChunkCount(); ++i)
	{
		m_pSolver->simulateChunk(i);
	}

	// 동적버퍼 map 전에 미리 값을 옮겨둠.
	cloth::MappedRange<PxVec4>	Particles = m_pCloth->getCurrentParticles();

	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;
	
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXPOSTEX* pVertices = static_cast<VTXPOSTEX*>(MappedSubResource.pData);

	for (_int i = 0; i < Particles.size(); ++i)
	{
		// xyz = 현재 포지션
		// w = 질량 역수 // 나중에 사용할 예정.
		pVertices[i].vPosition = PhysXConverter::ToXMFLOAT3(Particles[i].getXYZ());
	}

	m_pContext->Unmap(m_pVB, 0);

	cout << Particles[0].getXYZ().x << " " << Particles[0].getXYZ().y << " " << Particles[0].getXYZ().z << endl;

	m_pSolver->endSimulation();
}

HRESULT CVIBuffer_Cloth::Initialize_ClothMesh()
{
	nv::cloth::ClothMeshDesc MeshDesc;
	MeshDesc.setToDefault();
	MeshDesc.points.data = m_Vertices.data();
	MeshDesc.points.stride = sizeof(VTXPOSTEX);
	MeshDesc.points.count = m_iNumVertices;


	MeshDesc.flags = 0; // 32bit
	MeshDesc.triangles.data = m_Indices.data();
	MeshDesc.triangles.stride = sizeof(_ulong) * 3;
	MeshDesc.triangles.count = m_iNumIndices / 3;

	for (_uint i = 0; i < m_iNumVertices; ++i)
		m_InvMasses.push_back(0.5f);
	m_InvMasses[0] = 0.f;
	MeshDesc.invMasses.data = m_InvMasses.data();
	MeshDesc.invMasses.stride = sizeof(_float);
	MeshDesc.invMasses.count = m_iNumVertices;

	if (false == MeshDesc.isValid())
	{
		MSG_BOX("Failed to Create Cloth MeshDesc");
		return E_FAIL;
	}

	PxVec3 vGravity(0.f, 0.f, 1.f);
	cloth::Vector<_int>::Type PhaseTypeInfo;
	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);
	cloth::Factory* pClothFactory = pPhysX_Manager->Get_ClothFactory();
	m_pFabric = NvClothCookFabricFromMesh(pClothFactory, MeshDesc, vGravity, &PhaseTypeInfo);
	Safe_Release(pPhysX_Manager);

	if (nullptr == m_pFabric)
	{
		MSG_BOX("Failed Create Fabric");
		return E_FAIL;
	}
	//

	vector<PxVec4>			ParticlesCopy;
	ParticlesCopy.resize(m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		// To put attachment point closer to each other
		/*if (clothMesh.mInvMasses[i] < 1e-6)
			clothMesh.mVertices[i] = (clothMesh.mVertices[i]) * 0.95f;*/

		// w component is 1/mass, or 0.0f for anchored/fixed particles
		ParticlesCopy[i] = physx::PxVec4(PhysXConverter::ToPxVec3(m_Vertices[i].vPosition), 0.f);
	}

	// Create Cloth
	m_pCloth = pClothFactory->createCloth(cloth::Range<PxVec4>(&ParticlesCopy[0], &ParticlesCopy[0] + ParticlesCopy.size()), *m_pFabric);
	ParticlesCopy.clear();

	//
	cloth::PhaseConfig* pPhases = New cloth::PhaseConfig[m_pFabric->getNumPhases()];
	for (_int i = 0; i < m_pFabric->getNumPhases(); ++i)
	{
		pPhases[i].mPhaseIndex = i; // Set index to the corresponding set (constraint group)

		//Give phases different configs depending on type
		switch (PhaseTypeInfo[i])
		{
		case nv::cloth::ClothFabricPhaseType::eINVALID:
			// ERROR
			break;
		case cloth::ClothFabricPhaseType::eVERTICAL:
			break;
		case cloth::ClothFabricPhaseType::eHORIZONTAL:
			break;
		case cloth::ClothFabricPhaseType::eBENDING:
			break;
		case cloth::ClothFabricPhaseType::eSHEARING:
			break;
		default:
			// ERROR
			break;
		}

		//For this example we give very phase the same config
		pPhases[i].mStiffness = 0.3f;
		pPhases[i].mStiffnessMultiplier = 0.3f;
		pPhases[i].mCompressionLimit = 0.3f;
		pPhases[i].mStretchLimit = 0.3f;
	}

	m_pCloth->setPhaseConfig(cloth::Range<cloth::PhaseConfig>(pPhases, pPhases + m_pFabric->getNumPhases()));
	Safe_Delete_Array(pPhases);

	m_pSolver = pClothFactory->createSolver();

	m_pSolver->addCloth(m_pCloth);
	
	return S_OK;
}

CVIBuffer_Cloth* CVIBuffer_Cloth::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Cloth* pInstance = new CVIBuffer_Cloth(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CVIBuffer_Cloth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Cloth::Clone(void* pArg)
{
	CVIBuffer_Cloth* pInstance = new CVIBuffer_Cloth(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Cloth");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Cloth::Free()
{
	__super::Free();

	if (nullptr != m_pSolver)
	{
		m_pSolver->removeCloth(m_pCloth);
	}
	
	Safe_Delete(m_pSolver);
	Safe_Delete(m_pCloth);
	if (nullptr != m_pFabric &&
		true == m_isCloned)
	{
		m_pFabric->decRefCount();
	}
}
