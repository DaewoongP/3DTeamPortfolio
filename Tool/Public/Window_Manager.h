#pragma once
#include "Base.h"
#include "Tool_Defines.h"

#include "Object_Window.h"

BEGIN(Tool)

class CWindow_Manager final : public CBase
{
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
	unordered_map<const _tchar*, class CImWindow*>      m_ImWindows;

public:
	virtual void Free(void) override;
};

END