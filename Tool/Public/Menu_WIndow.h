#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CMenu_WIndow final : public CImWindow
{
private:
	enum MENULIST { OBJECT_WINDOW, MESH_WINDOW, NAVIMESH_WINDOW, CAMERA_WINDOW, EFFECT_WINDOW
	, ANIMATION_WINDOW, UI_WINDOW, LIGHT_WIDNOW, MENULIST_END };

private:
	explicit CMenu_WIndow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMenu_WIndow() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Setup_Current_Window(const _tchar* pTag);
	
private:
	MENULIST m_eCurMenuList = { MENULIST_END };

public:
	static CMenu_WIndow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END