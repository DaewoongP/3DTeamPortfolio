#include "..\Public\Model_Converter.h"

CModel_Converter::CModel_Converter()
{
	ZEROMEM(&m_Model);
}

HRESULT CModel_Converter::Convert_Model(_uint iType, const _char* pModelFilePath)
{
	TYPE eType = TYPE(iType);
	_uint		iFlag = 0;

	if (TYPE_ANIM == eType)
		iFlag = aiProcess_GlobalScale | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_PreTransformVertices | aiProcess_GlobalScale | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	// importer���� ������ ����
	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);

	if (nullptr == m_pAIScene)
	{
		MSG_BOX("Failed (Importer Read File)");
		return E_FAIL;
	}

	cout << "Convert Bones..." << endl;
	if (FAILED(Convert_Bones(m_pAIScene->mRootNode, 0, nullptr, true)))
	{
		MSG_BOX("Failed (Convert_Bones)");
		return E_FAIL;
	}
	
	Sort_Bones();
	
	cout << "Convert Meshes..." << endl;
	if (FAILED(Convert_Meshes()))
	{
		MSG_BOX("Failed (Convert_Meshes)");
		return E_FAIL;
	}

	_tchar szFileName[MAX_PATH] = TEXT("");
	_tchar FullPath[MAX_PATH] = TEXT("");
	CharToWChar(pModelFilePath, FullPath);
	_wsplitpath_s(FullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	_tchar szPath[MAX_PATH] = TEXT("../../Resources/Models/");
	wstring szSubFileName;
	if (wcswcs(szFileName, TEXT("Lod")))
	{
		szSubFileName = szFileName;
		szSubFileName = szSubFileName.substr(0, szSubFileName.size() - 5);
	}
	else
	{
		szSubFileName = szFileName;
	}

	// Write Anim
	if (TYPE_ANIM == eType)
	{
		lstrcat(szPath, TEXT("Anims/"));
		lstrcat(szPath, szSubFileName.c_str());
		lstrcat(szPath, TEXT("/"));
		fs::create_directory(szPath);
	}
	// Write NonAnim
	else if(TYPE_NONANIM == eType)
	{
		lstrcat(szPath, TEXT("NonAnims/"));
		lstrcat(szPath, szSubFileName.c_str());
		lstrcat(szPath, TEXT("/"));
		fs::create_directory(szPath);
	}
	// Write MapObject
	else if (TYPE_MAPOBJECT == eType)
	{
		lstrcat(szPath, TEXT("MapObject/NonAnims/"));
		lstrcat(szPath, szFileName);

		// ���� �� Lod �� �� ��� �����ؾ� �ȴ�.
		wstring ws(szPath);
		if (std::string::npos != ws.find(TEXT("_Lod")))
		{
			size_t findLod = ws.find(TEXT("_Lod"));
			ws = ws.substr(0, findLod);
			lstrcpy(szPath, ws.c_str());
			lstrcat(szPath, TEXT("/"));
			fs::create_directory(szPath);
		}

		else
		{
			lstrcat(szPath, TEXT("/"));
			fs::create_directory(szPath);
		}		
	}


	cout << "Convert Materials..." << endl;
	if (FAILED(Convert_Materials(eType, pModelFilePath, szPath)))
	{
		MSG_BOX("Failed (Convert_Materials)");
		return E_FAIL;
	}

	if (TYPE_ANIM == eType)
	{
		// �ִϸ��̼� �����ϰ�쿡�� ����.
		cout << "Convert Animations..." << endl;
		if (FAILED(Convert_Animations()))
		{
			MSG_BOX("Failed (Convert_Animations)");
			return E_FAIL;
		}
	}

	cout << "Writing Files..." << endl;
	// fbx���� üũ

	_tchar fileName[MAX_PATH] = {};
	_wsplitpath_s(FullPath, nullptr, 0, nullptr, 0, fileName, MAX_PATH, nullptr, 0);

	size_t fileNameLength = wcslen(fileName);
	const wchar_t* suffix = L"_COL";

	if ((fileNameLength >= wcslen(suffix) && wcscmp(fileName + fileNameLength - wcslen(suffix), suffix) == 0) ? FAILED(Write_File_COL(eType, szPath, szFileName)) :FAILED(Write_File(eType, szPath, szFileName)))
	{
		MSG_BOX("Failed (Write_File)");
		return E_FAIL;
	}

	cout << "Convert Success!" << endl;
	cout << "=====================================" << endl;

	return S_OK;
}

