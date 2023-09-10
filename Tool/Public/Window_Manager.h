#pragma once
#include "Base.h"
#include "Tool_Defines.h"
#include "Object_Window.h"
#include "UI_Window.h"
#include "Effect_Window.h"
#include "Animation_Window.h"
#include "Camera_Window.h"
#include "Light_Window.h"
#include "Navigation_Window.h"
#include "Cloth_Window.h"

BEGIN(Tool)

class CWindow_Manager final : public CBase
{
private:
	enum MENULIST {
		OBJECT_WINDOW, NAVIGATION_WINDOW, CAMERA_WINDOW, EFFECT_WINDOW
		, ANIMATION_WINDOW, UI_WINDOW, LIGHT_WINDOW, CLOTH_WINDOW,MENULIST_END
	};
	// window 생성시 위 리스트와 아래 플래그값을 하나씪 추가해주시면 됩니다.
	enum LOADINGFLAG {
		OBJECT_LOAD			= 1 << 0,
		NAVIGATION_LOAD		= 1 << 1,
		CAMERA_LOAD			= 1 << 2,
		EFFECT_LOAD			= 1 << 3,
		ANIAMTION_LOAD		= 1 << 4,
		UI_LOAD				= 1 << 5,
		LIGHT_LOAD			= 1 << 6,
		CLOTH_LOAD			= 1 << 7,
	};

	DECLARE_SINGLETON(CWindow_Manager);

private:
	explicit CWindow_Manager() = default;
	virtual ~CWindow_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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
	_uint							m_eLoadingFlag = { 0 };

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

public:
	virtual void Free(void) override;
};

END