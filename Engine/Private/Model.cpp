#include "..\Public\Model.h"
#include "Bone.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"
#include "Transform.h"
#include "Channel.h"
#include "Notify.h"
#include "Bone.h"

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
	, m_ModelGCM(rhs.m_ModelGCM)

	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iRootBoneIndex(rhs.m_iRootBoneIndex)
	, m_iAnimationPartCount(rhs.m_iAnimationPartCount)
	, m_isCreatedByGCM(rhs.m_isCreatedByGCM)
	, m_isExportedTool(rhs.m_isExportedTool)
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

	for (int i = 0; i < ANIM_END; i++)
	{
		m_AnimationDatasGCM[i] = rhs.m_AnimationDatasGCM[i];
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

CBone* CModel::Get_Bone_Index(_uint iIndex)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)
		{
			if (iIndex == pBone->Get_Index())
				return true;
			else
				return false;
		});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}

CAnimation* CModel::Get_Animation(const wstring& wstrAnimationTag, ANIMTYPE eType) const
{
	_uint iAnimationIndex = Find_Animation_Index(wstrAnimationTag);

	return m_tAnimationDesc[eType].Animations[iAnimationIndex];
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
	m_wstrModelPath = ToRelativePath(pModelFilePath);
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);
	char szFilePath[MAX_PATH] = "";
	char szExtension[MAX_PATH] = "";
	WCharToChar(pModelFilePath, szFilePath);
	_splitpath_s(szFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExtension, MAX_PATH);
	if (!strcmp(szExtension, ".dat"))
	{
		if (FAILED(Ready_File(eType, pModelFilePath)))
			return E_FAIL;

		if (FAILED(Ready_Bones(m_NodeDatas.front())))
			return E_FAIL;

		if (FAILED(Ready_Meshes(eType, PivotMatrix)))
			return E_FAIL;

		if (FAILED(Ready_Materials()))
			return E_FAIL;

		if (eType == TYPE::TYPE_ANIM)
		{
			if (FAILED(Ready_Animations()))
				return E_FAIL;
		}
	}
	else
	{
		m_isCreatedByGCM = true;
		if (FAILED(Ready_File_GCM(eType, pModelFilePath)))
			return E_FAIL;

		if (FAILED(Ready_Bones_GCM(m_NodeDatas.front())))
			return E_FAIL;

		if (FAILED(Ready_Meshes_GCM(eType, PivotMatrix)))
			return E_FAIL;

		if (FAILED(Ready_Materials_GCM()))
			return E_FAIL;

		if (eType == TYPE::TYPE_ANIM)
		{
			if (FAILED(Ready_Animations_GCM()))
				return E_FAIL;
		}
	}
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

void CModel::Change_Animation(const wstring& wstrAnimationTag, ANIMTYPE eType)
{
	m_tAnimationDesc[eType].iCurrentAnimIndex = Find_Animation_Index(wstrAnimationTag);
	m_tAnimationDesc[eType].iPreviousAnimIndex = m_tAnimationDesc[eType].iCurrentAnimIndex;
	m_tAnimationDesc[eType].isResetAnimTrigger = true;
	m_tAnimationDesc[eType].isFinishAnimation = false;
	//Reset_Animation(eType);
}

void CModel::Change_Animation(_uint iAnimIndex, ANIMTYPE eType)
{
	m_tAnimationDesc[eType].iCurrentAnimIndex = iAnimIndex;
	m_tAnimationDesc[eType].iPreviousAnimIndex = m_tAnimationDesc[eType].iCurrentAnimIndex;
	m_tAnimationDesc[eType].isResetAnimTrigger = true;
	m_tAnimationDesc[eType].isFinishAnimation = false;
	//Reset_Animation(eType);
}