HRESULT CModel_Converter::Convert_Bones(aiNode* pNode, _uint iParentIndex, _uint* iChildIndex, _bool isRoot)
{
	// ���Ͽ� ������ ��� ����ü
	NODE Node;
	ZEROMEM(&Node);

	// �� �̸�
	_tchar BoneName[256] = TEXT("");
	CharToWChar(pNode->mName.data, BoneName);
	lstrcpy(Node.szName, BoneName);

	// ����� ��������� �޾ƿͼ� ��ġ ��Ŵ (Col Major -> Row Major)
	// �⺻������ assimp ���̺귯���� Col Major ��ķ� ����ϱ� ����.
	_float4x4 TransposeMatrix;
	memcpy(&TransposeMatrix, &pNode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&TransposeMatrix, XMMatrixTranspose(XMLoadFloat4x4(&TransposeMatrix)));
	memcpy(&Node.TransformationMatrix, &TransposeMatrix, sizeof _float4x4);

	// Root Check
	if (true == isRoot)
	{
		// Parent except
		Node.iParent = -1;
	}
	else
	{
		// �ڽ� �ε��� �Ҵ�
		*iChildIndex = ++m_iCurrentNodeIndex;

		Node.iParent = iParentIndex;
	}

	// ����ü�� �ε���, �ڽ� ���� �� ����.
	Node.iNodeIndex = m_iCurrentNodeIndex;
	Node.iNumChildren = pNode->mNumChildren;

	// �ڽ��� ���� ��� �ڽĺ����� �����Ҵ�.
	if (0 != pNode->mNumChildren)
	{
		Node.iChildrens = new _uint[pNode->mNumChildren];
		ZeroMemory(Node.iChildrens, sizeof(_uint) * pNode->mNumChildren);
	}

	// �ڽ� ������ŭ ��ȸ�ϸ� ����Լ� ȣ���Ͽ� ���� ��Ӱ��� ó��.
	for (_uint i = 0; i < pNode->mNumChildren; i++)
	{
		_uint iRetIndex = { 0 };
		Convert_Bones(pNode->mChildren[i], Node.iNodeIndex, &iRetIndex);
		Node.iChildrens[i] = iRetIndex;
	}

	// ������ ���������̳ʿ� ����.
	m_Nodes.push_back(Node);

	return S_OK;
}

HRESULT CModel_Converter::Sort_Bones()
{
	sort(m_Nodes.begin(), m_Nodes.end(), [](NODE& Sour, NODE& Dest) {

		if (Sour.iNodeIndex < Dest.iNodeIndex)
			return true;
		else
			return false;
		});

	// �𵨿� ��� ���� ����.
	m_Model.iNumNodes = (_uint)m_Nodes.size();

	return S_OK;
}

HRESULT CModel_Converter::Convert_Meshes()
{
	// �޽����� ����ü ����
	m_Model.iNumMeshes = m_pAIScene->mNumMeshes;

	// �޽� ��ü�� �����Ҵ��Ͽ� �޽� ������ŭ �������.
	m_pMesh = new MESH[m_pAIScene->mNumMeshes];
	ZeroMemory(m_pMesh, sizeof(MESH) * m_pAIScene->mNumMeshes);

	// �޽� ������ŭ ��ȸ�ϸ� �޽��� �ش��ϴ� ������ �������
	for (_uint i = 0; i < m_pAIScene->mNumMeshes; ++i)
	{
		// Mesh
		MESH Mesh;
		ZEROMEM(&Mesh);

		// �޽��� ������ ���·� �Ѱ��־� ������ ��� �Ҵ� �Ͽ� ä�� ����.
		if (FAILED(Store_Mesh(m_pAIScene->mMeshes[i], &Mesh)))
		{
			MSG_BOX("Failed Store Mesh");
			return E_FAIL;
		}

		// �޽� �迭�� ����.
		m_pMesh[i] = Mesh;
	}

	return S_OK;
}

