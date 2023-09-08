#pragma once

// 정 : 심정환
// static mesh 인스턴싱을 위한 Model 클래스

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel_Instance : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel_Instance(const CModel_Instance& rhs);
	virtual ~CModel_Instance() = default;

public:
	_uint						Get_NumMeshes() const { return m_iNumMeshes; }

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _float4x4* pInstanceMatrix, _uint iInstanceCnt, _float4x4 PivotMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	HRESULT Bind_Material(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, TextureType MaterialType);

private: // Files
	MODEL							m_Model;
	vector<NODE>					m_NodeDatas;
	vector<MESH>					m_MeshDatas;
	vector<MATERIAL>				m_MaterialDatas;
	vector<ANIMATION>				m_AnimationDatas;

private: /* For.Meshes */
	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh_Instance*>	m_Meshes;

private: /* For.Materials */
	_uint							m_iNumMaterials = { 0 };
	vector<MESHMATERIAL>			m_Materials;

private:
	_float4x4						m_PivotMatrix;

private:
	HRESULT Ready_File(TYPE eType, const _tchar* pModelFilePath);
	HRESULT Ready_Meshes(TYPE eType, _float4x4* pInstanceMatrix, _uint iInstanceCnt, _float4x4 PivotMatrix);
	HRESULT Ready_Materials();

private:
	void Release_FileDatas();

public:
	static CModel_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, 
		_float4x4* pInstanceMatrix, _uint iInstanceCnt, _float4x4 PivotMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END