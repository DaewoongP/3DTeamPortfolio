#pragma once
#include "Base.h"
#include <mutex>
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CSeamless_Loader final : public CBase
{
private:
	explicit CSeamless_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSeamless_Loader() = default;

public:
	const HANDLE Get_Thread() const { return m_hThread; }
	const _tchar* Get_LoadingText() const { return m_szLoading; }
	_uint Get_LoadingPercent() const { return m_iLoadingPercent; }
	_bool Get_Finished() const { return m_isFinished; }

public:
	HRESULT Initialize();
	HRESULT Loading();

public:
	HRESULT Ready_Layer_Debug(const _tchar* pLayerTag);

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
	_tchar					m_szLoading[MAX_STR] = TEXT("");
	_bool					m_isFinished = { false };
	_uint					m_iLoadingPercent = { 0 };

public:
	static CSeamless_Loader* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual void Free() override;
};

END