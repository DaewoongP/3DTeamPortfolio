#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMain2_Loader final : public CBase
{
private:
	explicit CMain2_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMain2_Loader() = default;

public:
	const HANDLE Get_Thread() const { return m_hThread; }
	_bool Get_Finished() const { return m_isFinished; }

public:
	HRESULT Initialize(LEVELID eNextLevel);
	HRESULT Loading();

private:
	HANDLE					m_hThread = { 0 };
	CRITICAL_SECTION		m_Critical_Section;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	LEVELID					m_eNextLevelID = { LEVELID::LEVEL_END };
	_bool					m_isFinished = { false };

private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_Cliffside();
	HRESULT Loading_For_Vault();

public:
	static CMain2_Loader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel);
	virtual void Free() override;
};

END