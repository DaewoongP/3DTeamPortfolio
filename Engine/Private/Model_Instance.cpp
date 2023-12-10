#include "Model_Instance.h"
#include "Model_Instance.h"

#include "Bone.h"
#include "Notify.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"
#include "Mesh_Instance.h"

CModel_Instance::CModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CModel_Instance::CModel_Instance(const CModel_Instance& rhs)
	: CComponent(rhs)
	, m_Model(rhs.m_Model)
	, m_NodeDatas(rhs.m_NodeDatas)
	, m_MeshDatas(rhs.m_MeshDatas)
	, m_MaterialDatas(rhs.m_MaterialDatas)
	, m_AnimationDatas(rhs.m_AnimationDatas)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iRootBoneIndex(rhs.m_iRootBoneIndex)
	, m_isExportedTool(rhs.m_isExportedTool)
{
	for (auto& pOriginalBone : rhs.m_Bones)
	{
		m_Bones.push_back(pOriginalBone->Clone());
	}

	m_tAnimationDesc = ANIMATIONDESC(rhs.m_tAnimationDesc);
	for (auto& pOriginalAnimation : rhs.m_tAnimationDesc.Animations)
	{
		m_tAnimationDesc.Animations.push_back(pOriginalAnimation->Clone());
		m_tAnimationDesc.Animations.back()->Get_Notify_Point()->BindBoneMatrixForParticle(m_Bones);
	}

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

HRESULT CModel_Instance::Initialize_Prototype(CModel::TYPE eType, const _tchar* pModelFilePath, _float4x4* pInstanceMatrix, _uint iInstanceCnt, _float4x4 PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Ready_File(eType, pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Bones(m_NodeDatas.front())))
		return E_FAIL;

	if (FAILED(Ready_Meshes(eType, pInstanceMatrix, iInstanceCnt, PivotMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials()))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_Instance::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel_Instance::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

void CModel_Instance::Play_Animation(_float fTimeDelta)
{
	//�ִϸ��̼� ������ ���������
	if (m_tAnimationDesc.iNumAnimations == 0 ||
		m_tAnimationDesc.iCurrentAnimIndex >= m_tAnimationDesc.iNumAnimations)
		return;


	CAnimation* currentAnimation = m_tAnimationDesc.Animations[m_tAnimationDesc.iCurrentAnimIndex];
	if (currentAnimation->Invalidate_AccTime(fTimeDelta) || m_tAnimationDesc.isResetAnimTrigger)
	{
		//�ð��� ������, ����Ʈ���Ű� ȣ��ƴٸ� ���½�ŵ�ϴ�.
		currentAnimation->Reset();
		if (currentAnimation->Get_LerpAnim())
		{
			m_tAnimationDesc.isAnimChangeLerp = true;
			m_tAnimationDesc.fAnimChangeTimer = ANIMATIONLERPTIME;
		}
		m_isFirstFrame = true;
		m_tAnimationDesc.isResetAnimTrigger = false;
	}

	m_tAnimationDesc.isFinishAnimation = currentAnimation->Get_Duration() <= currentAnimation->Get_Accmulation() &&
		false == currentAnimation->Get_LoopAnim();

	currentAnimation->Invalidate_Frame(fTimeDelta, m_PivotMatrix, nullptr, nullptr);

	if (!m_tAnimationDesc.isAnimChangeLerp)
	{
		currentAnimation->Invalidate_TransformationMatrix(m_Bones, fTimeDelta, &m_tAnimationDesc.AffectBoneVec);
	}
	else if (m_tAnimationDesc.fAnimChangeTimer >= 0.0) //0���� ũ�ٸ�?
	{
		//���ҽ�Ŵ
		m_tAnimationDesc.fAnimChangeTimer -= fTimeDelta;
		//0 ���ϰ� �Ź�������?
		if (m_tAnimationDesc.fAnimChangeTimer < 0)
		{
			m_tAnimationDesc.isAnimChangeLerp = false;
			currentAnimation->Invalidate_TransformationMatrix(m_Bones, fTimeDelta, &m_tAnimationDesc.AffectBoneVec);
		}
		else //0���ϰ� �ƴ϶��? ����
		{
			currentAnimation->Invalidate_TransformationMatrix_Lerp(m_Bones, fTimeDelta, ANIMATIONLERPTIME - m_tAnimationDesc.fAnimChangeTimer, m_iRootBoneIndex, &m_tAnimationDesc.AffectBoneVec);
		}
	}

	for (auto& pBone : m_Bones)
	{
		_int iFindIndex = pBone->Get_Index();
		auto iter = find_if(m_tAnimationDesc.AffectBoneVec.begin(), m_tAnimationDesc.AffectBoneVec.end(), [&](auto data) {
			if (data == iFindIndex)
				return true;
			return false;
			});

		if (iter == m_tAnimationDesc.AffectBoneVec.end())
		{
			continue;
		}

		if (m_iRootBoneIndex == pBone->Get_ParentNodeIndex() && m_tAnimationDesc.Animations[m_tAnimationDesc.iCurrentAnimIndex]->Get_RootAnim_State())
		{
			pBone->Invalidate_CombinedTransformationMatrix_Basic(m_Bones);
		}
		else
		{
			pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
		}
	}
}

HRESULT CModel_Instance::Bind_Material(CShader* pShader, const char* pConstantName, _uint iMeshIndex, TextureType MaterialType)
{
	if (iMeshIndex >= m_iNumMeshes ||
		MaterialType >= TextureType_MAX ||
		MaterialType < 0 ||
		m_iNumMaterials == 0 ||
		nullptr == m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pMtrlTexture[MaterialType])
		return E_FAIL;

	return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pMtrlTexture[MaterialType]->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CModel_Instance::Bind_BoneMatrices(CShader* pShader, const char* pConstantName, _uint iMeshIndex)
{
	_float4x4		BoneMatrices[MAX_SHADERMATRIX];
	ZeroMemory(BoneMatrices, sizeof(_float4x4) * MAX_SHADERMATRIX);

	m_Meshes[iMeshIndex]->Get_Matrices(m_Bones, BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

	pShader->Bind_Matrices(pConstantName, BoneMatrices, MAX_SHADERMATRIX);

	return S_OK;
}

void CModel_Instance::Change_Animation(_uint iAnimIndex)
{
	m_tAnimationDesc.iCurrentAnimIndex = iAnimIndex;
	m_tAnimationDesc.iPreviousAnimIndex = m_tAnimationDesc.iCurrentAnimIndex;
	m_tAnimationDesc.isResetAnimTrigger = true;
	m_tAnimationDesc.isFinishAnimation = false;
}

HRESULT CModel_Instance::Ready_File(CModel::TYPE eType, const _tchar* pModelFilePath)
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
		// iNumChildren �� 0�� ��� ���� �Ҵ� ����
		if (0 != Node.iNumChildren)
		{
			Node.iChildrens = New _uint[Node.iNumChildren];
			ZeroMemory(Node.iChildrens, sizeof(_uint) * (Node.iNumChildren));
			ReadFile(hFile, Node.iChildrens, sizeof(_uint) * (Node.iNumChildren), &dwByte, nullptr);
		}

		else
		{
			Node.iChildrens = nullptr;
		}

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
	if (CModel::TYPE_ANIM == eType)
	{
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
	}
	else // NonAnim
	{
		m_Model.iNumAnimations = 0;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CModel_Instance::Ready_Bones(Engine::NODE Node)
{
	CBone* pBone = CBone::Create(Node);

	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	for (_uint i = 0; i < Node.iNumChildren; ++i)
	{
		Ready_Bones(m_NodeDatas[Node.iChildrens[i]]);
	}

	return S_OK;
}

HRESULT CModel_Instance::Ready_Meshes(CModel::TYPE eType, _float4x4* pInstanceMatrix, _uint iInstanceCnt, _float4x4 PivotMatrix)
{
	m_iNumMeshes = m_Model.iNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh_Instance* pMesh = CMesh_Instance::Create(m_pDevice, m_pContext, m_MeshDatas[i], pInstanceMatrix, iInstanceCnt, m_Bones, PivotMatrix, eType);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel_Instance::Ready_Materials()
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

HRESULT CModel_Instance::Ready_Animations()
{
	for (int i = 0; i < m_Model.iNumNodes; i++)
	{
		m_tAnimationDesc.AffectBoneVec.push_back(i);
	}

	m_tAnimationDesc.iNumAnimations = m_Model.iNumAnimations;
	for (_uint i = 0; i < m_Model.iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(m_AnimationDatas[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_tAnimationDesc.Animations.push_back(pAnimation);
	}
	return S_OK;
}

void CModel_Instance::Release_FileDatas()
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

CModel_Instance* CModel_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const _tchar* pModelFilePath, _float4x4* pInstanceMatrix, _uint iInstanceCnt, _float4x4 PivotMatrix)
{
	CModel_Instance* pInstance = New CModel_Instance(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, pInstanceMatrix, iInstanceCnt, PivotMatrix)))
	{
		MSG_BOX("Failed to Created CModel_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel_Instance::Clone(void* pArg)
{
	CModel_Instance* pInstance = New CModel_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CModel_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel_Instance::Free()
{
	__super::Free();

	if (!m_isCloned)
	{
		Release_FileDatas();
	}

	for (auto& pBone : m_Bones)
	{
		Safe_Release(pBone);
	}

	m_Bones.clear();

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

	for (auto& pAnimation : m_tAnimationDesc.Animations)
	{
		Safe_Release(pAnimation);
	}

	m_tAnimationDesc.Animations.clear();
}