HRESULT CModel_Converter::Store_Mesh(const aiMesh* pAIMesh, MESH* outMesh)
{
	// ���׸��� �ε��� ���� ����
	outMesh->iMaterialIndex = pAIMesh->mMaterialIndex;

	// �޽� �̸�
	_tchar MeshName[256] = TEXT("");
	CharToWChar(pAIMesh->mName.data, MeshName);
	lstrcpy(outMesh->szName, MeshName);

	// ���� ������ ����
	outMesh->iNumVertices = pAIMesh->mNumVertices;
	outMesh->iNumFaces = pAIMesh->mNumFaces;

	// �޽� ǥ�� �����Ҵ��Ͽ� ó��
	outMesh->Faces = new FACE[outMesh->iNumFaces];
	ZeroMemory(outMesh->Faces, sizeof(FACE) * outMesh->iNumFaces);

	// �޽� ǥ���� ��ȸ�ϸ� �޽��� �ε����� ����
	for (_uint i = 0; i < pAIMesh->mNumFaces; ++i)
	{
		outMesh->Faces[i].iNumIndices = pAIMesh->mFaces[i].mNumIndices;

		outMesh->Faces[i].iIndices = new _uint[outMesh->Faces[i].iNumIndices];
		ZeroMemory(outMesh->Faces[i].iIndices, sizeof(_uint) * outMesh->Faces[i].iNumIndices);

		for (_uint j = 0; j < outMesh->Faces[i].iNumIndices; ++j)
		{
			outMesh->Faces[i].iIndices[j] = pAIMesh->mFaces[i].mIndices[j];
		}
	}

	// ���ؽ��� ����� ���� ������, �븻, UV��ǥ, ź��Ʈ�� ����� ���� �����Ҵ�.
	outMesh->vPositions = new _float3[pAIMesh->mNumVertices];
	ZeroMemory(outMesh->vPositions, sizeof(_float3) * pAIMesh->mNumVertices);

	outMesh->vNormals = new _float3[pAIMesh->mNumVertices];
	ZeroMemory(outMesh->vNormals, sizeof(_float3) * pAIMesh->mNumVertices);

	outMesh->vTexCoords = new _float2[pAIMesh->mNumVertices];
	ZeroMemory(outMesh->vTexCoords, sizeof(_float2) * pAIMesh->mNumVertices);

	outMesh->vTangents = new _float3[pAIMesh->mNumVertices];
	ZeroMemory(outMesh->vTangents, sizeof(_float3) * pAIMesh->mNumVertices);

	// ���� ���ؽ� ��ȸ�ϸ� ��� ���� ����.
	for (_uint i = 0; i < pAIMesh->mNumVertices; i++)
	{
		memcpy(&outMesh->vPositions[i], &pAIMesh->mVertices[i], sizeof _float3);
		memcpy(&outMesh->vNormals[i], &pAIMesh->mNormals[i], sizeof _float3);
		memcpy(&outMesh->vTexCoords[i], &pAIMesh->mTextureCoords[0][i], sizeof _float2);
		memcpy(&outMesh->vTangents[i], &pAIMesh->mTangents[i], sizeof _float3);
	}

	// �޽��� ���� ���� ����
	outMesh->iNumBones = pAIMesh->mNumBones;

	// �� ��ü �����Ҵ�
	outMesh->Bones = new BONE[pAIMesh->mNumBones];
	ZeroMemory(outMesh->Bones, sizeof(BONE) * pAIMesh->mNumBones);

	// �޽��� ������ ��ȸ�ϸ� ���� ������ ������
	for (_uint i = 0; i < pAIMesh->mNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		BONE	Bone;
		ZEROMEM(&Bone);
		// �ش��ϴ� ���� �̸�
		_tchar BoneName[256] = TEXT("");
		CharToWChar(pAIBone->mName.data, BoneName);
		lstrcpy(Bone.szName, BoneName);

		// assimp ���̺귯���� �⺻������ ColMajor �̹Ƿ� Row Major ���·� ��ġ�Ͽ� ����.
		_float4x4 TransposeMatrix;
		memcpy(&TransposeMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&TransposeMatrix, XMMatrixTranspose(XMLoadFloat4x4(&TransposeMatrix)));
		memcpy(&Bone.OffsetMatrix, &TransposeMatrix, sizeof _float4x4);

		// �޽� ���� weight��
		Bone.iNumWeights = pAIBone->mNumWeights;

		Bone.Weights = new WEIGHT[pAIBone->mNumWeights];
		ZeroMemory(Bone.Weights, sizeof(WEIGHT) * pAIBone->mNumWeights);

		// �����Ҵ��� weight�� ��ȸ�ϸ� ���ؽ� ���̵�� weight ���� ����.
		for (_uint j = 0; j < pAIBone->mNumWeights; j++)
		{
			aiVertexWeight	AIVertexWeight = pAIBone->mWeights[j];

			WEIGHT Weight;
			ZEROMEM(&Weight);

			Weight.iVertexId = AIVertexWeight.mVertexId;
			Weight.fWeight = AIVertexWeight.mWeight;

			// Mesh Bone Weight ����
			Bone.Weights[j] = Weight;
		}

		// Mesh Bone ����
		outMesh->Bones[i] = Bone;
	}

	return S_OK;
}

