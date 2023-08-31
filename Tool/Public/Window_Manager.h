#pragma once
#include "Base.h"
#include "Tool_Defines.h"
#include "Object_Window.h"
#include "UI_Window.h"
#include "Effect_Window.h"
#include "Animation_Window.h"
#include "Camera_Window.h"
#include"Light_Window.h"

BEGIN(Tool)

class CWindow_Manager final : public CBase
{
private:
	enum MENULIST {
		OBJECT_WINDOW, MESH_WINDOW, NAVIGATION_WINDOW, CAMERA_WINDOW, EFFECT_WINDOW
		, ANIMATION_WINDOW, UI_WINDOW, LIGHT_WIDNOW, MENULIST_END
	};

	DECLARE_SINGLETON(CWindow_Manager);

private:
	explicit CWindow_Manager() = default;
	virtual ~CWindow_Manager() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();
	
public:
	HRESULT Add_Window(const _tchar* pWindowTag, class CImWindow* pWindow);
	class CImWindow* Find_Window(const _tchar* pWindowTag);

private:
	HRESULT Setup_Current_Window(const _tchar* pTag);

private:
	unordered_map<const _tchar*, class CImWindow*>      m_ImWindows;
	CImWindow*						m_pCurrrentWindow = { nullptr };

private:
	MENULIST						m_eCurMenuList = { MENULIST_END };

public:
	virtual void Free(void) override;
};

END