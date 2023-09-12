#include "..\Public\Dynamic_Mesh.h"
#include "Bone.h"
#include "PhysX_Manager.h"

CDynamic_Mesh::CDynamic_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMesh(pDevice, pContext)
{
}

CDynamic_Mesh::CDynamic_Mesh(const CDynamic_Mesh& rhs)
	: CMesh(rhs)
{
}

void CDynamic_Mesh::Set_WindVelocity(_float3 vWindVelocity)
{
	if (nullptr == m_pCloth)
		return;

	if (80.f < vWindVelocity.Length())
	{
		vWindVelocity.Normalize();
		vWindVelocity *= 80.f;
	}

	m_pCloth->setWindVelocity(PhysXConverter::ToPxVec3(vWindVelocity));
}

HRESULT CDynamic_Mesh::Initialize_Prototype(CModel::TYPE eType, const CModel::BONES& Bones, const Engine::MESH Mesh, _float4x4 PivotMatrix, HANDLE hFile)
{
	m_iMaterialIndex = Mesh.iMaterialIndex;
	lstrcpy(m_szName, Mesh.szName);
	m_iNumVertexBuffers = { 1 };
	m_iNumVertices = { Mesh.iNumVertices };
	m_iIndexStride = { sizeof(_ulong) };
	m_iNumIndices = { Mesh.iNumFaces * 3 };
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eType = eType;

	HRESULT		hr = 0;

	if (CModel::TYPE_NONANIM == m_eType)
	{
		hr = Ready_VertexBuffer_NonAnim(Mesh, PivotMatrix);
	}
	else
	{
		hr = Ready_VertexBuffer_Anim(Mesh, Bones);
	}

	FAILED_CHECK_RETURN(hr, E_FAIL);


#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint		iNumFaces = { 0 };

	for (_uint i = 0; i < Mesh.iNumFaces; ++i)
	{
		pIndices[iNumFaces++] = Mesh.Faces[i].iIndices[0];
		pIndices[iNumFaces++] = Mesh.Faces[i].iIndices[1];
		pIndices[iNumFaces++] = Mesh.Faces[i].iIndices[2];
	}

	m_Indices.resize(m_iNumIndices);
	memcpy(m_Indices.data(), pIndices, sizeof(_ulong) * m_iNumIndices);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);