HRESULT CModel_Converter::Convert_Materials(TYPE eType, const char* pModelFilePath, const _tchar* pSaveDirectory)
{
	// ���׸��� ����
	m_Model.iNumMaterials = m_pAIScene->mNumMaterials;

	// ���׸��� �����Ҵ��Ͽ� ����
	m_pMaterial = new MATERIAL[m_pAIScene->mNumMaterials];
	ZeroMemory(m_pMaterial, sizeof(MATERIAL) * m_pAIScene->mNumMaterials);

	// ���� ���׸��� ��ȸ
	for (_uint i = 0; i < m_pAIScene->mNumMaterials; ++i)
	{
		MATERIAL Material;
		ZEROMEM(&Material);

		// �ش��ϴ� ���׸����� �ؽ�ó ��� ��ȸ�ϸ� ����
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString	aistrPath;

			if (FAILED(m_pAIScene->mMaterials[i]->GetTexture(aiTextureType(j), 0, &aistrPath)))
				continue;

			_char pPath[MAX_STR] = "";
			strcpy_s(pPath, MAX_STR, aistrPath.data);
			string TexturePath = pPath;

			_char szDrivePath[MAX_PATH] = "";
			_char szModelPath[MAX_PATH] = "";
			_splitpath_s(pModelFilePath, szDrivePath, MAX_PATH, szModelPath, MAX_PATH, nullptr, 0, nullptr, 0);

			string DrivePath = szDrivePath;
			string ModelDirPath = szModelPath;

			// �������ø� ����� ��� �ڿ� ���� �ٴ� ���ڿ��� ������ ���� �� �־� ������ ���·� ����
			size_t iOffset = string::npos;
			while (string::npos != (iOffset = ModelDirPath.find("\\")))
			{
				ModelDirPath.replace(iOffset, 1, "/");
			}

			while (string::npos != (iOffset = TexturePath.find("\\")))
			{
				TexturePath.replace(iOffset, 1, "/");
			}

			while (string::npos != TexturePath.find("../"))
			{
				size_t DirSlashIndex = ModelDirPath.rfind("/");

				ModelDirPath = ModelDirPath.substr(0, DirSlashIndex);

				DirSlashIndex = ModelDirPath.rfind("/");

				ModelDirPath = ModelDirPath.substr(0, DirSlashIndex + 1);

				TexturePath = TexturePath.substr(3);
			}

			// ������ ��ε� strcat
			string FullPath = DrivePath + ModelDirPath + TexturePath;
			
			std::error_code ec;
			// skip_existing, overwrite_existing, update_existing 
			// ���� ������ �����Ұ�� ������ ���Ƿ� �ɼ� �����.
			fs::copy(FullPath, pSaveDirectory, fs::copy_options::skip_existing, ec);

			// �ؽ�ó Ÿ�� ����
			_char strFileName[MAX_PATH] = "";
			_char strFileExt[MAX_PATH] = "";
			_splitpath_s(FullPath.c_str(), nullptr, 0, nullptr, 0, strFileName, MAX_PATH, strFileExt, MAX_PATH);
			
			strcat_s(strFileName, strFileExt);
			_tchar	szTextureName[MAX_PATH] = TEXT("");
			_tchar	szTexturePath[MAX_PATH] = TEXT("");
			CharToWChar(strFileName, szTextureName);
			lstrcat(szTexturePath, pSaveDirectory);

			/*if (eType != TYPE_MAPOBJECT)
			{*/
				lstrcat(szTexturePath, szTextureName);

				lstrcpy(Material.MaterialTexture[j].szTexPath, szTexturePath);
				Material.MaterialTexture[j].TexType = TEXTYPE(j);
			//}				
			//else // �� ������Ʈ�� ��� ��θ� �ٸ��� �����Ѵ�.
			//{
			//	wstring wsTexturePath(szTexturePath);
			//	wstring wsTargetIndex(TEXT("MapObject/"));
			//	size_t Targetsize = wsTargetIndex.size();
			//	size_t FindIndex = wsTexturePath.find(wsTargetIndex);
			//	wstring wsNewTexturePath = wsTexturePath.substr(0, FindIndex + Targetsize);
			//	wsNewTexturePath += TEXT("Texture/");

			//	_tchar	szNewTexturePath[MAX_PATH] = TEXT("");
			//	lstrcpy(szNewTexturePath, wsNewTexturePath.c_str());
			//	lstrcat(szNewTexturePath, szTextureName);

			//	lstrcpy(Material.MaterialTexture[j].szTexPath, szNewTexturePath);
			//	Material.MaterialTexture[j].TexType = TEXTYPE(j);
			//}			
		}

		// ���׸��� ����
		m_pMaterial[i] = Material;
	}

	return S_OK;
}

