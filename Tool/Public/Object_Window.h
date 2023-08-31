#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CMapDummy;

class CObject_Window final : public CImWindow
{
private:
	explicit CObject_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CObject_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void PickingMenu(); // 피킹 메뉴
	void SelectModel(); // 모델 선택 메뉴
	void Deep_Copy_Name(); // 모델 이름 문자열 깊은 복사
	void Deep_Copy_Path(const _tchar* wszPath); // 모델 경로 문자열 깊은 복사

	_float3 Find_PickingPos(); // 지형의 피킹 위치를 찾는 함수
	HRESULT Create_Dummy(); // 최초 1번 Dummy 생성

private:
	CMapDummy* m_pDummy = { nullptr }; // 생성해둔 Dummy의 주소

	string m_strCurrentModel = { "Prototype_Component_Model_Tree" }; // 현재 활성화된 모델 이름, 초기값은 더미

	_int m_iModelIndex = { 0 }; // 선택된 모델 인덱스
	vector<string> m_vecModelList; // 현재 추가해둔 모델 이름 리스트
	vector<const _tchar*> m_vecModelList_t; // 모델 이름 리스트를 _tchar로 저장해둠
	vector<const _tchar*> m_vecModelPath_t; // 그 모델의 경로를 _tchar로 저장해둠

public:
	static CObject_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END