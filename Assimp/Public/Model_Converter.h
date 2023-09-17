#pragma once
#include "Assimp_Defines.h"
#include "Base.h"

BEGIN(Assimp)

class CModel_Converter final : public CBase
{
public:
	// �� Ÿ��
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_MAPOBJECT, TYPE_ONLYANIM, TYPE_END };

private:
	explicit CModel_Converter();
	virtual ~CModel_Converter() = default;

public:
	// Ŭ���� ���� �����Լ�
	HRESULT Convert_Model(_uint iType, const _char* pModelFilePath);

private:
	// ������� ���� �б� ���� �Ҵ��� ����� ��ü.
	const aiScene* m_pAIScene = { nullptr };
	// m_pAIScene�� ���� ������ �Ѱ��ֱ����� �Ҵ��� ����.
	Assimp::Importer		m_Importer;

private: /* For. BoneNode */
	// ���� ���� �ε���
	_uint					m_iCurrentNodeIndex = { 0 };
	// ������ ������ ���� ���������̳�
	vector<NODE>			m_Nodes;

private: /* For. Converted Structs */
	// ��ȯ�Ͽ� ��Ƶ� ����ü. 
	// ������ Ŭ���̾�Ʈ, ���� ���� �ѱ������ ������ ����
	MODEL					m_Model;
	// AIScene->mMeshes �� ������ ������� ����
	MESH* m_pMesh = { nullptr };
	// AIScene->mMaterials �� ������ ������� ����
	MATERIAL* m_pMaterial = { nullptr };
	// AIScene->mAnimations �� ������ ������� ����
	ANIMATION* m_pAnimation = { nullptr };
private:
	HRESULT Convert_Bones(aiNode* pNode, _uint iParentIndex, _Inout_ _uint* iChildIndex, _bool isRoot = false);
	// ���� �ε��� ������� ó���ϱ� ���� ���� �Լ�
	HRESULT Sort_Bones();
	HRESULT Convert_Meshes();
	HRESULT Store_Mesh(const aiMesh* pAIMesh, _Inout_ MESH* outMesh);
	HRESULT Convert_Materials(TYPE eType, const char* pModelFilePath, const _tchar* pSaveDirectory);
	HRESULT Convert_Animations();

private:
	// ������� ���� ����ü�� �������� ���Ϸ� ���ִ� �Լ�
	HRESULT Write_File(TYPE eType, const _tchar* pSaveDirectory, const _tchar* pFileName);
	HRESULT Write_File_COL(TYPE eType, const _tchar* pSaveDirectory, const _tchar* pFileName);
	HRESULT Write_File_Anim(TYPE eType, const _tchar* pSaveDirectory, const _tchar* pFileName);

public:
	static HRESULT Convert(_uint iType, const _char* pModelFilePath);
	virtual void Free() override;
};

END