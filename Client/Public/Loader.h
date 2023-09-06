#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	explicit CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	const HANDLE Get_Thread() const { return m_hThread; }
	const _tchar* Get_LoadingText() const { return m_szLoading; }
	_uint Get_LoadingPercent() const { return m_iLoadingPercent; }
	_bool Get_Finished() const { return m_isFinished; }

public:
	HRESULT Initialize(LEVELID eNextLevel);
	HRESULT Loading();

private:
	// ������ ���� �ڵ�
	HANDLE					m_hThread = { 0 };
	// ���� �����忡�� �ش� �����尡 ũ��Ƽ�� ������ ����� ������ �ٸ� �����尡 ������� ���ϰ� ó�� �ϴ� ����.
	CRITICAL_SECTION		m_Critical_Section;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	LEVELID					m_eNextLevelID = { LEVELID::LEVEL_END };
	// �ε� ���� Ȯ�ο� ���ڿ� ����
	_tchar					m_szLoading[MAX_STR] = TEXT("");
	// �ε��� �������� Ȯ���ϴ� ����
	_bool					m_isFinished = { false };

	_uint					m_iLoadingPercent = { 0 };

private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_MainGame();

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel);
	virtual void Free() override;
};

END