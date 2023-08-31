#pragma once
#include "Base.h"
#include "Model.h"

BEGIN(Engine)

class CMeshParts final : public CBase
{
private:
	explicit CMeshParts() = default;
	explicit CMeshParts(const CMeshParts& rhs);
	virtual ~CMeshParts() = default;

public:
	_uint Get_NumMeshes() {
		return m_iNumMeshes;
	}
	void Get_Matrices(const _uint& _iMeshIndex, CModel::BONES _Bones, 
		_Inout_ _float4x4* _pMatrices, _float4x4 _PivotMatrix);

public:
	HRESULT Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, 
		const wstring& _wstrMeshPartsFilePath, const CModel::BONES& _Bones);
	HRESULT Render(const _uint& _iMeshIndex);

public:
	HRESULT Bind_Material(CShader* _pShader, const char* _pConstantName, 
		const _uint& _iMeshIndex, Engine::TextureType _MaterialType);

private:
	wstring		m_wstrMeshPartsName = { TEXT("") };
	_bool		m_isCloned = { false };

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
	HRESULT Ready_Mesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const CModel::BONES& _Bones);
	HRESULT Ready_Material(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);

	void Release_FileDatas();

public:
	static CMeshParts* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, 
		const wstring& _wstrMeshPartsFilePath, const CModel::BONES& _Bones);
	virtual void Free() override;
};

END