#pragma endregion


	// 질량값 초기화
	m_InvMasses.clear();
	m_InvMasses.resize(m_iNumVertices);
	fill(m_InvMasses.begin(), m_InvMasses.end(), 0.3f);

	if (0 == hFile)
	{
		if (FAILED(Initialize_ClothMesh()))
			return E_FAIL;
	}
	else
	{
		if (FAILED(Initialize_ClothMesh(hFile)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CDynamic_Mesh::Initialize(void* pArg)
{
	return S_OK;
}

void CDynamic_Mesh::Tick(_float fTimeDelta)
{
	if (nullptr == m_pCloth ||
		nullptr == m_pSolver)
		return;

	// 고정 프레임
	m_pSolver->beginSimulation(1 / 60.f);

	for (_uint i = 0; i < m_pSolver->getSimulationChunkCount(); ++i)
	{
		m_pSolver->simulateChunk(i);
	}

	m_pSolver->endSimulation();

	// 동적버퍼 map 전에 미리 값을 옮겨둠.
	cloth::MappedRange<PxVec4>	Particles = m_pCloth->getCurrentParticles();
	vector<_float3> Positions;
	Positions.resize(m_iNumVertices);
	for (_int i = 0; i < Particles.size(); ++i)
	{
		// xyz = 현재 포지션
		// w = 질량 역수 // 나중에 사용할 예정.
		Positions[i] = PhysXConverter::ToXMFLOAT3(Particles[i].getXYZ());
	}

	if (CModel::TYPE_NONANIM == m_eType)
	{
		D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

		m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

		VTXMESH* pVertices = static_cast<VTXMESH*>(MappedSubResource.pData);

		for (_uint i = 0; i < m_iNumVertices; ++i)
		{
			pVertices[i].vPosition = Positions[i];
		}

		m_pContext->Unmap(m_pVB, 0);
	}
	else // ANIM
	{
		D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

		m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

		VTXANIMMESH* pVertices = static_cast<VTXANIMMESH*>(MappedSubResource.pData);

		for (_uint i = 0; i < m_iNumVertices; ++i)
		{
			pVertices[i].vPosition = Positions[i];
		}

		m_pContext->Unmap(m_pVB, 0);
	}
}

void CDynamic_Mesh::Reset_Position()
{
	if (CModel::TYPE_NONANIM == m_eType)
	{
		D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

		m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

		VTXMESH* pVertices = static_cast<VTXMESH*>(MappedSubResource.pData);

		memcpy(pVertices, m_NonAnimVertices.data(), sizeof(VTXMESH) * m_iNumVertices);

		m_pContext->Unmap(m_pVB, 0);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

		m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

		VTXANIMMESH* pVertices = static_cast<VTXANIMMESH*>(MappedSubResource.pData);

		memcpy(pVertices, m_AnimVertices.data(), sizeof(VTXANIMMESH) * m_iNumVertices);

		m_pContext->Unmap(m_pVB, 0);
	}
}

HRESULT CDynamic_Mesh::Remake_ClothMesh(vector<_float> InvMasses)
{
	m_InvMasses.clear();
	m_InvMasses = InvMasses;

	Clear_ClothMesh();

	if (FAILED(Initialize_ClothMesh()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDynamic_Mesh::Ready_VertexBuffer_NonAnim(const Engine::MESH Mesh, _float4x4 PivotMatrix)
{
	m_iStride = { sizeof(VTXMESH) };

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);
	m_VertexPositions.reserve(m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &Mesh.vPositions[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));
		m_VertexPositions.push_back(pVertices[i].vPosition);

		memcpy(&pVertices[i].vNormal, &Mesh.vNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		memcpy(&pVertices[i].vTexCoord, &Mesh.vTexCoords[i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &Mesh.vTangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent,
			XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PivotMatrix));
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	m_NonAnimVertices.resize(m_iNumVertices);
	memcpy(m_NonAnimVertices.data(), pVertices, sizeof(VTXMESH) * m_iNumVertices);

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CDynamic_Mesh::Ready_VertexBuffer_Anim(const Engine::MESH Mesh, const CModel::BONES& Bones)
{
	m_iStride = { sizeof(VTXANIMMESH) };

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };


	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);
	m_VertexPositions.reserve(m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &Mesh.vPositions[i], sizeof(_float3));
		m_VertexPositions.push_back(pVertices[i].vPosition);

		memcpy(&pVertices[i].vNormal, &Mesh.vNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexCoord, &Mesh.vTexCoords[i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &Mesh.vTangents[i], sizeof(_float3));
	}

	m_iNumBones = Mesh.iNumBones;

	for (_uint i = 0; i < Mesh.iNumBones; ++i)
	{
		Engine::BONE Bone = Mesh.Bones[i];

		_uint iIndex = { 0 };

		auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pValue) {
			if (!lstrcmp(Bone.szName, pValue->Get_Name()))
				return true;
			else
			{
				++iIndex;
				return false;
			}
			});

		_float4x4		OffsetMatrix;
		memcpy(&OffsetMatrix, &Bone.OffsetMatrix, sizeof(_float4x4));

		Bones[iIndex]->Set_OffsetMatrix(OffsetMatrix);

		m_BoneIndices.push_back(iIndex);

		for (_uint j = 0; j < Bone.iNumWeights; ++j)
		{
			Engine::WEIGHT VertexWeight = Bone.Weights[j];

			if (0.f == pVertices[VertexWeight.iVertexId].vBlendWeights.x)
			{
				pVertices[VertexWeight.iVertexId].vBlendIndices.x = i;
				pVertices[VertexWeight.iVertexId].vBlendWeights.x = VertexWeight.fWeight;
			}
			else if (0.f == pVertices[VertexWeight.iVertexId].vBlendWeights.y)
			{
				pVertices[VertexWeight.iVertexId].vBlendIndices.y = i;
				pVertices[VertexWeight.iVertexId].vBlendWeights.y = VertexWeight.fWeight;
			}
			else if (0.f == pVertices[VertexWeight.iVertexId].vBlendWeights.z)
			{
				pVertices[VertexWeight.iVertexId].vBlendIndices.z = i;
				pVertices[VertexWeight.iVertexId].vBlendWeights.z = VertexWeight.fWeight;
			}
			else if (0.f == pVertices[VertexWeight.iVertexId].vBlendWeights.w)
			{
				pVertices[VertexWeight.iVertexId].vBlendIndices.w = i;
				pVertices[VertexWeight.iVertexId].vBlendWeights.w = VertexWeight.fWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint	iBoneIndex = 0;

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pValue)
			{
				if (!lstrcmp(m_szName, pValue->Get_Name()))
					return true;
				else
				{
					++iBoneIndex;
					return false;
				}
			});


		m_BoneIndices.push_back(iBoneIndex);
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	m_AnimVertices.clear();
	m_AnimVertices.resize(m_iNumVertices);
	memcpy(m_AnimVertices.data(), pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CDynamic_Mesh::Initialize_ClothMesh(HANDLE hFile)
{
	if (FAILED(Read_ClothData(hFile)))
	{
		MSG_BOX("Failed Read File");
		return E_FAIL;
	}

	nv::cloth::ClothMeshDesc MeshDesc;
	MeshDesc.setToDefault();
	MeshDesc.points.data = m_VertexPositions.data();
	MeshDesc.points.stride = sizeof(_float3);
	MeshDesc.points.count = m_iNumVertices;

	MeshDesc.flags = 0; // 32bit
	MeshDesc.triangles.data = m_Indices.data();
	MeshDesc.triangles.stride = sizeof(_ulong) * 3;
	MeshDesc.triangles.count = m_Indices.size() / 3;

	MeshDesc.invMasses.data = m_InvMasses.data();
	MeshDesc.invMasses.stride = sizeof(_float);
	MeshDesc.invMasses.count = m_iNumVertices;

	if (false == MeshDesc.isValid())
	{
		MSG_BOX("Failed to Create Cloth MeshDesc");
		return E_FAIL;
	}

	PxVec3 vGravity(0.f, -9.81f, 0.f);
	cloth::Vector<_int>::Type PhaseTypeInfo;
	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);
	cloth::Factory* pClothFactory = pPhysX_Manager->Get_ClothFactory();
	m_pFabric = NvClothCookFabricFromMesh(pClothFactory, MeshDesc, vGravity, &PhaseTypeInfo, false);
	Safe_Release(pPhysX_Manager);

	if (nullptr == m_pFabric)
	{
		MSG_BOX("Failed Create Fabric");
		return E_FAIL;
	}

	vector<PxVec4>	ParticlesCopy;
	ParticlesCopy.resize(m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		ParticlesCopy[i] = physx::PxVec4(PhysXConverter::ToPxVec3(m_VertexPositions[i]), m_InvMasses[i]);
	}

	// Create Cloth
	m_pCloth = pClothFactory->createCloth(cloth::Range<PxVec4>(&ParticlesCopy[0], &ParticlesCopy[0] + ParticlesCopy.size()), *m_pFabric);
	ParticlesCopy.clear();

	//
	cloth::PhaseConfig* pPhases = New cloth::PhaseConfig[m_pFabric->getNumPhases()];
	for (_int i = 0; i < m_pFabric->getNumPhases(); ++i)
	{
		pPhases[i].mPhaseIndex = i;
		pPhases[i].mStiffness = 1.f;
		pPhases[i].mStiffnessMultiplier = 1.f;
		pPhases[i].mCompressionLimit = 1.f;
		pPhases[i].mStretchLimit = 1.f;
	}

	m_pCloth->setPhaseConfig(cloth::Range<cloth::PhaseConfig>(pPhases, pPhases + m_pFabric->getNumPhases()));
	Safe_Delete_Array(pPhases);

	m_pSolver = pClothFactory->createSolver();
	
	/* 크게 건드릴게 없는 고정 옵션 값 */
	m_pCloth->setSolverFrequency(60.f);
	m_pCloth->setTetherConstraintScale(1.1f);
	m_pCloth->setDragCoefficient(0.5f);
	m_pCloth->setLiftCoefficient(0.6f);
	//m_pCloth->setFriction(10.f);
	m_pCloth->setWindVelocity(PxVec3(0.f, 0.f, 0.f));
	m_pCloth->setGravity(vGravity);	
	_float4 vQuat = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90.f), 0.f, 0.f);
	m_pCloth->setRotation(PhysXConverter::ToPxQuat(vQuat));

	if (1 != m_ClothSpheres.size() % 2 &&
		0 != m_ClothSpheres.size())
	{
		vector<PxVec4> Spheres;
		Spheres.reserve(m_ClothSpheres.size());
		for (auto& ClothSphere : m_ClothSpheres)
		{
			Spheres.push_back({ PhysXConverter::ToPxVec3(ClothSphere.first), ClothSphere.second });
		}

		m_pCloth->setSpheres(cloth::Range<PxVec4>(Spheres.data(), Spheres.data() + 2), 0, m_pCloth->getNumSpheres());

		vector<_uint> CapsuleIndices;
		for (_uint i = 0; i < m_ClothSpheres.size(); ++i)
		{
			CapsuleIndices.push_back(i);
		}

		m_pCloth->setCapsules(cloth::Range<_uint>(CapsuleIndices.data(), CapsuleIndices.data() + 2), 0, m_pCloth->getNumCapsules());
	}

	m_pSolver->addCloth(m_pCloth);

	return S_OK;
}

HRESULT CDynamic_Mesh::Initialize_ClothMesh()
{
	nv::cloth::ClothMeshDesc MeshDesc;
	MeshDesc.setToDefault();
	MeshDesc.points.data = m_VertexPositions.data();
	MeshDesc.points.stride = sizeof(_float3);
	MeshDesc.points.count = m_iNumVertices;

	MeshDesc.flags = 0; // 32bit
	MeshDesc.triangles.data = m_Indices.data();
	MeshDesc.triangles.stride = sizeof(_ulong) * 3;
	MeshDesc.triangles.count = m_Indices.size() / 3;

	MeshDesc.invMasses.data = m_InvMasses.data();
	MeshDesc.invMasses.stride = sizeof(_float);
	MeshDesc.invMasses.count = m_iNumVertices;

	if (false == MeshDesc.isValid())
	{
		MSG_BOX("Failed to Create Cloth MeshDesc");
		return E_FAIL;
	}

	PxVec3 vGravity(0.f, -9.81f, 0.f);
	cloth::Vector<_int>::Type PhaseTypeInfo;
	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);
	cloth::Factory* pClothFactory = pPhysX_Manager->Get_ClothFactory();
	m_pFabric = NvClothCookFabricFromMesh(pClothFactory, MeshDesc, vGravity, &PhaseTypeInfo, false);
	Safe_Release(pPhysX_Manager);

	if (nullptr == m_pFabric)
	{
		MSG_BOX("Failed Create Fabric");
		return E_FAIL;
	}

	vector<PxVec4>	ParticlesCopy;
	ParticlesCopy.resize(m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		ParticlesCopy[i] = physx::PxVec4(PhysXConverter::ToPxVec3(m_VertexPositions[i]), m_InvMasses[i]);
	}

	// Create Cloth
	m_pCloth = pClothFactory->createCloth(cloth::Range<PxVec4>(&ParticlesCopy[0], &ParticlesCopy[0] + ParticlesCopy.size()), *m_pFabric);
	ParticlesCopy.clear();

	//
	cloth::PhaseConfig* pPhases = New cloth::PhaseConfig[m_pFabric->getNumPhases()];
	for (_int i = 0; i < m_pFabric->getNumPhases(); ++i)
	{
		pPhases[i].mPhaseIndex = i;
		pPhases[i].mStiffness = 1.f;
		pPhases[i].mStiffnessMultiplier = 1.f;
		pPhases[i].mCompressionLimit = 1.f;
		pPhases[i].mStretchLimit = 1.f;
	}

	m_pCloth->setPhaseConfig(cloth::Range<cloth::PhaseConfig>(pPhases, pPhases + m_pFabric->getNumPhases()));
	Safe_Delete_Array(pPhases);

	m_pSolver = pClothFactory->createSolver();

	/* 크게 건드릴게 없는 고정 옵션 값 */
	m_pCloth->setSolverFrequency(10.f);
	m_pCloth->setTetherConstraintScale(1.f);
	m_pCloth->setDragCoefficient(0.5f);
	m_pCloth->setLiftCoefficient(0.6f);
	//m_pCloth->setFriction(10.f);
	m_pCloth->setWindVelocity(PxVec3(0.f, 0.f, 0.f));
	m_pCloth->setGravity(vGravity);
	_float4 vQuat = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90.f), 0.f, 0.f);
	m_pCloth->setRotation(PhysXConverter::ToPxQuat(vQuat));

	if (1 != m_ClothSpheres.size() % 2 &&
		0 != m_ClothSpheres.size())
	{
		vector<PxVec4> Spheres;
		Spheres.reserve(m_ClothSpheres.size());
		for (auto& ClothSphere : m_ClothSpheres)
		{
			Spheres.push_back({ PhysXConverter::ToPxVec3(ClothSphere.first), ClothSphere.second });
		}

		m_pCloth->setSpheres(cloth::Range<PxVec4>(Spheres.data(), Spheres.data() + 2), 0, m_pCloth->getNumSpheres());

		vector<_uint> CapsuleIndices;
		for (_uint i = 0; i < m_ClothSpheres.size(); ++i)
		{
			CapsuleIndices.push_back(i);
		}

		m_pCloth->setCapsules(cloth::Range<_uint>(CapsuleIndices.data(), CapsuleIndices.data() + 2), 0, m_pCloth->getNumCapsules());
	}

	m_pSolver->addCloth(m_pCloth);

	return S_OK;
}

