#include "..\Public\MeshParts.h"

#include "Bone.h"
#include "Mesh.h"
#include "Dynamic_Mesh.h"
#include "Texture.h"
#include "Animation.h"

CMeshParts::CMeshParts(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CMeshParts::CMeshParts(const CMeshParts& _rhs)
	: CComponent(_rhs)
	, m_Model(_rhs.m_Model)
	, m_NodeDatas(_rhs.m_NodeDatas)
	, m_MeshDatas(_rhs.m_MeshDatas)
	, m_MaterialDatas(_rhs.m_MaterialDatas)
	, m_AnimationDatas(_rhs.m_AnimationDatas)
	, m_iNumMeshes(_rhs.m_iNumMeshes)
	, m_Meshes(_rhs.m_Meshes)
	, m_iNumMaterials(_rhs.m_iNumMaterials)
	, m_Materials(_rhs.m_Materials)
{
	for (auto& pMesh : m_Meshes)
	{
		Safe_AddRef(pMesh);
	}

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMtrlTexture)
			Safe_AddRef(pTexture);
	}
}

void CMeshParts::Get_Matrices(const _uint& _iMeshIndex, CModel::BONES _Bones, _Inout_ _float4x4* _pMatrices, _float4x4 _PivotMatrix)
{
	m_Meshes[_iMeshIndex]->Get_Matrices(_Bones, _pMatrices, _PivotMatrix);
}

void CMeshParts::Set_WindVelocity(_float3 vWindVelocity)
{
	if (0 == m_DynamicMeshIndices.size())
		return;

	for (auto& DynamicMeshIndex : m_DynamicMeshIndices)
	{
		static_cast<CDynamic_Mesh*>(m_Meshes[DynamicMeshIndex])->Set_WindVelocity(vWindVelocity);
	}
}

HRESULT CMeshParts::Initialize_Prototype(const wstring& _wstrMeshPartsFilePath, const wstring& _wstrMeshPartsTag)
{
	if (FAILED(Ready_File(_wstrMeshPartsFilePath.c_str())))
		return E_FAIL;

	if (FAILED(Ready_Material()))
		return E_FAIL;

	m_wstrMeshPartsTag = _wstrMeshPartsTag;

	return S_OK;
}

