#pragma once

// 정 : 심정환
// 부 : 주성환
// static mesh 인스턴싱을 위한 Model 클래스

#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CModel_Instance : public CComponent
{
private:
	CModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel_Instance(const CModel_Instance& rhs);
	virtual ~CModel_Instance() = default;

public:
	_uint							Get_NumMeshes() const { return m_iNumMeshes; }
	vector<class CMesh_Instance*>	Get_Meshes() { return m_Meshes; }
	_bool							Is_Finish_Animation() { return  m_tAnimationDesc.isFinishAnimation; }

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eType, const _tchar* pModelFilePath, _float4x4* pInstanceMatrix, _uint iInstanceCnt, _float4x4 PivotMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void	Play_Animation(_float fTimeDelta);
	HRESULT Bind_Material(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, TextureType MaterialType);
	virtual HRESULT Bind_BoneMatrices(class CShader* pShader, const char* pConstantName, _uint iMeshIndex);
	void	Change_Animation(_uint iAnimIndex);

private: // Files
	MODEL							m_Model;
	vector<NODE>					m_NodeDatas;
	vector<MESH>					m_MeshDatas;
	vector<MATERIAL>				m_MaterialDatas;
	vector<ANIMATION>				m_AnimationDatas;

protected: /* For.Bones */
	vector<class CBone*>			m_Bones;

private: /* For.Meshes */
	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh_Instance*>	m_Meshes;

private: /* For.Materials */
	_uint							m_iNumMaterials = { 0 };
	vector<MESHMATERIAL>			m_Materials;

protected: /* For.Animations */
	ANIMATIONDESC					m_tAnimationDesc;

protected: /* For.RootAnimation*/
	_uint							m_iRootBoneIndex = { 0 };
	_float4x4						m_PostRootMatrix = {};
	_float4x4						m_BeginRootAnimRootMatrix = {};

protected:
	_bool							m_isExportedTool = { false };
	_bool							m_isFirstFrame = { false };

private:
	_float4x4						m_PivotMatrix;

private:
	HRESULT Ready_File(CModel::TYPE eType, const _tchar* pModelFilePath);
	HRESULT Ready_Bones(Engine::NODE Node);
	HRESULT Ready_Meshes(CModel::TYPE eType, _float4x4* pInstanceMatrix, _uint iInstanceCnt, _float4x4 PivotMatrix);
	HRESULT Ready_Materials();
	HRESULT Ready_Animations();

private:
	void Release_FileDatas();

public:
	static CModel_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const _tchar* pModelFilePath,
		_float4x4* pInstanceMatrix, _uint iInstanceCnt, _float4x4 PivotMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END