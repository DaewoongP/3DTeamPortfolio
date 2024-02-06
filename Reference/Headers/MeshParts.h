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
		const CModel::BONES*	pBones = { nullptr };
		// �� ó���� ������ ���� ���
		const _tchar*			szClothDataFilePath = { nullptr };
		_float4					vColor;
	}MESHPARTSDESC;

#pragma region �ڵ� �߷�

private:
	explicit CMeshParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMeshParts(const CMeshParts& rhs);
	virtual ~CMeshParts() = default;

public:
	CCustomModel::MESHTYPE Get_MeshType() const {
		return m_eMeshType;
	}
	_uint Get_NumMeshes() {
		return m_iNumMeshes;
	}
	const vector<class CMesh*> Get_Meshes() const { return m_Meshes; }
	void Get_Matrices(const _uint& _iMeshIndex, CModel::BONES _Bones,
		_Inout_ _float4x4* _pMatrices, _float4x4 _PivotMatrix);
	const _float4& Get_Parts_Color() const {
		return m_vPartsColor;
	}
	void Set_MeshType(CCustomModel::MESHTYPE _eType) {
		m_eMeshType = _eType;
	}
	void Set_WindVelocity(_float3 vWindVelocity);
	void Set_Parts_Color(const _float4& vColor) {
		m_vPartsColor = vColor;
	}

	// ���� �޽� Tick ȣ��
	void Tick(const _uint& _iMeshIndex, _float _fTimeDelta);
	// Dynamic Mesh Index
	vector<_uint>				m_DynamicMeshIndices;
	HRESULT Ready_DynamicMesh(const CModel::BONES& _Bones, const _tchar* szClothDataFilePath);

private:
	MODEL						m_Model;
	vector<NODE>				m_NodeDatas;
	vector<ANIMATION>			m_AnimationDatas;
#pragma endregion // getter, setter ����

public:
	virtual HRESULT Initialize_Prototype(const wstring& wstrMeshPartsFilePath, 
		const wstring& wstrMeshPartsTag);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(const _uint& iMeshIndex);

public:
	HRESULT Bind_Material(class CShader* pShader, const char* pConstantName, 
		const _uint& iMeshIndex, Engine::TextureType MaterialType);

private:
	CCustomModel::MESHTYPE		m_eMeshType = { CCustomModel::MESH_END };
	wstring						m_wstrMeshPartsTag = { TEXT("") };
	_float4						m_vPartsColor;

private: // Files
	vector<MESH>				m_MeshDatas;
	vector<MATERIAL>			m_MaterialDatas;

private: /* For.Meshes */
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

private: /* For.Materials */
	_uint						m_iNumMaterials = { 0 };
	vector<MESHMATERIAL>		m_Materials;

private:
	HRESULT Ready_File(const _tchar* pMeshPartsFilePath);
	HRESULT Ready_Mesh(const CModel::BONES& Bones);
	HRESULT Ready_Material();

	void Release_FileDatas();

private: /* cloth */
	HANDLE Read_ClothIndexData(const _tchar* szClothDataFilePath, _uint iMeshIndex);

public:
	static CMeshParts* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		const wstring& wstrMeshPartsFilePath, const wstring& wstrMeshPartsTag);
	virtual CMeshParts* Clone(void* pArg) override;
	virtual void Free() override;
};

END