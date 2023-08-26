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
	// ���� ��ü���� ����ϱ� ���� ����ƽ ������Ʈ ���� �Լ�
	HRESULT Ready_Prototype_Component_For_Static();
	HRESULT Ready_Prototype_Object_For_Loading();
	HRESULT Ready_Fonts();
	HRESULT Open_Level(LEVELID eLevelIndex);

#ifdef _DEBUG
private:
	void Tick_FPS(_float fTimeDelta);
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
#endif // _DEBUG

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END