void CModel::Play_Animation(_float fTimeDelta, ANIMTYPE eType, CTransform* pTransform)
{
	// �ִϸ��̼� ������ ��������
	if (m_tAnimationDesc[eType].iNumAnimations == 0)
		return;


	CAnimation* currentAnimation = m_tAnimationDesc[eType].Animations[m_tAnimationDesc[eType].iCurrentAnimIndex];
	if (currentAnimation->Invalidate_AccTime(fTimeDelta) || m_tAnimationDesc[eType].isResetAnimTrigger)
	{
		//�ִϸ��̼� ��������
		currentAnimation->Reset();
		//������ ������������ ������ ��������.
		if (currentAnimation->Get_LerpAnim())
		{
			m_tAnimationDesc[eType].isAnimChangeLerp = true;
			m_tAnimationDesc[eType].fAnimChangeTimer = ANIMATIONLERPTIME;
		}
		////0�����(��ü)���? ��Ʈ ��Ʈ���� ������.
		m_isFirstFrame = true;
		//���¼��� �ٵ����ϱ� Ʈ���� ����.
		m_tAnimationDesc[eType].isResetAnimTrigger = false;
	}
	//Ʈ�������� �ִٸ�?
	else if (pTransform != nullptr)
	{
		//��Ʈ�ִϸ��̼� �������ִٸ� ��Ʈ�ִϸ��̼� ��������.
		if (currentAnimation->Get_RootAnim_State() &&
			!currentAnimation->Get_Paused_State() &&
			(currentAnimation->Get_Duration() >
				currentAnimation->Get_Accmulation()))
		{
			Do_Root_Animation(fTimeDelta, pTransform, m_isFirstFrame);
			m_isFirstFrame = false;
		}


	}
	// �ִϸ��̼� ���� üũ ( ���� �� ��� ��� false )
	m_tAnimationDesc[eType].isFinishAnimation = currentAnimation->Get_Duration() <=
		currentAnimation->Get_Accmulation() &&
		!currentAnimation->Get_LoopAnim();

	//��Ƽ���� ������
	currentAnimation->Invalidate_Frame(fTimeDelta);

	//�� ������
	//���� �𵨿� ���������� �ȵ��ִٸ� �׳� ������
	if (!m_tAnimationDesc[eType].isAnimChangeLerp)
	{
		currentAnimation->Invalidate_TransformationMatrix(m_Bones, fTimeDelta, &m_tAnimationDesc[eType].AffectBoneVec);
	}
	else if (m_tAnimationDesc[eType].fAnimChangeTimer >= 0.0)
	{
		//���������� ���ֵ��� ��������.
		currentAnimation->Invalidate_TransformationMatrix_Lerp(m_Bones, fTimeDelta, ANIMATIONLERPTIME - m_tAnimationDesc[eType].fAnimChangeTimer, m_iRootBoneIndex, &m_tAnimationDesc[eType].AffectBoneVec);
		m_tAnimationDesc[eType].fAnimChangeTimer -= fTimeDelta;
	}
	else
		//���������� ���ִµ� �ð��� �ٵ�����? �������� ����.
		m_tAnimationDesc[eType].isAnimChangeLerp = false;

	for (auto& pBone : m_Bones)
	{
		_int iFindIndex = pBone->Get_Index();
		auto iter = find_if(m_tAnimationDesc[eType].AffectBoneVec.begin(), m_tAnimationDesc[eType].AffectBoneVec.end(), [&](auto data) {
			if (data == iFindIndex)
				return true;
			return false;
			});

		if (iter == m_tAnimationDesc[eType].AffectBoneVec.end())
		{
			continue;
		}

		//������ ���� ��Ʈ���� ����������ִٸ�? �׸��� ��Ʈ�ִϸ��̼��� �������ִٸ�?
		if (m_iRootBoneIndex == pBone->Get_ParentNodeIndex() && m_tAnimationDesc[eType].Animations[m_tAnimationDesc[eType].iCurrentAnimIndex]->Get_RootAnim_State())
		{
			//�� ���� ��Ʈ���� ������ �����ʾƾ���.(��Ʈ�� matrix�� transform�� ���������� �������ٰű� ������.)
			pBone->Invalidate_CombinedTransformationMatrix_Basic(m_Bones);
		}
		else
		{
			pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
		}
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

void CModel::Do_Root_Animation(_float fTimeDelta, CTransform* pTransform, _bool isFirstFrame)
{
	if (pTransform != nullptr)
	{
		//루트 애니메이션을위한 현재와 과거
		_float4x4 current_Matrix = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();
		_float4x4 post_Matirx = XMMatrixIdentity();

		//이번 애니메이션 첫 프레임이라면? 과거루트위치는 지금 뼈의 위치
		if (isFirstFrame)
		{
			//시작 당시의 루트 뼈의 매트릭스( 중간에 바뀜 )
			m_PostRootMatrix = current_Matrix;
			//시작 당시의 루트 뼈의 매트릭스( 안바꿀거임 )
			m_BeginRootAnimRootMatrix = current_Matrix;
		}

		post_Matirx = m_PostRootMatrix;
		
		//회전값 추출
		_float3 vCurrent_Look = current_Matrix.Look();
		_float3 vPost_Look = post_Matirx.Look();
		_float3 vFirst_Look = m_BeginRootAnimRootMatrix.Look();

		_float4x4 player_Matrix_Override = XMMatrixIdentity();
		_float4x4 FirstFrameToCurrentRotation = XMMatrixIdentity();

		vCurrent_Look.Normalize();
		vPost_Look.Normalize();
		vFirst_Look.Normalize();

		//과거 / 현재 회전값 추출
		{
			_float dot = XMVectorGetX(XMVector3Dot(vPost_Look, vCurrent_Look));
			_float radian = acosf(dot);

			if (!isnan(radian))
			{
				if (XMVectorGetY(XMVector3Cross(vCurrent_Look, vPost_Look)) > 0)
					radian = 2 * XMVectorGetX(g_XMPi) - radian;

				player_Matrix_Override = XMMatrixRotationY(radian);
			}
		}

		//첫프레임 / 현재 회전값 추출
		{
			_float dot = XMVectorGetX(XMVector3Dot(vFirst_Look, vCurrent_Look));
			_float radian = acosf(dot);

			if (!isnan(radian))
			{
				if (XMVectorGetY(XMVector3Cross(vCurrent_Look, vFirst_Look)) > 0)
					radian = 2 * XMVectorGetX(g_XMPi) - radian;

				FirstFrameToCurrentRotation = XMMatrixRotationY(radian);
			}
		}
		
		_float3 vCurrent_Position = current_Matrix.Translation();
		_float3 vPost_Position = post_Matirx.Translation();

		//시작점 기준 회전값 추출
		

		_float4x4 vPivotOutPosScale = m_PivotMatrix;
		vPivotOutPosScale.MatrixScale(_float3(1, 1, 1));
		vPivotOutPosScale._41 = 0;
		vPivotOutPosScale._42 = 0;
		vPivotOutPosScale._43 = 0;

		//이건 영점기준 애니메이션의 이동값입니다. 이 이동값을 돌려줌.
		_float3 Calculated_Position = (vCurrent_Position - vPost_Position);
		Calculated_Position = XMVector3TransformNormal(Calculated_Position, XMMatrixInverse(nullptr,FirstFrameToCurrentRotation));
		Calculated_Position = XMVector3TransformNormal(Calculated_Position, vPivotOutPosScale);
		_float4x4 PositionMatrix = XMMatrixTranslation(Calculated_Position.x, Calculated_Position.y, Calculated_Position.z);

		_float3 vOffsetVector = m_tAnimationDesc[0].Animations[m_tAnimationDesc[0].iCurrentAnimIndex]->Get_OffsetPosition();
		_float4x4 offsetPositionMatrix = XMMatrixTranslation(vOffsetVector.x, vOffsetVector.y, vOffsetVector.z);
		pTransform->Set_WorldMatrix(offsetPositionMatrix * player_Matrix_Override * PositionMatrix * pTransform->Get_WorldMatrix());
		m_PostRootMatrix = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();
	}
}

HRESULT CModel::Separate_Animation(_int iFromIndex, _int iToIndex, ANIMTYPE eType)
{
	//���� �� �����ҿ��� �����մϴ�.
	for (auto iter = m_tAnimationDesc[eType].AffectBoneVec.begin(); iter < m_tAnimationDesc[eType].AffectBoneVec.end();)
	{
		if ((*iter) >= iFromIndex && (*iter) <= iToIndex)
		{
			iter = m_tAnimationDesc[eType].AffectBoneVec.erase(iter);
		}
		else
			iter++;
	}

	//���ο� �� �����ҿ� �� ģ������ �־��ݴϴ�.
	_int max = iToIndex - iFromIndex + 1;
	for (int i = 0; i < max; i++)
	{
		m_tAnimationDesc[m_iAnimationPartCount].AffectBoneVec.push_back(i + iFromIndex);
	}

	//�ִϸ��̼��� �������ݴϴ�.
	m_tAnimationDesc[m_iAnimationPartCount].iNumAnimations = m_Model.iNumAnimations;
	for (_uint i = 0; i < m_Model.iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(m_AnimationDatas[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_tAnimationDesc[m_iAnimationPartCount].Animations.push_back(pAnimation);
	}
	m_iAnimationPartCount++;

	return S_OK;
}

void CModel::Delete_Animation(_uint iAnimIndex, ANIMTYPE eType)
{
	Safe_Release(*(m_tAnimationDesc[eType].Animations.begin() + iAnimIndex));
	m_tAnimationDesc[eType].Animations.erase(m_tAnimationDesc[eType].Animations.begin() + iAnimIndex);
	m_tAnimationDesc[eType].iNumAnimations--;

	if (m_tAnimationDesc[eType].iCurrentAnimIndex >= m_tAnimationDesc[eType].iNumAnimations && m_tAnimationDesc[eType].iNumAnimations != 0)
		m_tAnimationDesc[eType].iCurrentAnimIndex--;
}

HRESULT CModel::Bind_Notify(const _uint& iAnimIndex, const wstring& wstrNotifyTag, function<void()>& Func, ANIMTYPE eType)
{
	if (0 > iAnimIndex || m_tAnimationDesc[eType].Animations.size() <= iAnimIndex ||
		0 > eType || ANIM_END <= eType || nullptr == Func)
		return E_FAIL;

	CAnimation* pAnimation = m_tAnimationDesc[eType].Animations[iAnimIndex];
	if (nullptr == pAnimation)
		return E_FAIL;

	Engine::KEYFRAME* pKeyFrame = pAnimation->Find_NotifyFrame(wstrNotifyTag.c_str());
	if (nullptr == pKeyFrame)
		return E_FAIL;

	static_cast<NOTIFYFRAME*>(pKeyFrame)->Action = Func;

	return S_OK;
}

HRESULT CModel::Bind_Notify(const wstring& wstrAnimIndex, const wstring& wstrNotifyTag, function<void()>& Func, ANIMTYPE eType)
{
	_uint iAnimationIndex = { 0 };
	iAnimationIndex = Find_Animation_Index(wstrAnimIndex);

	return Bind_Notify(iAnimationIndex, wstrNotifyTag, Func, eType);
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

			// 만약 텍스처 풀에 텍스처가 있으면 그포인터를 가져온다
			// 만약 텍스첲 풅에 텍스처가 없다면 생성해서 풀에 넣어준다.

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
	for (int i = 0; i < m_Model.iNumNodes; i++)
	{
		m_tAnimationDesc[0].AffectBoneVec.push_back(i);
	}

	m_iAnimationPartCount++;
	m_tAnimationDesc[0].iNumAnimations = m_Model.iNumAnimations;
	for (_uint i = 0; i < m_Model.iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(m_AnimationDatas[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_tAnimationDesc[0].Animations.push_back(pAnimation);
	}
	return S_OK;
}

//���� �� �ִϸ��̼��� ��ȯ
HRESULT CModel::Convert_Animations_GCM()
{
	if (m_isExportedTool)
		Release_FileDatas_GCM();
	else
	{
		ZEROMEM(&m_ModelGCM);
		for (int i = 0; i < ANIM_END; i++)
			m_AnimationDatasGCM[i].clear();
	}

	m_ModelGCM.iAnimationPartCount = m_iAnimationPartCount;
	m_ModelGCM.iNumNodes = m_NodeDatas.size();
	m_ModelGCM.iNumMaterials = m_iNumMeshes;
	m_ModelGCM.iNumMeshes = m_iNumMaterials;
	m_ModelGCM.iNumAnimations = New _uint[m_iAnimationPartCount];
	m_ModelGCM.iRootBoneIndex = m_iRootBoneIndex;

	m_ModelGCM.iAffectBones = New vector<unsigned int>[m_iAnimationPartCount]();
	for (_uint partCnt = 0; partCnt < m_iAnimationPartCount; partCnt++)
	{
		for (int i = 0; i < m_tAnimationDesc[partCnt].AffectBoneVec.size(); i++)
		{
			m_ModelGCM.iAffectBones[partCnt].push_back(m_tAnimationDesc[partCnt].AffectBoneVec[i]);
		}
	}

	for (_uint partCnt = 0; partCnt < m_iAnimationPartCount; partCnt++)
	{
		m_ModelGCM.iNumAnimations[partCnt] = m_tAnimationDesc[partCnt].iNumAnimations;
		//�ִϸ� �����
		for (_uint i = 0; i < m_tAnimationDesc[partCnt].iNumAnimations; ++i)
		{
			CAnimation* pAnimation = m_tAnimationDesc[partCnt].Animations[i];

			ANIMATION_GCM Animation;
			ZEROMEM(&Animation);
			// �ִϸ��̼� �̸�
			lstrcpy(Animation.szName, pAnimation->Get_AnimationName());

			// �ִϸ��̼� ���� ����
			Animation.fDuration = pAnimation->Get_Duration();
			Animation.fTickPerSecond = pAnimation->Get_TickPerSecond();
			Animation.iNumChannels = pAnimation->Get_NumChannels();
			Animation.isLerp = pAnimation->Get_LerpAnim();
			Animation.isRootAnim = pAnimation->Get_RootAnim();
			Animation.isLoop = pAnimation->Get_LoopAnim();
			Animation.vOffsetPosition = pAnimation->Get_OffsetPosition();

			// ä�θ����
			Animation.Channels = New CHANNEL_GCM[pAnimation->Get_NumChannels()];
			ZeroMemory(Animation.Channels, sizeof(CHANNEL_GCM) * pAnimation->Get_NumChannels());
			for (_uint j = 0; j < pAnimation->Get_NumChannels(); ++j)
			{
				CChannel* pChannel = pAnimation->Get_ChannelVector_Point()[j];

				CHANNEL_GCM Channel;
				ZEROMEM(&Channel);
				// ä�� �̸�
				lstrcpy(Channel.szName, pChannel->Get_ChannelName());

				// ä���� ������ Ű ����
				Channel.iNumMartixKeys = pChannel->Get_NumKeyFrames();
				Channel.iMatrixFrame = New MATRIXFRAME_GCM[pChannel->Get_NumKeyFrames()];
				ZeroMemory(Channel.iMatrixFrame, sizeof(MATRIXFRAME_GCM) * pChannel->Get_NumKeyFrames());

				vector<MATRIXFRAME>* pMatrixVec = pChannel->Get_MarixFrame();
				for (_uint k = 0; k < pChannel->Get_NumKeyFrames(); ++k)
				{
					Channel.iMatrixFrame[k].fTime = (*pMatrixVec)[k].fTime;
					memcpy(&Channel.iMatrixFrame[k].vScale, &(*pMatrixVec)[k].vScale, sizeof _float3);
					memcpy(&Channel.iMatrixFrame[k].vRotation, &(*pMatrixVec)[k].vRotation, sizeof _float4);
					memcpy(&Channel.iMatrixFrame[k].vTranslation, &(*pMatrixVec)[k].vTranslation, sizeof _float3);
				}
				// ä�� ����
				Animation.Channels[j] = Channel;
			}
			// ��Ƽ���� �����
			Animation.Notify = New NOTIFY_GCM;
			//��Ƽ���� ������ �����
			Animation.Notify->iNumKeyFrames = pAnimation->Get_Notify_Point()->Get_NotifyFrameCount();
			//������ ��������
			vector<pair<wstring, KEYFRAME*>>* pNotifyVec = pAnimation->Get_Notify_Point()->Get_NotifyFrame();

			//�������� ���ο���� ������ִ� ������.
			for (_uint j = 0; j < pAnimation->Get_Notify_Point()->Get_NotifyFrameCount(); ++j)
			{
				pair<wstring, KEYFRAME*> keyframe = (*pNotifyVec)[j];

				switch (*reinterpret_cast<KEYFRAME_GCM::KEYFRAMETYPE*>(&keyframe.second->eKeyFrameType))
				{
				case KEYFRAME_GCM::KF_SPEED:
				{
					SPEEDFRAME_GCM* Notify_Frame = New SPEEDFRAME_GCM();

					lstrcpy(Notify_Frame->szName, keyframe.first.data());
					Notify_Frame->eKeyFrameType = *reinterpret_cast<KEYFRAME_GCM::KEYFRAMETYPE*>(&keyframe.second->eKeyFrameType);
					Notify_Frame->fTime = keyframe.second->fTime;
					Notify_Frame->fSpeed = static_cast<SPEEDFRAME*>(keyframe.second)->fSpeed;

					//�������� ä���� ���ο� �����͸� ��Ƽ���̿� �־���.
					Animation.Notify->tKeyFrame.push_back(Notify_Frame);
					break;
				}

				case KEYFRAME_GCM::KF_SOUND:
				{
					SOUNDFRAME_GCM* Notify_Frame = New SOUNDFRAME_GCM();

					lstrcpy(Notify_Frame->szName, keyframe.first.data());
					Notify_Frame->eKeyFrameType = *reinterpret_cast<KEYFRAME_GCM::KEYFRAMETYPE*>(&keyframe.second->eKeyFrameType);
					Notify_Frame->fTime = keyframe.second->fTime;
					lstrcpy(Notify_Frame->wszSoundTag, static_cast<SOUNDFRAME*>(keyframe.second)->wszSoundTag);

					//�������� ä���� ���ο� �����͸� ��Ƽ���̿� �־���.
					Animation.Notify->tKeyFrame.push_back(Notify_Frame);
					break;
				}
				case KEYFRAME_GCM::KF_NOTIFY:
				{
					NOTIFYFRAME_GCM* Notify_Frame = New NOTIFYFRAME_GCM();

					lstrcpy(Notify_Frame->szName, keyframe.first.data());
					Notify_Frame->eKeyFrameType = *reinterpret_cast<KEYFRAME_GCM::KEYFRAMETYPE*>(&keyframe.second->eKeyFrameType);
					Notify_Frame->fTime = keyframe.second->fTime;

					//�������� ä���� ���ο� �����͸� ��Ƽ���̿� �־���.
					Animation.Notify->tKeyFrame.push_back(Notify_Frame);
					break;
				}
				}
			}

			m_AnimationDatasGCM[partCnt].push_back(Animation);
		}
	}
	return S_OK;
}

//gcm ������ �д� ģ��
HRESULT CModel::Ready_File_GCM(TYPE eType, const _tchar* pModelFilePath)
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

	ZEROMEM(&m_ModelGCM);

	// Nodes NumNodes
	ReadFile(hFile, &(m_ModelGCM.iNumNodes), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_ModelGCM.iNumNodes; ++i)
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
	ReadFile(hFile, &(m_ModelGCM.iNumMeshes), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_ModelGCM.iNumMeshes; ++i)
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
	ReadFile(hFile, &(m_ModelGCM.iNumMaterials), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_ModelGCM.iNumMaterials; ++i)
	{
		MATERIAL Material;
		ZEROMEM(&Material);

		// MaterialTex
		ReadFile(hFile, Material.MaterialTexture, sizeof(MATERIALTEX) * TextureType_MAX, &dwByte, nullptr);

		m_MaterialDatas.push_back(Material);
	}

	ReadFile(hFile, &(m_ModelGCM.iRootBoneIndex), sizeof(_uint), &dwByte, nullptr);

	// Read Animations
	if (TYPE_ANIM == eType)
	{
		// Animation NumAnimations
		ReadFile(hFile, &(m_ModelGCM.iAnimationPartCount), sizeof(_uint), &dwByte, nullptr);

		m_ModelGCM.iAffectBones = New vector<unsigned int>[m_ModelGCM.iAnimationPartCount];
		for (_uint partCnt = 0; partCnt < m_ModelGCM.iAnimationPartCount; partCnt++)
		{
			_uint size = 0;
			ReadFile(hFile, &size, sizeof(_uint), &dwByte, nullptr);
			m_ModelGCM.iAffectBones[partCnt].resize(size);
			for (int i = 0; i < size; i++)
			{
				ReadFile(hFile, &m_ModelGCM.iAffectBones[partCnt][i], sizeof(_uint), &dwByte, nullptr);
			}
		}



		m_ModelGCM.iNumAnimations = New unsigned int[m_ModelGCM.iAnimationPartCount];
		for (_uint animPartIndex = 0; animPartIndex < m_ModelGCM.iAnimationPartCount; animPartIndex++)
		{
			ReadFile(hFile, &(m_ModelGCM.iNumAnimations[animPartIndex]), sizeof(_uint), &dwByte, nullptr);
			for (_uint i = 0; i < m_ModelGCM.iNumAnimations[animPartIndex]; ++i)
			{
				ANIMATION_GCM Animation;
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

				// Animation OptionSetting
				ReadFile(hFile, &(Animation.isLoop), sizeof(_bool), &dwByte, nullptr);
				ReadFile(hFile, &(Animation.isRootAnim), sizeof(_bool), &dwByte, nullptr);
				ReadFile(hFile, &(Animation.isLerp), sizeof(_bool), &dwByte, nullptr);

				ReadFile(hFile, &(Animation.vOffsetPosition), sizeof(XMFLOAT3), &dwByte, nullptr);

				// Animation NumChannels
				ReadFile(hFile, &(Animation.iNumChannels), sizeof(_uint), &dwByte, nullptr);

				Animation.Channels = New CHANNEL_GCM[Animation.iNumChannels];
				ZeroMemory(Animation.Channels, sizeof(CHANNEL_GCM) * (Animation.iNumChannels));

				//ä�θ�ŭ �ݺ�
				for (_uint j = 0; j < Animation.iNumChannels; ++j)
				{
					CHANNEL_GCM Channel;
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
					ReadFile(hFile, &(Channel.iNumMartixKeys), sizeof(_uint), &dwByte, nullptr);

					// Channel ScalingKeys
					Channel.iMatrixFrame = New MATRIXFRAME_GCM[Channel.iNumMartixKeys];
					ZeroMemory(Channel.iMatrixFrame, sizeof(MATRIXFRAME_GCM) * (Channel.iNumMartixKeys));
					ReadFile(hFile, Channel.iMatrixFrame, sizeof(MATRIXFRAME_GCM) * (Channel.iNumMartixKeys), &dwByte, nullptr);

					Animation.Channels[j] = Channel;
				}

				// Animation NumNotify
				Animation.Notify = New NOTIFY_GCM{};
				//ZeroMemory(Animation.Notify, sizeof(NOTIFY_GCM));

				ReadFile(hFile, &(Animation.Notify->iNumKeyFrames), sizeof(_uint), &dwByte, nullptr);

				for (_uint j = 0; j < Animation.Notify->iNumKeyFrames; ++j)
				{
					_tchar szFrameName[MAX_PATH] = {};
					_int   iType = 0;

					// Animation Name
					ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
					ReadFile(hFile, szFrameName, dwStrByte, &dwByte, nullptr);
					if (0 == dwByte)
					{
						MSG_BOX("Failed Read String Data");
						return E_FAIL;
					}
					ReadFile(hFile, &iType, sizeof(int), &dwByte, nullptr);

					switch (*reinterpret_cast<KEYFRAME_GCM::KEYFRAMETYPE*>(&iType))
					{
					case KEYFRAME_GCM::KF_SPEED:
					{
						SPEEDFRAME_GCM* NotifyFrame = New SPEEDFRAME_GCM;

						lstrcpy(NotifyFrame->szName, szFrameName);
						NotifyFrame->eKeyFrameType = *reinterpret_cast<KEYFRAME_GCM::KEYFRAMETYPE*>(&iType);
						ReadFile(hFile, &NotifyFrame->fTime, sizeof(_float), &dwByte, nullptr);
						ReadFile(hFile, &NotifyFrame->fSpeed, sizeof(_float), &dwByte, nullptr);
						Animation.Notify->tKeyFrame.push_back(NotifyFrame);
						//Animation.Notify->tKeyFrame[j] = NotifyFrame;
						break;
					}
					case KEYFRAME_GCM::KF_NOTIFY:
					{
						NOTIFYFRAME_GCM* NotifyFrame = New NOTIFYFRAME_GCM;

						lstrcpy(NotifyFrame->szName, szFrameName);
						NotifyFrame->eKeyFrameType = *reinterpret_cast<KEYFRAME_GCM::KEYFRAMETYPE*>(&iType);
						ReadFile(hFile, &NotifyFrame->fTime, sizeof(_float), &dwByte, nullptr);
						Animation.Notify->tKeyFrame.push_back(NotifyFrame);
						break;
					}
					case KEYFRAME_GCM::KF_SOUND:
					{
						SOUNDFRAME_GCM* NotifyFrame = New SOUNDFRAME_GCM;

						lstrcpy(NotifyFrame->szName, szFrameName);
						NotifyFrame->eKeyFrameType = *reinterpret_cast<KEYFRAME_GCM::KEYFRAMETYPE*>(&iType);
						ReadFile(hFile, &NotifyFrame->fTime, sizeof(_float), &dwByte, nullptr);
						ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
						ReadFile(hFile, NotifyFrame->wszSoundTag, dwStrByte, &dwByte, nullptr);
						if (0 == dwByte)
						{
							MSG_BOX("Failed Read String Data");
							return E_FAIL;
						}
						Animation.Notify->tKeyFrame.push_back(NotifyFrame);
						break;
					}
					}
				}
				m_AnimationDatasGCM[animPartIndex].push_back(Animation);
			}
		}

	}
	else // NonAnim
	{
		m_ModelGCM.iNumAnimations = 0;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CModel::Ready_Bones_GCM(Engine::NODE Node)
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

HRESULT CModel::Ready_Meshes_GCM(TYPE eType, _float4x4 PivotMatrix)
{
	m_iNumMeshes = m_ModelGCM.iNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eType, m_Bones, m_MeshDatas[i], PivotMatrix);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials_GCM()
{
	m_iNumMaterials = m_ModelGCM.iNumMaterials;

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

//gcm���� ��ȯ�� �ִ������� �ٽ� canimation���� ��ȯ
HRESULT CModel::Ready_Animations_GCM()
{
	m_iAnimationPartCount = m_ModelGCM.iAnimationPartCount;
	m_iRootBoneIndex = m_ModelGCM.iRootBoneIndex;
	for (_uint iAnimTypeIndex = 0; iAnimTypeIndex < m_iAnimationPartCount; iAnimTypeIndex++)
	{
		for (int i = 0; i < m_ModelGCM.iAffectBones[iAnimTypeIndex].size(); i++)
		{
			m_tAnimationDesc[iAnimTypeIndex].AffectBoneVec.push_back(m_ModelGCM.iAffectBones[iAnimTypeIndex][i]);
		}

		m_tAnimationDesc[iAnimTypeIndex].iNumAnimations = m_ModelGCM.iNumAnimations[iAnimTypeIndex];
		for (_uint i = 0; i < m_ModelGCM.iNumAnimations[iAnimTypeIndex]; ++i)
		{
			CAnimation* pAnimation = CAnimation::Create(m_AnimationDatasGCM[iAnimTypeIndex][i], m_Bones);
			if (nullptr == pAnimation)
				return E_FAIL;

			m_tAnimationDesc[iAnimTypeIndex].Animations.push_back(pAnimation);
		}
	}

	return S_OK;
}

HRESULT CModel::Ready_File_Animation(ANIMTYPE eType, const _tchar* pAnimationFilePath)
{
	HANDLE hFile = CreateFile(pAnimationFilePath,
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	_ulong	dwStrByte = 0;

	_uint iNumOfAnim = 0;
	ReadFile(hFile, &iNumOfAnim, sizeof(_uint), &dwByte, nullptr);

	vector<ANIMATION> animVec;

	for (_uint i = 0; i < iNumOfAnim; ++i)
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

		animVec.push_back(Animation);
	}

	Add_Animations(eType, &animVec);
	Release_TempAnimVec(&animVec);
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CModel::Add_Animations(ANIMTYPE eType, vector<ANIMATION>* AnimVec)
{
	// �ִϸ��̼� ���� �߰�
	for (auto& anim : (*AnimVec))
	{
		auto iter = find_if(m_tAnimationDesc[eType].Animations.begin(), m_tAnimationDesc[eType].Animations.end(), [&](CAnimation* animDesc) {
			return (!lstrcmpW(anim.szName, animDesc->Get_AnimationName()));
			});
		if (iter == m_tAnimationDesc[eType].Animations.end())
		{
			CAnimation* pAnimation = CAnimation::Create(anim, m_Bones);
			if (nullptr == pAnimation)
				return E_FAIL;

			m_tAnimationDesc[eType].Animations.push_back(pAnimation);
			m_tAnimationDesc[eType].iNumAnimations++;
		}
	}
	return S_OK;
}

HRESULT CModel::Release_TempAnimVec(vector<ANIMATION>* AnimVec)
{
	for (auto& anim : (*AnimVec))
	{
		for (_uint i = 0; i < anim.iNumChannels; ++i)
		{
			Safe_Delete_Array(anim.Channels[i].ScalingKeys);
			Safe_Delete_Array(anim.Channels[i].RotationKeys);
			Safe_Delete_Array(anim.Channels[i].PositionKeys);
		}
		Safe_Delete_Array(anim.Channels);
	}
	(*AnimVec).clear();
	return S_OK;
}

HRESULT CModel::Write_File_GCM(TYPE eType, const _tchar* pModelFilePath)
{
	m_isExportedTool = true;
	_tchar szPath[MAX_PATH] = TEXT("../../Resources/Models/Anims/");
	lstrcat(szPath, pModelFilePath);
	lstrcat(szPath, TEXT("/"));
	lstrcat(szPath, pModelFilePath);
	lstrcat(szPath, TEXT(".gcm"));

	HANDLE hFile = CreateFile(szPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	_ulong	dwStrByte = 0;

	// Write Nodes

	// Nodes NumNodes
	//ZEROMEM(&m_ModelGCM);
	WriteFile(hFile, &(m_ModelGCM.iNumNodes), sizeof(_uint), &dwByte, nullptr);

	for (auto& Node : m_NodeDatas)
	{
		// Node Name
		dwStrByte = sizeof(_tchar) * (lstrlen(Node.szName) + 1);
		WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		WriteFile(hFile, Node.szName, dwStrByte, &dwByte, nullptr);

		// Node Transformation
		WriteFile(hFile, &(Node.TransformationMatrix), sizeof(_float4x4), &dwByte, nullptr);

		// Node NodeIndex
		WriteFile(hFile, &(Node.iNodeIndex), sizeof(_uint), &dwByte, nullptr);

		// Node Parent
		WriteFile(hFile, &(Node.iParent), sizeof(_int), &dwByte, nullptr);

		// Node NumChildren
		WriteFile(hFile, &(Node.iNumChildren), sizeof(_uint), &dwByte, nullptr);

		// Node Children (array)
		WriteFile(hFile, Node.iChildrens, sizeof(_uint) * Node.iNumChildren, &dwByte, nullptr);
	}

	// Write Meshes

	// Meshes NumMeshes
	WriteFile(hFile, &(m_ModelGCM.iNumMeshes), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_ModelGCM.iNumMeshes; ++i)
	{
		MESH Mesh = m_MeshDatas[i];

		// Mesh Name
		dwStrByte = sizeof(_tchar) * (lstrlen(Mesh.szName) + 1);
		WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		WriteFile(hFile, Mesh.szName, dwStrByte, &dwByte, nullptr);

		// Mesh MaterialIndex
		WriteFile(hFile, &(Mesh.iMaterialIndex), sizeof(_uint), &dwByte, nullptr);

		// Mesh NumVertices
		WriteFile(hFile, &(Mesh.iNumVertices), sizeof(_uint), &dwByte, nullptr);

		// Mesh NumFaces
		WriteFile(hFile, &(Mesh.iNumFaces), sizeof(_uint), &dwByte, nullptr);

		for (_uint j = 0; j < Mesh.iNumFaces; ++j)
		{
			FACE Face = Mesh.Faces[j];
			// Face NumIndices
			WriteFile(hFile, &(Face.iNumIndices), sizeof(_uint), &dwByte, nullptr);

			// Face Indices
			WriteFile(hFile, Face.iIndices, sizeof(_uint) * Face.iNumIndices, &dwByte, nullptr);
		}

		// Mesh Positions
		WriteFile(hFile, Mesh.vPositions, sizeof(_float3) * Mesh.iNumVertices, &dwByte, nullptr);

		// Mesh Normals
		WriteFile(hFile, Mesh.vNormals, sizeof(_float3) * Mesh.iNumVertices, &dwByte, nullptr);

		// Mesh TexCoords
		WriteFile(hFile, Mesh.vTexCoords, sizeof(_float2) * Mesh.iNumVertices, &dwByte, nullptr);

		// Mesh Tangents
		WriteFile(hFile, Mesh.vTangents, sizeof(_float3) * Mesh.iNumVertices, &dwByte, nullptr);

		// Mesh NumBones
		WriteFile(hFile, &(Mesh.iNumBones), sizeof(_uint), &dwByte, nullptr);

		// Write Bones
		for (_uint j = 0; j < Mesh.iNumBones; j++)
		{
			BONE Bone = Mesh.Bones[j];

			// Bone Name
			dwStrByte = (_ulong)sizeof(_tchar) * (lstrlen(Bone.szName) + 1);
			WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
			WriteFile(hFile, Bone.szName, dwStrByte, &dwByte, nullptr);

			// Mesh OffsetMatrix
			WriteFile(hFile, &(Bone.OffsetMatrix), sizeof(_float4x4), &dwByte, nullptr);

			// Mesh NumWeights
			WriteFile(hFile, &(Bone.iNumWeights), sizeof(_uint), &dwByte, nullptr);

			// Write Weights
			for (_uint k = 0; k < Bone.iNumWeights; k++)
			{
				WEIGHT Weight = Bone.Weights[k];

				// Weight VertexId
				WriteFile(hFile, &(Weight.iVertexId), sizeof(_uint), &dwByte, nullptr);

				// Weight Weight
				WriteFile(hFile, &(Weight.fWeight), sizeof(_float), &dwByte, nullptr);
			}
		}
	}

	// Write Materials

	// Material NumMaterials
	WriteFile(hFile, &(m_ModelGCM.iNumMaterials), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_ModelGCM.iNumMaterials; ++i)
	{
		MATERIAL Material = m_MaterialDatas[i];

		// MaterialTex
		WriteFile(hFile, Material.MaterialTexture, sizeof(MATERIALTEX) * TextureType_MAX, &dwByte, nullptr);
	}

	//��Ʈ �� ����
	WriteFile(hFile, &(m_ModelGCM.iRootBoneIndex), sizeof(_uint), &dwByte, nullptr);

	// Write Animations
	if (TYPE_ANIM == eType)
	{
		// Animation NumAnimations
		WriteFile(hFile, &(m_ModelGCM.iAnimationPartCount), sizeof(_uint), &dwByte, nullptr);

		//AffectBone Write
		for (_uint partCnt = 0; partCnt < m_iAnimationPartCount; partCnt++)
		{
			_uint size = m_ModelGCM.iAffectBones[partCnt].size();
			WriteFile(hFile, &size, sizeof(_uint), &dwByte, nullptr);
			for (int i = 0; i < m_ModelGCM.iAffectBones[partCnt].size(); i++)
			{
				WriteFile(hFile, &(m_ModelGCM.iAffectBones[partCnt][i]), sizeof(_uint), &dwByte, nullptr);
			}
		}

		for (_uint animPartIndex = 0; animPartIndex < m_ModelGCM.iAnimationPartCount; animPartIndex++)
		{
			WriteFile(hFile, &(m_ModelGCM.iNumAnimations[animPartIndex]), sizeof(_uint), &dwByte, nullptr);
			for (_uint i = 0; i < m_ModelGCM.iNumAnimations[animPartIndex]; ++i)
			{
				ANIMATION_GCM Animation = m_AnimationDatasGCM[animPartIndex][i];

				// Animation Name
				dwStrByte = (_ulong)sizeof(_tchar) * (lstrlen(Animation.szName) + 1);
				WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
				WriteFile(hFile, Animation.szName, dwStrByte, &dwByte, nullptr);

				// Animation Duration
				WriteFile(hFile, &(Animation.fDuration), sizeof(_float), &dwByte, nullptr);

				// Animation TickPerSecond
				WriteFile(hFile, &(Animation.fTickPerSecond), sizeof(_float), &dwByte, nullptr);

				// Animation OptionSetting
				WriteFile(hFile, &(Animation.isLoop), sizeof(_bool), &dwByte, nullptr);
				WriteFile(hFile, &(Animation.isRootAnim), sizeof(_bool), &dwByte, nullptr);
				WriteFile(hFile, &(Animation.isLerp), sizeof(_bool), &dwByte, nullptr);

				WriteFile(hFile, &(Animation.vOffsetPosition), sizeof(XMFLOAT3), &dwByte, nullptr);


				// Animation NumChannels
				WriteFile(hFile, &(Animation.iNumChannels), sizeof(_uint), &dwByte, nullptr);

				for (_uint j = 0; j < Animation.iNumChannels; ++j)
				{
					CHANNEL_GCM Channel = Animation.Channels[j];

					// Channel Name
					dwStrByte = (_ulong)sizeof(_tchar) * (lstrlen(Channel.szName) + 1);
					WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
					WriteFile(hFile, Channel.szName, dwStrByte, &dwByte, nullptr);

					// Channel NumMatrixKeys
					WriteFile(hFile, &(Channel.iNumMartixKeys), sizeof(_uint), &dwByte, nullptr);

					// Channel MatrixKeys
					WriteFile(hFile, Channel.iMatrixFrame, sizeof(MATRIXFRAME_GCM) * (Channel.iNumMartixKeys), &dwByte, nullptr);
				}

				//��Ƽ���̰� �󸶳� �����ϴ���?
				WriteFile(hFile, &(Animation.Notify->iNumKeyFrames), sizeof(_uint), &dwByte, nullptr);

				//�� ��Ƽ������ �������� �����ϴ� ģ���� ���� ��.
				for (_uint j = 0; j < Animation.Notify->iNumKeyFrames; ++j)
				{
					KEYFRAME_GCM* Keyframe = Animation.Notify->tKeyFrame[j];

					// Channel Name
					dwStrByte = (_ulong)sizeof(_tchar) * (lstrlen(Keyframe->szName) + 1);
					WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
					WriteFile(hFile, Keyframe->szName, dwStrByte, &dwByte, nullptr);

					//�̳� �����ε� ���� �� �ҷ��������� �����ҵ�.
					WriteFile(hFile, &(Keyframe->eKeyFrameType), sizeof(int), &dwByte, nullptr);
					WriteFile(hFile, &(Keyframe->fTime), sizeof(_float), &dwByte, nullptr);

					switch (Keyframe->eKeyFrameType)
					{
					case KEYFRAME::KF_SPEED:
						WriteFile(hFile, &reinterpret_cast<SPEEDFRAME_GCM*>(Keyframe)->fSpeed, sizeof(float), &dwByte, nullptr);
						break;

					case KEYFRAME::KF_SOUND:
						dwStrByte = (_ulong)sizeof(_tchar) * (lstrlen(reinterpret_cast<SOUNDFRAME_GCM*>(Keyframe)->wszSoundTag) + 1);
						WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
						WriteFile(hFile, reinterpret_cast<SOUNDFRAME_GCM*>(Keyframe)->wszSoundTag, dwStrByte, &dwByte, nullptr);
						break;

					case KEYFRAME::KF_NOTIFY:
						break;
					}
				}
			}
		}

	}

	CloseHandle(hFile);

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

void CModel::Release_FileDatas_GCM()
{
	//������ŭ �ݺ�
	Safe_Delete_Array(m_ModelGCM.iAffectBones);

	for (auto& AnimDataGCM : m_AnimationDatasGCM)
	{
		//�ִϸ��̼Ǹ�ŭ �ݺ�
		for (auto& Animation : AnimDataGCM)
		{
			//ä�� ����
			for (_uint i = 0; i < Animation.iNumChannels; ++i)
			{
				Safe_Delete_Array(Animation.Channels[i].iMatrixFrame);
			}
			Safe_Delete_Array(Animation.Channels);
			//��Ƽ���� ����
			for (_uint i = 0; i < Animation.Notify->iNumKeyFrames; ++i)
			{
				Safe_Delete(Animation.Notify->tKeyFrame[i]);
			}
			Safe_Delete(Animation.Notify);
		}

		AnimDataGCM.clear();
	}
	Safe_Delete_Array(m_ModelGCM.iNumAnimations);
}

_uint CModel::Find_Animation_Index(const wstring& wstrTag, ANIMTYPE eType) const
{
	_uint iAnimationIndex = { 0 };

	for (auto pAnimation : m_tAnimationDesc[eType].Animations)
	{
		wstring wstrAnimationTag = pAnimation->Get_AnimationName();

		if (wstring::npos != wstrAnimationTag.find(wstrTag))
			break;

		++iAnimationIndex;
	}

	return iAnimationIndex;
}

void CModel::Reset_Animation(ANIMTYPE eType)
{
	if (true == m_tAnimationDesc[eType].isResetAnimTrigger)
	{
		// �ִϸ��̼� ����
		m_tAnimationDesc[eType].Animations[m_tAnimationDesc[eType].iCurrentAnimIndex]->Reset();

		// ���� �ִϸ��̼ǿ� ������ �����Ǿ������� ���� ����
		if (m_tAnimationDesc[eType].Animations[m_tAnimationDesc[eType].iCurrentAnimIndex]->Get_LerpAnim())
		{
			m_tAnimationDesc[eType].isAnimChangeLerp = true;
			m_tAnimationDesc[eType].fAnimChangeTimer = ANIMATIONLERPTIME;
		}

		// 0�����(��ü)���? ��Ʈ ��Ʈ���� �ʱ�ȭ
		if (eType == 0)
			m_PostRootMatrix = XMMatrixIdentity();

		// ���¼��� �ٵ����ϱ� Ʈ���� ����.
		m_tAnimationDesc[eType].isResetAnimTrigger = false;
	}
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, _float4x4 PivotMatrix)
{
	CModel* pInstance = New CModel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PivotMatrix)))
	{
		__debugbreak();
		MSG_BOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = New CModel(*this);

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
		if (m_isCreatedByGCM)
			Release_FileDatas_GCM();
	}

	//�ͽ���Ʈ�� ��쿡�� ��ü�� �ƴ� Ŭ�п� gcm�� ���ŵǹǷ� �����������.
	if (m_isExportedTool/*&&!m_isCreatedByGCM*/)
		Release_FileDatas_GCM();

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
