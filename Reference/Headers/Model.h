#pragma once

/* =============================================== */
// 
//	정 : 안철민
//	부 :
//
/* =============================================== */

#include "Component.h"

BEGIN(Engine)
#define ANIMATIONLERPTIME 0.3f
class ENGINE_DLL CModel : public CComponent
{

public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
	enum ANIMTYPE { HEAD,UPPERBODY,UNDERBODY,ARM,ANIM_END};

private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	const _float4x4* Get_PivotFloat4x4Ptr() const { return &m_PivotMatrix; }
	_float4x4 Get_PivotFloat4x4() const { return m_PivotMatrix; }
	_uint Get_CurrentAnimIndex() const { return m_iCurrentAnimIndex; }
	// 모델의 메쉬 개수 반환
	_uint Get_NumMeshes() const { return m_iNumMeshes; }
	// 모델의 애니메이션 개수 반환
	_uint Get_NumAnimations() const { return m_iNumAnimations; }
	const class CBone* Get_Bone(const _tchar* pBoneName);
	// 현재 실행중인 애니메이션
	class CAnimation* Get_Animation() const { return m_Animations[m_iCurrentAnimIndex]; }
	class CAnimation* Get_Animation(_uint iAnimationIndex) const { return m_Animations[iAnimationIndex]; }
	const _float4x4* Get_BoneCombinedTransformationMatrixPtr(_uint iIndex);
	_float4x4 Get_BoneCombinedTransformationMatrix(_uint iIndex);

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _float4x4 PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	// Lerp를 진행하지 않고 해당 애니메이션 초기상태로 바로 세팅한다.
	void	Reset_Animation(_uint iAnimIndex, CTransform* pTransform = nullptr);
	void	Play_Animation(_float fTimeDelta,class CTransform* pTransform = nullptr);
	HRESULT Find_BoneIndex(const _tchar* pBoneName, _Inout_ _uint* iIndex);
	void	Set_CurrentAnimIndex(_uint iIndex);
	void	Set_RootBone(_uint iIndex) { m_iRootBoneIndex = iIndex; }
	void	Do_Root_Animation(CTransform* pTransform);

public:
	HRESULT Bind_Material(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, Engine::TextureType MaterialType);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const char* pConstantName, _uint iMeshIndex);

private: // Files
	MODEL							m_Model;
	vector<NODE>					m_NodeDatas;
	vector<MESH>					m_MeshDatas;
	vector<MATERIAL>				m_MaterialDatas;
	vector<ANIMATION>				m_AnimationDatas;

private: /* For.Bones */
	vector<class CBone*>			m_Bones;
public:
	typedef vector<class CBone*>	BONES;

private: /* For.Meshes */
	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh*>			m_Meshes;

private: /* For.Materials */
	_uint							m_iNumMaterials = { 0 };
	vector<MESHMATERIAL>			m_Materials;

private: /* For.Animations */
	_uint							m_iPreviousAnimIndex = { 0 };
	_uint							m_iCurrentAnimIndex = { 10 };
	_uint							m_iNumAnimations = { 0 };
	vector<class CAnimation*>		m_Animations;//[ANIM_END]
	_bool							m_isResetAnimTrigger = { false };

private: /* For.Animation_Lerp*/
	_bool							m_isAnimChangeLerp = { false };
	_float							m_fAnimChangeTimer = { 0.f };

private: /* For.RootAnimation*/
	_uint							m_iRootBoneIndex = { 5 };
	_float4x4						m_PostRootMatrix;

private:
	_float4x4						m_PivotMatrix;

private:
	HRESULT Ready_File(TYPE eType, const _tchar* pModelFilePath);
	HRESULT Ready_Bones(Engine::NODE Node);
	HRESULT Ready_Meshes(TYPE eType, _float4x4 PivotMatrix);
	HRESULT Ready_Materials();
	HRESULT Ready_Animations();

private:
	void Release_FileDatas();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, _float4x4 PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END