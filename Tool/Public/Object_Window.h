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

	_float3 Find_PickingPos(); // 지형의 피킹 위치를 찾는 함수
	HRESULT Create_Dummy(); // 최초 1번 Dummy 생성

private:
	CMapDummy* m_pDummy = { nullptr }; // 생성해둔 Dummy의 주소

	_tchar wszCurrentModel[MAX_PATH] = {}; // 현재 활성화된 모델 이름

public:
	static CObject_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END