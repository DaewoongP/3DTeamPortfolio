#pragma once

/* =============================================== */
// 
//	정 : 안철민
//	부 : 주성환
//
/* =============================================== */

#include "Component.h"

BEGIN(Engine)
class CTransform;
#define ANIMATIONLERPTIME 0.3f
class ENGINE_DLL CModel : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
	enum ANIMTYPE { UPPERBODY, UNDERBODY, ANIM_END };

protected:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	const _float4x4* Get_PivotFloat4x4Ptr() const { return &m_PivotMatrix; }
	_float4x4					Get_PivotFloat4x4() const { return m_PivotMatrix; }
	_uint						Get_CurrentAnimIndex(ANIMTYPE eType = UPPERBODY) const { return m_tAnimationDesc[eType].iCurrentAnimIndex; }
	_uint						Get_NumMeshes() const { return m_iNumMeshes; }
	_uint						Get_NumAnimations(ANIMTYPE eType = UPPERBODY) const { return m_tAnimationDesc[eType].iNumAnimations; }
	const class CBone* Get_Bone(const _tchar* pBoneName);
	class CBone* Get_Bone_Index(_uint iIndex);
	class CAnimation* Get_Animation(ANIMTYPE eType = UPPERBODY) const { return m_tAnimationDesc[eType].Animations[m_tAnimationDesc[eType].iCurrentAnimIndex]; }
	class CAnimation* Get_Animation(_uint iAnimationIndex, ANIMTYPE eType = UPPERBODY) const { return  m_tAnimationDesc[eType].Animations[iAnimationIndex]; }
	class CAnimation* Get_Animation(const wstring& wstrAnimationTag, ANIMTYPE eType = UPPERBODY) const;
	const _float4x4* Get_BoneCombinedTransformationMatrixPtr(_uint iIndex);
	_float4x4					Get_BoneCombinedTransformationMatrix(_uint iIndex);
	vector<class CBone*>* Get_Bone_Vector_Point() { return &m_Bones; }
	_uint						Get_AnimationPartCount() { return m_iAnimationPartCount; }
	_uint						Get_RootBoneIndex() { return m_iRootBoneIndex; }
	_bool						Is_Finish_Animation(ANIMTYPE eType = UPPERBODY) { return  m_tAnimationDesc[eType].isFinishAnimation; }
	class CMesh* Get_Meshes_Test() { return(m_Meshes[0]); }
	vector<class CMesh*>* Get_MeshesVec() { return &m_Meshes; }

	// 뼈오프셋 * 뼈 컴바인 * 모델피벗에 대한 메트릭스(내부적으로 0,1,2행 노말라이즈)를 반환
	_float4x4 Get_Attaching_Bone_Matrix(_uint iBoneIndex);
	
public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _float4x4 PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void	Change_Animation(const wstring& wstrAnimationTag, ANIMTYPE eType = UPPERBODY);
	void	Change_Animation(_uint iAnimIndex, ANIMTYPE eType = UPPERBODY);
	//루트애니메이션을 사용할경우 ctransform을 넣어준다.
	void	Play_Animation(_float fTimeDelta, ANIMTYPE eType = UPPERBODY, CTransform* pTransform = nullptr);
	HRESULT Find_BoneIndex(const _tchar* pBoneName, _Inout_ _uint* iIndex);
	void	Set_CurrentAnimIndex(_uint iIndex, ANIMTYPE eType = UPPERBODY);
	void	Set_RootBone(_uint iIndex) { m_iRootBoneIndex = iIndex; }
	void	Do_Root_Animation(_float fTimeDelta, CTransform* pTransform = nullptr, _bool isFirstFrame = false);
	HRESULT Separate_Animation(_int iFromIndex, _int iToIndex, ANIMTYPE eType);
	void	Delete_Animation(_uint iAnimIndex, ANIMTYPE eType = UPPERBODY);
	HRESULT Bind_Notify(const _uint& iAnimIndex, const wstring& wstrNotifyTag, function<void()>& Func, ANIMTYPE eType = UPPERBODY);
	HRESULT Bind_Notify(const wstring& wstrAnimIndex, const wstring& wstrNotifyTag, function<void()>& Func, ANIMTYPE eType = UPPERBODY);
	HRESULT Bind_Notifies(const wstring& wstrNotifyTag, function<void()>& Func, ANIMTYPE eType = UPPERBODY);

public:
	virtual HRESULT Bind_Material(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, Engine::TextureType MaterialType);
	virtual HRESULT Bind_BoneMatrices(class CShader* pShader, const char* pConstantName, _uint iMeshIndex);

protected: // Files
	wstring							m_wstrModelPath;
	MODEL							m_Model;
	vector<NODE>					m_NodeDatas;
	vector<MESH>					m_MeshDatas;
	vector<MATERIAL>				m_MaterialDatas;
	vector<ANIMATION>				m_AnimationDatas;

	/* For GCM */
	MODEL_GCM						m_ModelGCM;
	vector<ANIMATION_GCM>			m_AnimationDatasGCM[ANIM_END];

protected: /* For.Bones */
	vector<class CBone*>			m_Bones;
public:
	typedef vector<class CBone*>	BONES;

protected: /* For.Meshes */
	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh*>			m_Meshes;

protected: /* For.Materials */
	_uint							m_iNumMaterials = { 0 };
	vector<MESHMATERIAL>			m_Materials;

protected: /* For.Animations */
	_uint							m_iAnimationPartCount = { 0 };
	ANIMATIONDESC					m_tAnimationDesc[ANIM_END];

protected: /* For.RootAnimation*/
	_uint							m_iRootBoneIndex = { 0 };
	_float4x4						m_PostRootMatrix = {};
	_float4x4						m_BeginRootAnimRootMatrix = {};

protected:
	_float4x4						m_PivotMatrix;

protected:
	_bool							m_isExportedTool = { false };
	_bool							m_isCreatedByGCM = { false };
	_bool							m_isFirstFrame = { false };

protected:
	HRESULT Ready_File(TYPE eType, const _tchar* pModelFilePath);
	HRESULT Ready_Bones(Engine::NODE Node);
	HRESULT Ready_Meshes(TYPE eType, _float4x4 PivotMatrix);
	HRESULT Ready_Materials();
	HRESULT Ready_Animations();

public:
	HRESULT Convert_Animations_GCM();
	HRESULT Write_File_GCM(TYPE eType, const _tchar* pModelFilePath);

	HRESULT Ready_File_GCM(TYPE eType, const _tchar* pModelFilePath);
	HRESULT Ready_Bones_GCM(Engine::NODE Node);
	HRESULT Ready_Meshes_GCM(TYPE eType, _float4x4 PivotMatrix);
	HRESULT Ready_Materials_GCM();
	HRESULT Ready_Animations_GCM();

	HRESULT Ready_File_Animation(ANIMTYPE eType, const _tchar* pAnimationFilePath);
	HRESULT Add_Animations(ANIMTYPE eType, vector<ANIMATION>* AnimVec);
	HRESULT Release_TempAnimVec(vector<ANIMATION>* AnimVec);

protected:
	void Release_FileDatas();
	void Release_FileDatas_GCM();
	_uint Find_Animation_Index(const wstring& strTag, ANIMTYPE eType = UPPERBODY) const;
	void Reset_Animation(ANIMTYPE eType);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, _float4x4 PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END