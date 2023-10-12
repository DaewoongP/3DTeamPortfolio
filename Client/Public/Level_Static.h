#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLevel_Static final : public CBase
{
	DECLARE_SINGLETON(CLevel_Static)
private:
	explicit CLevel_Static() = default;
	virtual ~CLevel_Static() = default;

public:
	HRESULT Initialize();

private:
	HRESULT Ready_Layer_Player(const _tchar * pLayerTag);
	HRESULT Ready_Layer_Inventory(const _tchar * pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar * pLayerTag);
	HRESULT Ready_Layer_FieldGuide_UI(const _tchar * pLayerTag);
	HRESULT Ready_Layer_Menu_UI(const _tchar * pLayerTag);

private:
	HRESULT Add_Scene();

#ifdef _DEBUG
private:
	HRESULT Ready_Layer_Debug(const _tchar * pLayerTag);
#endif // _DEBUG

private:
	class CGameInstance*		m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

END

#define LOAD_STATIC_LEVEL		CLevel_Static::GetInstance()->Initialize()