#pragma once

/* =============================================== */
//	[CCustomModel]
//	- 커스터마이징 전용 모델
//  - 커스터마이징 모델은 뼈와 애니메이션 정보만 있으면 됨
// 
//	정 : 주성환
//	부 : 안철민
//
/* =============================================== */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCustomModel : public CComponent
{
public:
	enum MESHTYPE { HEAD, ARM, ROBE, UPPERBODY, UNDERBODY, SOCKS, SHOES, MESH_END };

private:
	explicit CCustomModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCustomModel(const CCustomModel& rhs);
	virtual ~CCustomModel() = default;

public:
	const _float4x4* Get_PivotFloat4x4Ptr() const {
		return &m_PivotMatrix;
	}
	_float4x4 Get_PivotFloat4x4() const {
		return m_PivotMatrix;
	}
	_uint Get_CurrentAnimIndex() const {
		return m_iCurrentAnimIndex;
	}
	// 모델의 메쉬 개수 반환
	_uint Get_NumMeshes(const _uint& _iMeshPartsIndex) const;
	
	// 모델의 애니메이션 개수 반환
	_uint Get_NumAnimations() const {
		return m_iNumAnimations;
	}
	const class CBone* Get_Bone(const _tchar* pBoneName);
	// 현재 실행중인 애니메이션
	class CAnimation* Get_Animation() const {
		return m_Animations[m_iCurrentAnimIndex];
	}
	class CAnimation* Get_Animation(const _uint& _iAnimationIndex) const {
		return m_Animations[_iAnimationIndex];
	}
	const _float4x4* Get_BoneCombinedTransformationMatrixPtr(_uint iIndex);
	_float4x4 Get_BoneCombinedTransformationMatrix(_uint iIndex);
	HRESULT Add_MeshParts(const _uint& _iLevelIndex, const wstring& _wstrPrototypeTag, MESHTYPE _eMeshPartsType);

public:
	virtual HRESULT Initialize_Prototype(const wstring& _wstrModelFilePath, _float4x4 _PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex);

public:
	// Lerp를 진행하지 않고 해당 애니메이션 초기상태로 바로 세팅한다.
	void	Reset_Animation(const _uint& _iAnimIndex);
	void	Play_Animation(const _float& _fTimeDelta);
	HRESULT Find_BoneIndex(const _tchar* _pBoneName, _uint* _iIndex);

public:
	HRESULT Bind_Material(class CShader* _pShader, const char* _pConstantName,
		const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex, Engine::TextureType _MaterialType);
	HRESULT Bind_BoneMatrices(class CShader* _pShader, const char* _pConstantName, const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex);

private: // Files
	MODEL				m_Model;
	vector<NODE>		m_NodeDatas;
	vector<MESH>		m_MeshDatas;
	vector<MATERIAL>	m_MaterialDatas;
	vector<ANIMATION>	m_AnimationDatas;

private: /* For.MeshParts */
	array<class CMeshParts*, MESH_END>	m_MeshParts = { nullptr };

private: /* For.Bones */
	CModel::BONES		m_Bones;

private: /* For.Animations */
	_uint							m_iPreviousAnimIndex = { 0 };
	_uint							m_iCurrentAnimIndex = { 0 };
	_uint							m_iNumAnimations = { 0 };

	vector<class CAnimation*>		m_Animations;

private:
	_float4x4 m_PivotMatrix;

private:
	HRESULT Ready_File(const wstring& _wstrModelFilePath);
	HRESULT Ready_Bones(Engine::NODE _Node);
	HRESULT Ready_Animations();

	void Release_FileDatas();

public:
	static CCustomModel* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _wstrModelFilePath, _float4x4 _PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END