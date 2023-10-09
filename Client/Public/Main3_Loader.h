#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMain3_Loader final : public CBase
{
private:
	explicit CMain3_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMain3_Loader() = default;

public:
	const HANDLE Get_Thread() const { return m_hThread; }
	_bool Get_Finished() const { return m_isFinished; }

public:
	HRESULT Initialize(LEVELID eNextLevel, _bool isStaticLoaded);
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
	_bool					m_isStaticLoaded = { false };

private:
	HRESULT Loading_For_Logo(LEVELID eLevelID);
	HRESULT Loading_For_Cliffside(LEVELID eLevelID);
	HRESULT Loading_For_Vault(LEVELID eLevelID);
	HRESULT Loading_For_GreatHall(LEVELID eLevelID);
	HRESULT Loading_For_Hogsmeade(LEVELID eLevelID);
	HRESULT Loading_For_Sanctum(LEVELID eLevelID);

private:
	HRESULT Loading_For_Static(LEVELID eLevelID);

public:
	static CMain3_Loader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel, _bool isStaticLoaded);
	virtual void Free() override;
};

END