void CDynamic_Mesh::Clear_ClothMesh()
{
	if (nullptr != m_pSolver)
	{
		m_pSolver->removeCloth(m_pCloth);
	}

	Safe_Delete(m_pSolver);
	Safe_Delete(m_pCloth);

	if (nullptr != m_pFabric)
	{
		m_pFabric->decRefCount();
	}
}

HRESULT CDynamic_Mesh::Read_ClothData(HANDLE hFile)
{
	_ulong	dwByte = 0;

	_uint iSize = { 0 };
	// Vertex Num
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	if (iSize != m_VertexPositions.size())
	{
		MSG_BOX("Invalid Data");
		CloseHandle(hFile);
		return E_FAIL;
	}

	m_InvMasses.clear();
	m_InvMasses.reserve(iSize);

	for (_uint i = 0; i < iSize; ++i)
	{
		_float fInvMass = 1.f;
		ReadFile(hFile, &fInvMass, sizeof(_float), &dwByte, nullptr);
		m_InvMasses.push_back(fInvMass);
	}

	_uint iCapsuleSize = { 0 };
	ReadFile(hFile, &iCapsuleSize, sizeof(_uint), &dwByte, nullptr);

	m_ClothSpheres.reserve(iCapsuleSize * 2);

	for (_uint i = 0; i < iCapsuleSize; ++i)
	{
		pair<_float3, _float> SourSphere, DestSphere;
		ReadFile(hFile, &SourSphere.first, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &SourSphere.second, sizeof(_float), &dwByte, nullptr);

		m_ClothSpheres.push_back(SourSphere);

		ReadFile(hFile, &DestSphere.first, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &DestSphere.second, sizeof(_float), &dwByte, nullptr);

		m_ClothSpheres.push_back(DestSphere);
	}

	return S_OK;
}

CDynamic_Mesh* CDynamic_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	CModel::TYPE eType, const CModel::BONES& Bones, const Engine::MESH Mesh, _float4x4 PivotMatrix, HANDLE hFile)
{
	CDynamic_Mesh* pInstance = new CDynamic_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, Bones, Mesh, PivotMatrix, hFile)))
	{
		MSG_BOX("Failed to Created CDynamic_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CDynamic_Mesh::Clone(void* pArg)
{
	CDynamic_Mesh* pInstance = new CDynamic_Mesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDynamic_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDynamic_Mesh::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Clear_ClothMesh();
	}
}
