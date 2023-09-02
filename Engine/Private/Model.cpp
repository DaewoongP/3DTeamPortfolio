#include "..\Public\Model.h"
#include "Bone.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"
#include "Transform.h"
CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
	ZEROMEM(&m_Model);
}

CModel::CModel(const CModel& rhs)
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
{
	for (_uint AnimTypeIndex = 0; AnimTypeIndex < ANIM_END; ++AnimTypeIndex)
	{
		m_tAnimationDesc[AnimTypeIndex] = ANIMATIONDESC(rhs.m_tAnimationDesc[AnimTypeIndex]);
		for (auto& pOriginalAnimation : rhs.m_tAnimationDesc[AnimTypeIndex].Animations)
		{
			m_tAnimationDesc[AnimTypeIndex].Animations.push_back(pOriginalAnimation->Clone());
		}
	}

	for (auto& pOriginalBone : rhs.m_Bones)
	{
		m_Bones.push_back(pOriginalBone->Clone());
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

const CBone* CModel::Get_Bone(const _tchar* pBoneName)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)
		{
			if (!lstrcmp(pBoneName, pBone->Get_Name()))
				return true;
			else
				return false;
		});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}

const _float4x4* CModel::Get_BoneCombinedTransformationMatrixPtr(_uint iIndex)
{
	return m_Bones[iIndex]->Get_CombinedTransformationMatrixPtr();
}

_float4x4 CModel::Get_BoneCombinedTransformationMatrix(_uint iIndex)
{
	return m_Bones[iIndex]->Get_CombinedTransformationMatrix();
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _float4x4 PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Ready_File(eType, pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Bones(m_NodeDatas.front())))
		return E_FAIL;

	if (FAILED(Ready_Meshes(eType, PivotMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials()))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	m_PostRootMatrix = XMMatrixIdentity();

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

void CModel::Reset_Animation(_uint iAnimIndex, ANIMTYPE eType, CTransform* pTransform)
{
	m_tAnimationDesc[eType].iCurrentAnimIndex = iAnimIndex;
	m_tAnimationDesc[eType].iPreviousAnimIndex = m_tAnimationDesc[eType].iCurrentAnimIndex;
	m_tAnimationDesc[eType].isResetAnimTrigger = true;
}

void CModel::Play_Animation(_float fTimeDelta, ANIMTYPE eType, CTransform* pTransform)
{
	if (m_tAnimationDesc[eType].Animations[m_tAnimationDesc[eType].iCurrentAnimIndex]->Invalidate_AccTime(fTimeDelta) || m_tAnimationDesc[eType].isResetAnimTrigger )
	{
		//애니메이션 재생이 다 되면 여기가 실행되는거임.
		m_tAnimationDesc[eType].Animations[m_tAnimationDesc[eType].iCurrentAnimIndex]->Reset();
		m_tAnimationDesc[eType].isAnimChangeLerp = true;
		m_tAnimationDesc[eType].fAnimChangeTimer = ANIMATIONLERPTIME;
		m_PostRootMatrix = XMMatrixIdentity();
		m_tAnimationDesc[eType].isResetAnimTrigger = false;
	}
	else if (pTransform != nullptr)
	{
		if(m_tAnimationDesc[eType].Animations[m_tAnimationDesc[eType].iCurrentAnimIndex]->Get_RootAnim_State())
			Do_Root_Animation(pTransform);
	}

	//노티파이용
	m_tAnimationDesc[eType].Animations[m_tAnimationDesc[eType].iCurrentAnimIndex]->Invalidate_Frame(fTimeDelta);

	//뼈 이동용
	if (!m_tAnimationDesc[eType].isAnimChangeLerp)
	{
		m_tAnimationDesc[eType].Animations[m_tAnimationDesc[eType].iCurrentAnimIndex]->Invalidate_TransformationMatrix(m_Bones, fTimeDelta);
	}
	else if(m_tAnimationDesc[eType].fAnimChangeTimer >= 0.0)
	{
		m_tAnimationDesc[eType].Animations[m_tAnimationDesc[eType].iCurrentAnimIndex]->Invalidate_TransformationMatrix_Lerp(m_Bones, fTimeDelta, ANIMATIONLERPTIME - m_tAnimationDesc[eType].fAnimChangeTimer,m_iRootBoneIndex);
		m_tAnimationDesc[eType].fAnimChangeTimer -= fTimeDelta;
	}
	else
		m_tAnimationDesc[eType].isAnimChangeLerp = false;
	
	
	/* 모델에 표현되어있는 모든 뼈들의 CombinedTransformationMatrix */
	_int iBoneIndex = 0;
	
	for (auto& pBone : m_Bones)
	{
		if (m_iRootBoneIndex == pBone->Get_ParentNodeIndex()&& m_tAnimationDesc[eType].Animations[m_tAnimationDesc[eType].iCurrentAnimIndex]->Get_RootAnim_State())
		{
			pBone->Invalidate_CombinedTransformationMatrix_Basic(m_Bones);
		}
		else 
		{
			pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
		}
		iBoneIndex++;
	}
	
}

HRESULT CModel::Find_BoneIndex(const _tchar* pBoneName, _Inout_ _uint* iIndex)
{
	*iIndex = 0;
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pValue) {
		if (!lstrcmp(pBoneName, pValue->Get_Name()))
			return true;
		else
		{
			++(*iIndex);
			return false;
		}
	});

	if (m_Bones.end() == iter)
	{
		MSG_BOX("Failed Find BoneIndex");
		return E_FAIL;
	}

	return S_OK;
}

