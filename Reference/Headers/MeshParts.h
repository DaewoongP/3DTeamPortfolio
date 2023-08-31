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
	void Get_Matrices(const _uint& _iMeshIndex, CModel::BONES _Bones, _Inout_ _float4x4* _pMatrices, _float4x4 _PivotMatrix);
	CModel::BONES Get_Bones() const {
		return m_Bones;
	}
	_uint Get_CurrentAnimIndex() const { 
		return m_iCurrentAnimIndex; 
	}
	_uint Get_NumAnimations() const { 
		return m_iNumAnimations;
	}
	class CAnimation* Get_Animation() const {
		return m_Animations[m_iCurrentAnimIndex]; 
	}
	class CAnimation* Get_Animation(const _uint& _iAnimationIndex) const {
		return m_Animations[_iAnimationIndex]; 
	}

public:
	HRESULT Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _wstrMeshPartsFilePath);
	HRESULT Render(const _uint& _iMeshIndex);

public:
	// Lerp를 진행하지 않고 해당 애니메이션 초기상태로 바로 세팅한다.
	void	Reset_Animation(_uint iAnimIndex);
	void	Play_Animation(_float fTimeDelta);

public:
	HRESULT Bind_Material(CShader* _pShader, const char* _pConstantName, const _uint& _iMeshIndex, Engine::TextureType _MaterialType);

private:
	wstring		m_wstrMeshPartsName = { TEXT("") };
	_bool		m_isCloned = { false };

private: // Files
	MODEL				m_Model;
	vector<NODE>		m_NodeDatas;
	vector<MESH>		m_MeshDatas;
	vector<MATERIAL>	m_MaterialDatas;
	vector<ANIMATION>	m_AnimationDatas;

private: /* For.Bones */
	CModel::BONES			m_Bones;

private: /* For.Meshes */
	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

private: /* For.Materials */
	_uint					m_iNumMaterials = { 0 };
	vector<MESHMATERIAL>	m_Materials;

private: /* For.Animations */
	_uint							m_iPreviousAnimIndex = { 0 };
	_uint							m_iCurrentAnimIndex = { 0 };
	_uint							m_iNumAnimations = { 0 };

	vector<class CAnimation*>		m_Animations;

private:
	HRESULT Ready_File(const _tchar* pMeshPartsFilePath);
	HRESULT Ready_Bones(Engine::NODE Node);
	HRESULT Ready_Mesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	HRESULT Ready_Material(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	HRESULT Ready_Animations();

	void Release_FileDatas();

public:
	static CMeshParts* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _wstrMeshPartsFilePath);
	virtual void Free() override;
};

END