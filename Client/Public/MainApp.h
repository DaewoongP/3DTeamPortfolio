#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)
#ifdef _DEBUG
class CImGui_Manager;
#endif // _DEBUG

class CMainApp final : public CBase
{
private:
	explicit CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

public:
	// 게임 전체에서 사용하기 위한 스태틱 컴포넌트 생성 함수
	HRESULT Ready_Prototype_Component_For_Static();
	HRESULT Ready_Fonts();
	HRESULT Open_Level(LEVELID eLevelIndex);

private:
	void Tick_FPS(_float fTimeDelta);

#ifdef _DEBUG
private: /* ImGui */
	void Debug_ImGui();
#endif // _DEBUG

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };

private: /* For. Frame Per Second*/
	_tchar					m_szFPS[MAX_STR];
	_int					m_iFps = { 0 };
	_float					m_fFpsTime = { 0.f };

#ifdef _DEBUG
private: /* ImGui */
	CImGui_Manager*			m_pImGui_Manager = { nullptr };
	LEVELID					m_eLevelID = { LEVEL_END };
	_bool					m_isStaticLoaded = { false };
#endif // _DEBUG

private: /* Singleton */
	class CMagicBallPool*	m_pMagicBallPool = { nullptr };
	class CQuest_Manager*	m_pQuest_Manager = { nullptr };

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END