HRESULT CModel_Converter::Convert_Animations()
{
	m_Model.iNumAnimations = m_pAIScene->mNumAnimations;

	m_pAnimation = new ANIMATION[m_pAIScene->mNumAnimations];
	ZeroMemory(m_pAnimation, sizeof(ANIMATION) * m_pAIScene->mNumAnimations);

	// �ִϸ��̼� ��ȸ
	for (_uint i = 0; i < m_pAIScene->mNumAnimations; ++i)
	{
		aiAnimation* pAIAnimation = m_pAIScene->mAnimations[i];

		ANIMATION Animation;
		ZEROMEM(&Animation);
		// �ִϸ��̼� �̸�
		_tchar AnimName[256] = TEXT("");
		CharToWChar(pAIAnimation->mName.data, AnimName);
		lstrcpy(Animation.szName, AnimName);

		// �ִϸ��̼� ���� ����
		Animation.fDuration = _float(pAIAnimation->mDuration);
		Animation.fTickPerSecond = _float(pAIAnimation->mTicksPerSecond);
		Animation.iNumChannels = pAIAnimation->mNumChannels;

		Animation.Channels = new CHANNEL[pAIAnimation->mNumChannels];
		ZeroMemory(Animation.Channels, sizeof(CHANNEL) * pAIAnimation->mNumChannels);

		// �ִϸ��̼� ä�� ��ȸ
		for (_uint j = 0; j < pAIAnimation->mNumChannels; ++j)
		{
			aiNodeAnim* pAIChannel = pAIAnimation->mChannels[j];

			CHANNEL Channel;
			ZEROMEM(&Channel);
			// ä�� �̸�
			_tchar ChannelName[256] = TEXT("");
			CharToWChar(pAIChannel->mNodeName.data, ChannelName);
			lstrcpy(Channel.szName, ChannelName);

			// ä���� ������ Ű ����
			Channel.iNumScalingKeys = pAIChannel->mNumScalingKeys;
			Channel.ScalingKeys = new VECTORKEY[pAIChannel->mNumScalingKeys];
			ZeroMemory(Channel.ScalingKeys, sizeof(VECTORKEY) * pAIChannel->mNumScalingKeys);

			for (_uint k = 0; k < pAIChannel->mNumScalingKeys; ++k)
			{
				Channel.ScalingKeys[k].fTime = _float(pAIChannel->mScalingKeys[k].mTime);
				memcpy(&Channel.ScalingKeys[k].vValue, &pAIChannel->mScalingKeys[k].mValue, sizeof _float3);
			}

			// ä���� �����̼� Ű ���� (���ʹϾ� ����)
			Channel.iNumRotationKeys = pAIChannel->mNumRotationKeys;
			Channel.RotationKeys = new QUATERNIONKEY[pAIChannel->mNumRotationKeys];
			ZeroMemory(Channel.RotationKeys, sizeof(QUATERNIONKEY) * pAIChannel->mNumRotationKeys);

			for (_uint k = 0; k < pAIChannel->mNumRotationKeys; ++k)
			{
				Channel.RotationKeys[k].fTime = _float(pAIChannel->mRotationKeys[k].mTime);

				Channel.RotationKeys[k].vValue.x = pAIChannel->mRotationKeys[k].mValue.x;
				Channel.RotationKeys[k].vValue.y = pAIChannel->mRotationKeys[k].mValue.y;
				Channel.RotationKeys[k].vValue.z = pAIChannel->mRotationKeys[k].mValue.z;
				Channel.RotationKeys[k].vValue.w = pAIChannel->mRotationKeys[k].mValue.w;
			}

			// ä���� ������ Ű ����
			Channel.iNumPositionKeys = pAIChannel->mNumPositionKeys;
			Channel.PositionKeys = new VECTORKEY[pAIChannel->mNumPositionKeys];
			ZeroMemory(Channel.PositionKeys, sizeof(VECTORKEY) * pAIChannel->mNumPositionKeys);

			for (_uint k = 0; k < pAIChannel->mNumPositionKeys; ++k)
			{
				Channel.PositionKeys[k].fTime = _float(pAIChannel->mPositionKeys[k].mTime);
				memcpy(&Channel.PositionKeys[k].vValue, &pAIChannel->mPositionKeys[k].mValue, sizeof _float3);
			}

			// ä�� ����
			Animation.Channels[j] = Channel;
		}

		// �ִϸ��̼� ����
		m_pAnimation[i] = Animation;
	}

	return S_OK;
}

