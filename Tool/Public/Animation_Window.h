#pragma once
#include "ImWindow.h"
#include "Model.h"

BEGIN(Tool)

class CAnimation_Window final : public CImWindow
{
private:
	explicit CAnimation_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CAnimation_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	// 현재 수정중인 더미 오브젝트
	class CDummy* m_pDummyObject = { nullptr };
	class CModel* m_pDummyModel = { nullptr };
	class CCamera_Point* m_pCameraPoint = { nullptr };
	class CCamera_Point* m_pParticleNotify[CModel::ANIM_END] = { nullptr };

	_int			m_iMaxModelIndex = { 0 };
	_int		    m_iModelIndex = { 0 };
	vector<string>  m_vecModelList;
	vector<wstring> m_vecModelList_t;

	//애니메이션용도임.
	_char			m_szCurrentItem[CModel::ANIM_END][MAX_PATH] = {};
	_int			m_iRootIndex = { 0 };
	_int			m_iFromBone[CModel::ANIM_END] = { 0 };
	_int			m_iToBone[CModel::ANIM_END] = { 0 };
	_char			m_szAnimationTag[CModel::ANIM_END][MAX_PATH] = {};

	//노티파이
	_float			m_fNotifyActionTime = { 0 };
	_float			m_fNotifySpeed = { 0 };
	_char			m_szNotifyTag[MAX_PATH] = "";
	_char			m_szNotifyName[MAX_PATH] = "";
	_char			m_szCurrentItemType[MAX_PATH] = "";
	_int			m_iParitcleNotify_BindBoneIndex = {};
	_float3			m_vParitcleNotify_OffsetPosition = {};
	KEYFRAME::KEYFRAMETYPE m_eNotifyKeyFrameType = { KEYFRAME::KF_SPEED };

	//사운드 노티파이
	_bool			m_isFoce = { false };
	_int			m_iChannel = { 2 };
	_float			m_fVolum = { 0.7f };

	_char			m_szAnimationSearch[CModel::ANIM_END][MAX_PATH] = {};

	_int			m_iSelectedNotifyIndex[CModel::ANIM_END] = { 0 };

	_tchar			m_wszCurrentDummyModelTag[MAX_PATH] = {};

	//애니메잇녀 추가용
	_bool			m_isAnimationAdd = { false };
	_int			m_iAnimationPartNum = { 0 };
private:
	void Create_Dummy_Button();
	void OpenFile_Button();
	void AddModel_Button();
	void Select_Model();

	void Add_Animation_To_Model(CModel::ANIMTYPE ePartCnt, CModel* m_pDummyModel);

	void Export_Model();

	void OffsetVectorSetting(CModel* pDummyModel);
	void ParticlePositionVisualize();

	void Animation_ComboBox(CModel::ANIMTYPE ePartCnt, _char* szCurrentItem, CModel* pDummyModel);
	void Animation_ChildFrame(CModel::ANIMTYPE ePartCnt, _char* szCurrentItem, CModel* pDummyModel);
	void Animation_Table(CModel::ANIMTYPE ePartCnt, _char* szCurrentItem, CModel* pDummyModel);
	void Animation_Action_Button(CModel::ANIMTYPE ePartCnt, CModel* pDummyModel, _float* fNotifyActionTime);
	void Notify_InputFileds(_char* szNotifyName, KEYFRAME::KEYFRAMETYPE* eNotifyKeyFrameType, _float* fNotifyActionTime, _float* fNotifySpeed, _char* szNotifyActionTag);
	void Add_Notify_Button(CModel::ANIMTYPE ePartCnt, _char* szNotifyName, CModel* m_pDummyModel, KEYFRAME::KEYFRAMETYPE* eNotifyKeyFrameType, _float* fNotifyActionTime, _float* fNotifySpeed, _char* szActionTagName);
	void Edit_Notify_Button(CModel::ANIMTYPE ePartCnt, CModel* pDummyModel);

	void Create_Notify_ChildFrame(CModel::ANIMTYPE ePartCnt, CModel* pDummyModel);
	void Create_Notify_View(CModel::ANIMTYPE ePartCnt, CModel* pDummyModel);

	void Bone_Tree(CBone* bone, CModel* pDummyModel);
public:
	static CAnimation_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END