HRESULT CMeshParts::Initialize(void* _pArg)
{
	if (nullptr == _pArg)
	{
		MSG_BOX("[CMeshParts] _pArg is nullptr");
		return E_FAIL;
	}

	MESHPARTSDESC* pMeshPartsDesc = static_cast<MESHPARTSDESC*>(_pArg);
	if (nullptr == pMeshPartsDesc->pBones)
	{
		MSG_BOX("[CMeshParts] pMeshPartsDesc's m_pBones is nullptr");
		return E_FAIL;
	}
	m_vPartsColor = pMeshPartsDesc->vColor;

	if (nullptr == pMeshPartsDesc->szClothDataFilePath)
	{
		if (FAILED(Ready_Mesh(*pMeshPartsDesc->pBones)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(Ready_DynamicMesh(*pMeshPartsDesc->pBones, pMeshPartsDesc->szClothDataFilePath)))
			return E_FAIL;
	}

	return S_OK;
}

void CMeshParts::Tick(const _uint& _iMeshIndex, _float _fTimeDelta)
{
	m_Meshes[_iMeshIndex]->Tick(_fTimeDelta);
}

HRESULT CMeshParts::Render(const _uint& _iMeshIndex)
{
	if (FAILED(m_Meshes[_iMeshIndex]->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeshParts::Bind_Material(CShader* _pShader, const char* _pConstantName, const _uint& _iMeshIndex, Engine::TextureType _MaterialType)
{
	_uint iMaterialIndex = m_Meshes[_iMeshIndex]->Get_MaterialIndex();

	if (_iMeshIndex >= m_Meshes.size() ||
		_MaterialType >= TextureType_MAX ||
		_MaterialType < 0 ||
		m_iNumMaterials == 0 ||
		nullptr == m_Materials[iMaterialIndex].pMtrlTexture[_MaterialType])
		return E_FAIL;

	return m_Materials[iMaterialIndex].pMtrlTexture[_MaterialType]->Bind_ShaderResource(_pShader, _pConstantName);
}

HRESULT CMeshParts::Ready_File(const _tchar* _pMeshPartsFilePath)
{
	HANDLE hFile = CreateFile(_pMeshPartsFilePath,
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	_ulong	dwStrByte = 0;

	ZEROMEM(&m_Model);

	// Nodes NumNodes
	ReadFile(hFile, &(m_Model.iNumNodes), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_Model.iNumNodes; ++i)
	{
		NODE Node;
		ZEROMEM(&Node);

		// Node Name
		ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		ReadFile(hFile, Node.szName, dwStrByte, &dwByte, nullptr);
		if (0 == dwByte)
		{
			MSG_BOX("Failed Read String Data");
			return E_FAIL;
		}

		// Node Transformation
		ReadFile(hFile, &(Node.TransformationMatrix), sizeof(_float4x4), &dwByte, nullptr);

		// Node NodeIndex
		ReadFile(hFile, &(Node.iNodeIndex), sizeof(_uint), &dwByte, nullptr);

		// Node Parent
		ReadFile(hFile, &(Node.iParent), sizeof(_int), &dwByte, nullptr);

		// Node NumChildren
		ReadFile(hFile, &(Node.iNumChildren), sizeof(_uint), &dwByte, nullptr);

		// Node Children (array)
		Node.iChildrens = New _uint[Node.iNumChildren];
		ZeroMemory(Node.iChildrens, sizeof(_uint) * (Node.iNumChildren));
		ReadFile(hFile, Node.iChildrens, sizeof(_uint) * (Node.iNumChildren), &dwByte, nullptr);

		m_NodeDatas.push_back(Node);
	}

	// Write Meshes

	// Meshes NumMeshes
	ReadFile(hFile, &(m_Model.iNumMeshes), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_Model.iNumMeshes; ++i)
	{
		MESH Mesh;
		ZEROMEM(&Mesh);

		// Mesh Name
		ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		ReadFile(hFile, Mesh.szName, dwStrByte, &dwByte, nullptr);
		if (0 == dwByte)
		{
			MSG_BOX("Failed Read String Data");
			return E_FAIL;
		}

		// Mesh MaterialIndex
		ReadFile(hFile, &(Mesh.iMaterialIndex), sizeof(_uint), &dwByte, nullptr);

		// Mesh NumVertices
		ReadFile(hFile, &(Mesh.iNumVertices), sizeof(_uint), &dwByte, nullptr);

		// Mesh NumFaces
		ReadFile(hFile, &(Mesh.iNumFaces), sizeof(_uint), &dwByte, nullptr);

		Mesh.Faces = New FACE[Mesh.iNumFaces];
		ZeroMemory(Mesh.Faces, sizeof(FACE) * (Mesh.iNumFaces));

		for (_uint j = 0; j < Mesh.iNumFaces; ++j)
		{
			FACE Face;
			ZEROMEM(&Face);

			// Face NumIndices
			ReadFile(hFile, &(Face.iNumIndices), sizeof(_uint), &dwByte, nullptr);

			// Face Indices
			Face.iIndices = New _uint[Face.iNumIndices];
			ZeroMemory(Face.iIndices, sizeof(_uint) * (Face.iNumIndices));
			ReadFile(hFile, Face.iIndices, sizeof(_uint) * (Face.iNumIndices), &dwByte, nullptr);

			Mesh.Faces[j] = Face;
		}

		// Mesh Positions
		Mesh.vPositions = New _float3[Mesh.iNumVertices];
		ZeroMemory(Mesh.vPositions, sizeof(_float3) * (Mesh.iNumVertices));
		ReadFile(hFile, Mesh.vPositions, sizeof(_float3) * (Mesh.iNumVertices), &dwByte, nullptr);

		// Mesh Normals
		Mesh.vNormals = New _float3[Mesh.iNumVertices];
		ZeroMemory(Mesh.vNormals, sizeof(_float3) * (Mesh.iNumVertices));
		ReadFile(hFile, Mesh.vNormals, sizeof(_float3) * (Mesh.iNumVertices), &dwByte, nullptr);

		// Mesh TexCoords
		Mesh.vTexCoords = New _float2[Mesh.iNumVertices];
		ZeroMemory(Mesh.vTexCoords, sizeof(_float2) * (Mesh.iNumVertices));
		ReadFile(hFile, Mesh.vTexCoords, sizeof(_float2) * (Mesh.iNumVertices), &dwByte, nullptr);

		// Mesh Tangents
		Mesh.vTangents = New _float3[Mesh.iNumVertices];
		ZeroMemory(Mesh.vTangents, sizeof(_float3) * (Mesh.iNumVertices));
		ReadFile(hFile, Mesh.vTangents, sizeof(_float3) * (Mesh.iNumVertices), &dwByte, nullptr);

		// Mesh NumBones
		ReadFile(hFile, &(Mesh.iNumBones), sizeof(_uint), &dwByte, nullptr);

		Mesh.Bones = New BONE[Mesh.iNumBones];
		ZeroMemory(Mesh.Bones, sizeof(BONE) * (Mesh.iNumBones));

		// Write Bones
		for (_uint j = 0; j < Mesh.iNumBones; ++j)
		{
			BONE Bone;
			ZEROMEM(&Bone);

			// Mesh Name
			ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
			ReadFile(hFile, Bone.szName, dwStrByte, &dwByte, nullptr);
			if (0 == dwByte)
			{
				MSG_BOX("Failed Read String Data");
				return E_FAIL;
			}

			// Mesh OffsetMatrix
			ReadFile(hFile, &(Bone.OffsetMatrix), sizeof(_float4x4), &dwByte, nullptr);

			// Mesh NumWeights
			ReadFile(hFile, &(Bone.iNumWeights), sizeof(_uint), &dwByte, nullptr);

			Bone.Weights = New WEIGHT[Bone.iNumWeights];
			ZeroMemory(Bone.Weights, sizeof(WEIGHT) * (Bone.iNumWeights));

			// Write Weights
			for (_uint k = 0; k < Bone.iNumWeights; ++k)
			{
				WEIGHT Weight;
				ZEROMEM(&Weight);

				// Weight VertexId
				ReadFile(hFile, &(Weight.iVertexId), sizeof(_uint), &dwByte, nullptr);

				// Weight Weight
				ReadFile(hFile, &(Weight.fWeight), sizeof(_float), &dwByte, nullptr);

				Bone.Weights[k] = Weight;
			}

			Mesh.Bones[j] = Bone;
		}

		m_MeshDatas.push_back(Mesh);
	}

	// Read Materials

	// Material NumMaterials
	ReadFile(hFile, &(m_Model.iNumMaterials), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_Model.iNumMaterials; ++i)
	{
		MATERIAL Material;
		ZEROMEM(&Material);

		// MaterialTex
		ReadFile(hFile, Material.MaterialTexture, sizeof(MATERIALTEX) * TextureType_MAX, &dwByte, nullptr);

		m_MaterialDatas.push_back(Material);
	}

	// Read Animations
	// Animation NumAnimations
	ReadFile(hFile, &(m_Model.iNumAnimations), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_Model.iNumAnimations; ++i)
	{
		ANIMATION Animation;
		ZEROMEM(&Animation);

		// Animation Name
		ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		ReadFile(hFile, Animation.szName, dwStrByte, &dwByte, nullptr);
		if (0 == dwByte)
		{
			MSG_BOX("Failed Read String Data");
			return E_FAIL;
		}

		// Animation Duration
		ReadFile(hFile, &(Animation.fDuration), sizeof(_float), &dwByte, nullptr);

		// Animation TickPerSecond
		ReadFile(hFile, &(Animation.fTickPerSecond), sizeof(_float), &dwByte, nullptr);

		// Animation NumChannels
		ReadFile(hFile, &(Animation.iNumChannels), sizeof(_uint), &dwByte, nullptr);

		Animation.Channels = New CHANNEL[Animation.iNumChannels];
		ZeroMemory(Animation.Channels, sizeof(CHANNEL) * (Animation.iNumChannels));

		for (_uint j = 0; j < Animation.iNumChannels; ++j)
		{
			CHANNEL Channel;
			ZEROMEM(&Channel);

			// Animation Name
			ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
			ReadFile(hFile, Channel.szName, dwStrByte, &dwByte, nullptr);
			if (0 == dwByte)
			{
				MSG_BOX("Failed Read String Data");
				return E_FAIL;
			}

			// Channel NumScalingKeys
			ReadFile(hFile, &(Channel.iNumScalingKeys), sizeof(_uint), &dwByte, nullptr);

			// Channel ScalingKeys
			Channel.ScalingKeys = New VECTORKEY[Channel.iNumScalingKeys];
			ZeroMemory(Channel.ScalingKeys, sizeof(VECTORKEY) * (Channel.iNumScalingKeys));
			ReadFile(hFile, Channel.ScalingKeys, sizeof(VECTORKEY) * (Channel.iNumScalingKeys), &dwByte, nullptr);

			// Channel NumRotationKeys
			ReadFile(hFile, &(Channel.iNumRotationKeys), sizeof(_uint), &dwByte, nullptr);

			// Channel RotationKeys
			Channel.RotationKeys = New QUATERNIONKEY[Channel.iNumRotationKeys];
			ZeroMemory(Channel.RotationKeys, sizeof(QUATERNIONKEY) * (Channel.iNumRotationKeys));
			ReadFile(hFile, Channel.RotationKeys, sizeof(QUATERNIONKEY) * (Channel.iNumRotationKeys), &dwByte, nullptr);

			// Channel NumPositionKeys
			ReadFile(hFile, &(Channel.iNumPositionKeys), sizeof(_uint), &dwByte, nullptr);

			// Channel PositionKeys
			Channel.PositionKeys = New VECTORKEY[Channel.iNumPositionKeys];
			ZeroMemory(Channel.PositionKeys, sizeof(VECTORKEY) * (Channel.iNumPositionKeys));
			ReadFile(hFile, Channel.PositionKeys, sizeof(VECTORKEY) * (Channel.iNumPositionKeys), &dwByte, nullptr);

			Animation.Channels[j] = Channel;
		}

		m_AnimationDatas.push_back(Animation);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CMeshParts::Ready_Mesh(const CModel::BONES& _Bones)
{
	m_iNumMeshes = m_Model.iNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, _Bones, m_MeshDatas[i], _float4x4());
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CMeshParts::Ready_DynamicMesh(const CModel::BONES& _Bones, const _tchar* szClothDataFilePath)
{
	m_iNumMeshes = m_Model.iNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		HANDLE hFile = Read_ClothIndexData(szClothDataFilePath, i);

		CDynamic_Mesh* pMesh = CDynamic_Mesh::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, _Bones, m_MeshDatas[i], _float4x4(), hFile);
		
		if (0 != hFile)
		{
			CloseHandle(hFile);
		}			

		if (nullptr == pMesh)
			return E_FAIL;

		m_DynamicMeshIndices.push_back(m_Meshes.size());

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CMeshParts::Ready_Material()
{
	m_iNumMaterials = m_Model.iNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MESHMATERIAL	MeshMaterial;
		ZeroMemory(&MeshMaterial, sizeof MeshMaterial);

		for (_uint j = 0; j < TextureType_MAX; ++j)
		{
			if (!lstrcmp(m_MaterialDatas[i].MaterialTexture[j].szTexPath, TEXT("")))
			{
				if (j == DIFFUSE)
				{
					MSG_BOX("Diffuse Texture NULL");
				}
				else
					continue;
			}

			MeshMaterial.pMtrlTexture[j] = CTexture::Create(m_pDevice, m_pContext,
				m_MaterialDatas[i].MaterialTexture[j].szTexPath, 1);

			if (nullptr == MeshMaterial.pMtrlTexture[j])
			{
				MSG_BOX("Mtrl Texture NULL");
				return E_FAIL;
			}
		}

		m_Materials.push_back(MeshMaterial);
	}

	return S_OK;
}

void CMeshParts::Release_FileDatas()
{
	for (auto& Node : m_NodeDatas)
	{
		Safe_Delete_Array(Node.iChildrens);
	}

	m_NodeDatas.clear();

	for (auto& Mesh : m_MeshDatas)
	{
		for (_uint i = 0; i < Mesh.iNumFaces; ++i)
		{
			Safe_Delete_Array(Mesh.Faces[i].iIndices);
		}

		Safe_Delete_Array(Mesh.Faces);

		Safe_Delete_Array(Mesh.vPositions);
		Safe_Delete_Array(Mesh.vNormals);
		Safe_Delete_Array(Mesh.vTexCoords);
		Safe_Delete_Array(Mesh.vTangents);

		for (_uint i = 0; i < Mesh.iNumBones; ++i)
		{
			Safe_Delete_Array(Mesh.Bones[i].Weights);
		}

		Safe_Delete_Array(Mesh.Bones);
	}

	m_MeshDatas.clear();

	m_MaterialDatas.clear();

	for (auto& Animation : m_AnimationDatas)
	{
		for (_uint i = 0; i < Animation.iNumChannels; ++i)
		{
			Safe_Delete_Array(Animation.Channels[i].ScalingKeys);
			Safe_Delete_Array(Animation.Channels[i].RotationKeys);
			Safe_Delete_Array(Animation.Channels[i].PositionKeys);
		}

		Safe_Delete_Array(Animation.Channels);
	}

	m_AnimationDatas.clear();
}

HANDLE CMeshParts::Read_ClothIndexData(const _tchar* szClothDataFilePath, _uint iMeshIndex)
{
	if (nullptr == szClothDataFilePath ||
		!lstrcmp(TEXT("Default"), szClothDataFilePath))
		return 0;

	HANDLE hFile = CreateFile(szClothDataFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return 0;
	}

	_ulong	dwByte = 0;

	_uint iDataIndex = { 99999 };
	ReadFile(hFile, &iDataIndex, sizeof(_uint), &dwByte, nullptr);

	if (iDataIndex != iMeshIndex)
	{
		CloseHandle(hFile);
		return 0;
	}
	
	return hFile;
}

CMeshParts* CMeshParts::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, 
	const wstring& _wstrMeshPartsFilePath, const wstring& _wstrMeshPartsTag)
{
	CMeshParts* pInstance = New CMeshParts(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_wstrMeshPartsFilePath, _wstrMeshPartsTag)))
	{
		MSG_BOX("Failed to Created CMeshParts");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMeshParts* CMeshParts::Clone(void* _pArg)
{
	CMeshParts* pInstance = New CMeshParts(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned CMeshParts");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshParts::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Release_FileDatas();
	}

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMtrlTexture)
			Safe_Release(pTexture);
	}
	m_Materials.clear();
}
