#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	explicit CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVELID eNextLevelID, _bool isStaticLoaded);
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private: /* Loaders */
	// 다음 레벨을 구성하기위한 쓰레드 생성 객체
	class CMain0_Loader*	m_pMain0_Loader = { nullptr };
	class CMain1_Loader*	m_pMain1_Loader = { nullptr };
	class CMain2_Loader*	m_pMain2_Loader = { nullptr };
	class CMain3_Loader*	m_pMain3_Loader = { nullptr };

	LEVELID					m_eNextLevelID = { LEVEL_END };

#ifdef _DEBUG
private:
	wstring					m_szFinishedLoader = TEXT("");
#endif // _DEBUG

private:
	HRESULT Loading_Logo(const _tchar* pLayerTag);
	HRESULT Loading_Cliffside(const _tchar* pLayerTag);
	HRESULT Loading_Vault(const _tchar* pLayerTag);
	HRESULT Loading_GreatHall(const _tchar* pLayerTag);

private:
	_bool Check_Thread_ExitCode(HANDLE hWnd);
#ifdef _DEBUG
	void Render_Finished();
#endif // _DEBUG

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID, _bool isStaticLoaded = true);
	virtual void Free() override;
};

END