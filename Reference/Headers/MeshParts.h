#pragma once

/* =============================================== */
//	[CMeshParts]
//	- CCustomModel�� ���� �� MeshParts Ŭ����
//  - MeshParts�� Mesh�� Material ������ ������ �ִ�.
//	- CCustomModel�� Bones�� �̿��ؼ� Mesh�� BoneIndices�� ����
//  - MeshParts�� �±״� "PartsIndex(HEAD, ARM, UPPERBODY...)" + "_ Name" ���� �����Ѵ�.
// 
//	�� : �ּ�ȯ
//	�� : ��ö��
//
/* =============================================== */

#include "Component.h"
#include "CustomModel.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CMeshParts final : public CComponent
{
public:
	typedef struct tagMeshPartsDesc
	{
		const CModel::BONES* m_pBones = { nullptr };
	}MESHPARTSDESC;

private:
	explicit CMeshParts(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CMeshParts(const CMeshParts& _rhs);
	virtual ~CMeshParts() = default;

public:
	CCustomModel::MESHTYPE Get_MeshType() const {
		return m_eMeshType;
	}
	_uint Get_NumMeshes() {
		return m_iNumMeshes;
	}
	void Get_Matrices(const _uint& _iMeshIndex, CModel::BONES _Bones, 
		_Inout_ _float4x4* _pMatrices, _float4x4 _PivotMatrix);
	void Set_MeshType(CCustomModel::MESHTYPE _eType) {
		m_eMeshType = _eType;
	}

public:
	virtual HRESULT Initialize_Prototype(const wstring& _wstrMeshPartsFilePath, const wstring& _wstrMeshPartsTag);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(const _uint& _iMeshIndex);

public:
	HRESULT Bind_Material(class CShader* _pShader, const char* _pConstantName, 
		const _uint& _iMeshIndex, Engine::TextureType _MaterialType);

private:
	CCustomModel::MESHTYPE m_eMeshType = { CCustomModel::MESH_END };
	wstring		m_wstrMeshPartsTag = { TEXT("") };

private: // Files
	MODEL				m_Model;
	vector<NODE>		m_NodeDatas;
	vector<MESH>		m_MeshDatas;
	vector<MATERIAL>	m_MaterialDatas;
	vector<ANIMATION>	m_AnimationDatas;

private: /* For.Meshes */
	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

private: /* For.Materials */
	_uint					m_iNumMaterials = { 0 };
	vector<MESHMATERIAL>	m_Materials;

private:
	HRESULT Ready_File(const _tchar* _pMeshPartsFilePath);
	HRESULT Ready_Mesh(const CModel::BONES& _Bones);
	HRESULT Ready_Material();

	void Release_FileDatas();

public:
	static CMeshParts* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, 
		const wstring& _wstrMeshPartsFilePath, const wstring& _wstrMeshPartsTag);
	virtual CMeshParts* Clone(void* _pArg) override;
	virtual void Free() override;
};

END