#pragma once
#include "Assimp_Defines.h"
#include "Base.h"

BEGIN(Assimp)

class CModel_Converter final : public CBase
{
public:
	// 모델 타입
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_MAPOBJECT, TYPE_ONLYANIM, TYPE_END };

private:
	explicit CModel_Converter();
	virtual ~CModel_Converter() = default;

public:
	// 클래스 최초 구동함수
	HRESULT Convert_Model(_uint iType, const _char* pModelFilePath);

private:
	// 어심프로 모델을 읽기 위해 할당한 어심프 객체.
	const aiScene* m_pAIScene = { nullptr };
	// m_pAIScene에 읽은 파일을 넘겨주기위해 할당한 변수.
	Assimp::Importer		m_Importer;

private: /* For. BoneNode */
	// 현재 뼈의 인덱스
	_uint					m_iCurrentNodeIndex = { 0 };
	// 생성한 뼈들을 담은 벡터컨테이너
	vector<NODE>			m_Nodes;

private: /* For. Converted Structs */
	// 변환하여 담아둘 구조체. 
	// 실제로 클라이언트, 툴에 값을 넘기기위한 값들을 정의
	MODEL					m_Model;
	// AIScene->mMeshes 의 값들을 담기위한 변수
	MESH* m_pMesh = { nullptr };
	// AIScene->mMaterials 의 값들을 담기위한 변수
	MATERIAL* m_pMaterial = { nullptr };
	// AIScene->mAnimations 의 값들을 담기위한 변수
	ANIMATION* m_pAnimation = { nullptr };
private:
	HRESULT Convert_Bones(aiNode* pNode, _uint iParentIndex, _Inout_ _uint* iChildIndex, _bool isRoot = false);
	// 뼈를 인덱스 순서대로 처리하기 위한 정렬 함수
	HRESULT Sort_Bones();
	HRESULT Convert_Meshes();
	HRESULT Store_Mesh(const aiMesh* pAIMesh, _Inout_ MESH* outMesh);
	HRESULT Convert_Materials(TYPE eType, const char* pModelFilePath, const _tchar* pSaveDirectory);
	HRESULT Convert_Animations();

private:
	// 현재까지 담은 구조체와 변수들을 파일로 써주는 함수
	HRESULT Write_File(TYPE eType, const _tchar* pSaveDirectory, const _tchar* pFileName);
	HRESULT Write_File_COL(TYPE eType, const _tchar* pSaveDirectory, const _tchar* pFileName);
	HRESULT Write_File_Anim(TYPE eType, const _tchar* pSaveDirectory, const _tchar* pFileName);

public:
	static HRESULT Convert(_uint iType, const _char* pModelFilePath);
	virtual void Free() override;
};

END