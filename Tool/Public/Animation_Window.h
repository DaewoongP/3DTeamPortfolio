#pragma once
#include "ImWindow.h"

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
	_int			m_iSelectedNotifyIndex = { 0 };
	vector<string>  m_vecModelList;
	vector<wstring> m_vecModelList_t;

private:
	void Create_Dummy_Button();
	void OpenFile_Button();
	void AddModel_Button();
	void Animation_ComboBox(_char* szCurrentItem,CModel* pDummyModel);
	void Animation_Action_Button(CModel* pDummyModel, _float* fNotifyActionTime);
	void Notify_InputFileds(_char*  szNotifyName, KEYFRAME::KEYFRAMETYPE* eNotifyKeyFrameType, _float* fNotifyActionTime, _float* fNotifySpeed);
	void Add_Notify_Button(_char* szNotifyName, CModel* pDummyModel, KEYFRAME::KEYFRAMETYPE* eNotifyKeyFrameType, _float* fNotifyActionTime, _float* fNotifySpeed);
	void Edit_Notify_Button();
	void Select_Model();

public:
	static CAnimation_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END