HRESULT CModel_Converter::Write_File(TYPE eType, const _tchar* pSaveDirectory, const _tchar* pFileName)
{
	_tchar szPath[MAX_PATH] = TEXT("");
	lstrcpy(szPath, pSaveDirectory);

	lstrcat(szPath, pFileName);
	lstrcat(szPath, TEXT(".dat"));

	HANDLE hFile = CreateFile(szPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	_ulong	dwStrByte = 0;

	// Write Nodes

	// Nodes NumNodes
	WriteFile(hFile, &(m_Model.iNumNodes), sizeof(_uint), &dwByte, nullptr);

	for (auto& Node : m_Nodes)
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
	WriteFile(hFile, &(m_Model.iNumMeshes), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_Model.iNumMeshes; ++i)
	{
		MESH Mesh = m_pMesh[i];

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
	WriteFile(hFile, &(m_Model.iNumMaterials), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_Model.iNumMaterials; ++i)
	{
		MATERIAL Material = m_pMaterial[i];

		// MaterialTex
		WriteFile(hFile, Material.MaterialTexture, sizeof(MATERIALTEX) * TextureType_MAX, &dwByte, nullptr);
	}

	// Write Animations
	if (TYPE_ANIM == eType)
	{
		// Animation NumAnimations
		WriteFile(hFile, &(m_Model.iNumAnimations), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < m_Model.iNumAnimations; ++i)
		{
			ANIMATION Animation = m_pAnimation[i];

			// Animation Name
			dwStrByte = (_ulong)sizeof(_tchar) * (lstrlen(Animation.szName) + 1);
			WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
			WriteFile(hFile, Animation.szName, dwStrByte, &dwByte, nullptr);

			// Animation Duration
			WriteFile(hFile, &(Animation.fDuration), sizeof(_float), &dwByte, nullptr);

			// Animation TickPerSecond
			WriteFile(hFile, &(Animation.fTickPerSecond), sizeof(_float), &dwByte, nullptr);

			// Animation NumChannels
			WriteFile(hFile, &(Animation.iNumChannels), sizeof(_uint), &dwByte, nullptr);

			for (_uint j = 0; j < Animation.iNumChannels; ++j)
			{
				CHANNEL Channel = Animation.Channels[j];

				// Channel Name
				dwStrByte = (_ulong)sizeof(_tchar) * (lstrlen(Channel.szName) + 1);
				WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
				WriteFile(hFile, Channel.szName, dwStrByte, &dwByte, nullptr);

				// Channel NumScalingKeys
				WriteFile(hFile, &(Channel.iNumScalingKeys), sizeof(_uint), &dwByte, nullptr);

				// Channel ScalingKeys
				WriteFile(hFile, Channel.ScalingKeys, sizeof(VECTORKEY) * (Channel.iNumScalingKeys), &dwByte, nullptr);

				// Channel NumRotationKeys
				WriteFile(hFile, &(Channel.iNumRotationKeys), sizeof(_uint), &dwByte, nullptr);

				// Channel RotationKeys
				WriteFile(hFile, Channel.RotationKeys, sizeof(QUATERNIONKEY) * (Channel.iNumRotationKeys), &dwByte, nullptr);

				// Channel NumPositionKeys
				WriteFile(hFile, &(Channel.iNumPositionKeys), sizeof(_uint), &dwByte, nullptr);

				// Channel PositionKeys
				WriteFile(hFile, Channel.PositionKeys, sizeof(VECTORKEY) * (Channel.iNumPositionKeys), &dwByte, nullptr);
			}
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CModel_Converter::Write_File_COL(TYPE eType, const _tchar* pSaveDirectory, const _tchar* pFileName)
{
	_tchar szPath[MAX_PATH] = TEXT("");
	lstrcpy(szPath, pSaveDirectory);

	lstrcat(szPath, pFileName);
	lstrcat(szPath, TEXT(".dat"));

	HANDLE hFile = CreateFile(szPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	_ulong	dwStrByte = 0;

	// Write Nodes

	// Meshes NumMeshes
	WriteFile(hFile, &(m_Model.iNumMeshes), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_Model.iNumMeshes; ++i)
	{
		MESH Mesh = m_pMesh[i];

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
	}

	CloseHandle(hFile);

	return S_OK;
}


HRESULT CModel_Converter::Convert(_uint iType, const _char* pModelFilePath)
{
	CModel_Converter* pInstance = new CModel_Converter();

	if (FAILED(pInstance->Convert_Model(iType, pModelFilePath)))
	{
		MSG_BOX("Failed to Created CModelConverter");
		Safe_Release(pInstance);
		return E_FAIL;
	}
	else
	{
		Safe_Release(pInstance);
		return S_OK;
	}
}

void CModel_Converter::Free()
{
	// delete Nodes
	for (auto& iter : m_Nodes)
	{
		Safe_Delete_Array(iter.iChildrens);
	}
	m_Nodes.clear();

	// delete Meshes
	for (_uint i = 0; i < m_Model.iNumMeshes; ++i)
	{
		MESH Mesh = m_pMesh[i];

		for (_uint j = 0; j < Mesh.iNumFaces; ++j)
		{
			Safe_Delete_Array(Mesh.Faces[j].iIndices);
		}
		Safe_Delete_Array(Mesh.Faces);

		Safe_Delete_Array(Mesh.vPositions);
		Safe_Delete_Array(Mesh.vNormals);
		Safe_Delete_Array(Mesh.vTexCoords);
		Safe_Delete_Array(Mesh.vTangents);

		for (_uint j = 0; j < Mesh.iNumBones; ++j)
		{
			Safe_Delete_Array(Mesh.Bones[j].Weights);
		}
		Safe_Delete_Array(Mesh.Bones);
	}

	Safe_Delete_Array(m_pMesh);

	// delete Materials
	Safe_Delete_Array(m_pMaterial);

	// delete Animations
	for (_uint i = 0; i < m_Model.iNumAnimations; ++i)
	{
		ANIMATION Animation = m_pAnimation[i];

		for (_uint j = 0; j < Animation.iNumChannels; ++j)
		{
			Safe_Delete_Array(Animation.Channels[j].ScalingKeys);
			Safe_Delete_Array(Animation.Channels[j].RotationKeys);
			Safe_Delete_Array(Animation.Channels[j].PositionKeys);
		}

		Safe_Delete_Array(Animation.Channels);
	}
	Safe_Delete_Array(m_pAnimation);
}
