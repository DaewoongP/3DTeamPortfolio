#pragma once
#include "Base.h"
#include "Tool_Defines.h"
#include "Window_Manager.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

class CMainTool final : public CBase
{
private:
	explicit CMainTool();
	virtual ~CMainTool() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

public:
	HRESULT Initialize_ImGui();
	HRESULT Render_ImGui();

	// 게임 전체에서 사용하기 위한 스태틱 컴포넌트 생성 함수
	HRESULT Ready_Prototype_Component();
	HRESULT Ready_Prototype_Object();
	HRESULT Ready_Fonts();
	HRESULT Open_Level(LEVELID eLevelIndex);

#ifdef _DEBUG
private:
	void Tick_FPS(_float fTimeDelta);
#endif // _DEBUG

private:
	void Tick_ImGui();

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	_float4					m_vImGuiClearColor;
	ID3D11Texture2D*		m_pTexture2D = { nullptr };
	ID3D11RenderTargetView* m_pImGuiRTV = { nullptr };

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };
	CWindow_Manager*		m_pWindow_Manager = { nullptr };

#ifdef _DEBUG
private: /* For. Frame Per Second*/
	_tchar					m_szFPS[MAX_STR];
	_int					m_iFps = { 0 };
	_float					m_fFpsTime = { 0.f };
#endif // _DEBUG

public:
	static CMainTool* Create();
	virtual void Free() override;
};