void CModel::Set_CurrentAnimIndex(_uint iIndex, ANIMTYPE eType)
{ 
	m_tAnimationDesc[eType].iCurrentAnimIndex = iIndex;
	m_tAnimationDesc[eType].isAnimChangeLerp = true;
	m_tAnimationDesc[eType].fAnimChangeTimer = ANIMATIONLERPTIME;
}

void CModel::Do_Root_Animation(CTransform* pTransform)
{
	if (pTransform != nullptr)
	{
		_float4x4 current_Matrix = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();
		_float4x4 post_Matirx = m_PostRootMatrix;

		_float3 vCurrent_Look = current_Matrix.Look();
		_float3 vPost_Look = post_Matirx.Look();

		_float4x4 player_Matrix_Override = XMMatrixIdentity();

		vCurrent_Look.Normalize();
		vPost_Look.Normalize();

		
		if (vCurrent_Look!=vPost_Look && fabsf(vCurrent_Look.x- vPost_Look.x)>0.0001f&& fabsf(vCurrent_Look.z - vPost_Look.z) > 0.0001f)
		{
			_float dot = XMVectorGetX(XMVector3Dot(vPost_Look, vCurrent_Look));
			_float radian = acosf(dot);
			
			if (XMVectorGetY(XMVector3Cross(vCurrent_Look, vPost_Look)) > 0)
				radian = 2 * XMVectorGetX(g_XMPi) - radian;
		
			player_Matrix_Override = XMMatrixRotationY(radian);		
		}

		_float3 vCurrent_Position = current_Matrix.Translation();
		_float3 vPost_Position = post_Matirx.Translation();
		_float3 Calculated_Position = (vCurrent_Position - vPost_Position);
		memcpy(player_Matrix_Override.m[3], &Calculated_Position, sizeof _float3);

		pTransform->Set_WorldMatrix(player_Matrix_Override * pTransform->Get_WorldMatrix());
		m_PostRootMatrix = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();
	}
}

