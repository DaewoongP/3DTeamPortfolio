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
	_char			m_szNotifyName[MAX_PATH] = "";
	_char			m_szCurrentItemType[MAX_PATH] = "";			
	KEYFRAME::KEYFRAMETYPE m_eNotifyKeyFrameType = { KEYFRAME::KF_SPEED };

	_int			m_iSelectedNotifyIndex[CModel::ANIM_END] = { 0 };

	_tchar			m_wszCurrentDummyModelTag[MAX_PATH] = {};
private:
	void Create_Dummy_Button();
	void OpenFile_Button();
	void AddModel_Button();
	void Select_Model();

	void Export_Model();

	void Animation_ComboBox(CModel::ANIMTYPE ePartCnt,_char* szCurrentItem, CModel* pDummyModel);
	void Animation_ChildFrame(CModel::ANIMTYPE ePartCnt, _char* szCurrentItem, CModel* pDummyModel);
	void Animation_Action_Button(CModel::ANIMTYPE ePartCnt, CModel* pDummyModel, _float* fNotifyActionTime);
	void Notify_InputFileds( _char* szNotifyName, KEYFRAME::KEYFRAMETYPE* eNotifyKeyFrameType, _float* fNotifyActionTime, _float* fNotifySpeed);
	void Add_Notify_Button(CModel::ANIMTYPE ePartCnt, _char* szNotifyName, CModel* pDummyModel, KEYFRAME::KEYFRAMETYPE* eNotifyKeyFrameType, _float* fNotifyActionTime, _float* fNotifySpeed);
	void Edit_Notify_Button(CModel::ANIMTYPE ePartCnt, CModel* pDummyModel);
	
	void Create_Notify_View(CModel::ANIMTYPE ePartCnt, CModel* pDummyModel);

	void Bone_Tree( CBone* bone, CModel* pDummyModel);
public:
	static CAnimation_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END