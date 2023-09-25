#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)

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
	HRESULT Ready_Prototype_Object_For_Loading();
	HRESULT Ready_Fonts();
	HRESULT Open_Level(LEVELID eLevelIndex);

#ifdef _DEBUG
private:
	void Tick_FPS(_float fTimeDelta);

private: /* ImGui */
	HRESULT Initialize_ImGui();
	void Tick_ImGui();
	HRESULT Render_ImGui();
#endif // _DEBUG

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };

#ifdef _DEBUG
private: /* For. Frame Per Second*/
	_tchar					m_szFPS[MAX_STR];
	_int					m_iFps = { 0 };
	_float					m_fFpsTime = { 0.f };

private: /* ImGui */
	_float4					m_vImGuiClearColor;
	ID3D11Texture2D*		m_pTexture2D = { nullptr };
	ID3D11RenderTargetView* m_pImGuiRTV = { nullptr };
#endif // _DEBUG

private: /* Singleton */
	class CMagicBallPool*	m_pMagicBallPool = { nullptr };

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END