HRESULT CModel::Bind_Material(CShader* pShader, const char* pConstantName, _uint iMeshIndex, Engine::TextureType MaterialType)
{
	if (iMeshIndex >= m_iNumMeshes ||
		MaterialType >= TextureType_MAX ||
		MaterialType < 0 ||
		m_iNumMaterials == 0 ||
		nullptr == m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pMtrlTexture[MaterialType])
		return E_FAIL;

	return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pMtrlTexture[MaterialType]->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const char* pConstantName, _uint iMeshIndex)
{
	_float4x4		BoneMatrices[MAX_SHADERMATRIX];
	ZeroMemory(BoneMatrices, sizeof(_float4x4) * MAX_SHADERMATRIX);

	m_Meshes[iMeshIndex]->Get_Matrices(m_Bones, BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

	pShader->Bind_Matrices(pConstantName, BoneMatrices, MAX_SHADERMATRIX);

	return S_OK;
}

HRESULT CModel::Ready_File(TYPE eType, const _tchar* pModelFilePath)
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
		Node.iChildrens = new _uint[Node.iNumChildren];
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

		// Mesh Normals
		Mesh.vNormals = new _float3[Mesh.iNumVertices];
		ZeroMemory(Mesh.vNormals, sizeof(_float3) * (Mesh.iNumVertices));
		ReadFile(hFile, Mesh.vNormals, sizeof(_float3) * (Mesh.iNumVertices), &dwByte, nullptr);

		// Mesh TexCoords
		Mesh.vTexCoords = new _float2[Mesh.iNumVertices];
		ZeroMemory(Mesh.vTexCoords, sizeof(_float2) * (Mesh.iNumVertices));
		ReadFile(hFile, Mesh.vTexCoords, sizeof(_float2) * (Mesh.iNumVertices), &dwByte, nullptr);

		// Mesh Tangents
		Mesh.vTangents = new _float3[Mesh.iNumVertices];
		ZeroMemory(Mesh.vTangents, sizeof(_float3) * (Mesh.iNumVertices));
		ReadFile(hFile, Mesh.vTangents, sizeof(_float3) * (Mesh.iNumVertices), &dwByte, nullptr);

		// Mesh NumBones
		ReadFile(hFile, &(Mesh.iNumBones), sizeof(_uint), &dwByte, nullptr);

		Mesh.Bones = new BONE[Mesh.iNumBones];
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

			Bone.Weights = new WEIGHT[Bone.iNumWeights];
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
	if (TYPE_ANIM == eType)
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

			Animation.Channels = new CHANNEL[Animation.iNumChannels];
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
				Channel.ScalingKeys = new VECTORKEY[Channel.iNumScalingKeys];
				ZeroMemory(Channel.ScalingKeys, sizeof(VECTORKEY) * (Channel.iNumScalingKeys));
				ReadFile(hFile, Channel.ScalingKeys, sizeof(VECTORKEY) * (Channel.iNumScalingKeys), &dwByte, nullptr);

				// Channel NumRotationKeys
				ReadFile(hFile, &(Channel.iNumRotationKeys), sizeof(_uint), &dwByte, nullptr);

				// Channel RotationKeys
				Channel.RotationKeys = new QUATERNIONKEY[Channel.iNumRotationKeys];
				ZeroMemory(Channel.RotationKeys, sizeof(QUATERNIONKEY) * (Channel.iNumRotationKeys));
				ReadFile(hFile, Channel.RotationKeys, sizeof(QUATERNIONKEY) * (Channel.iNumRotationKeys), &dwByte, nullptr);

				// Channel NumPositionKeys
				ReadFile(hFile, &(Channel.iNumPositionKeys), sizeof(_uint), &dwByte, nullptr);

				// Channel PositionKeys
				Channel.PositionKeys = new VECTORKEY[Channel.iNumPositionKeys];
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

HRESULT CModel::Ready_Bones(Engine::NODE Node)
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

HRESULT CModel::Ready_Meshes(TYPE eType, _float4x4 PivotMatrix)
{
	m_iNumMeshes = m_Model.iNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eType, m_Bones, m_MeshDatas[i], PivotMatrix);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials()
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

HRESULT CModel::Ready_Animations()
{
	for (_uint iAnimTypeIndex = 0; iAnimTypeIndex < ANIM_END; iAnimTypeIndex++)
	{
		m_tAnimationDesc[iAnimTypeIndex].iNumAnimations = m_Model.iNumAnimations;
		for (_uint i = 0; i < m_Model.iNumAnimations; ++i)
		{
			CAnimation* pAnimation = CAnimation::Create(m_AnimationDatas[i], m_Bones);
			if (nullptr == pAnimation)
				return E_FAIL;

			m_tAnimationDesc[iAnimTypeIndex].Animations.push_back(pAnimation);
		}
	}
	return S_OK;
}

void CModel::Release_FileDatas()
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

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, _float4x4 PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
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

	for (int i = 0; i < ANIM_END; i++)
	{
		if (m_tAnimationDesc[i].Animations.size() == 0)
			continue;
		for (auto& pAnimation : m_tAnimationDesc[i].Animations)
		{
			Safe_Release(pAnimation);
		}
		m_tAnimationDesc[i].Animations.clear();
	}
}
