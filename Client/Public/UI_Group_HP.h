#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CUI_Progress;
END

BEGIN(Client)
class CUI_Back;
class CUI_HP;
class CUI_Font;

class CUI_Group_HP final : public CGameObject
{
public:
	struct HPDESC
	{
		_tchar wszFilePath[MAX_PATH] = TEXT("");
		_tchar wszNumber[MAX_PATH] = TEXT("");
		CHealth* pHealth = { nullptr };
	};

private:
	explicit CUI_Group_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group_HP(const CUI_Group_HP& rhs);
	virtual ~CUI_Group_HP() = default;

public:
	void	Set_Potion(_uint iPotion);
	void	Set_HP(_float fGauge);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CUI_Back*			m_pUI_Back = { nullptr };
	CUI_HP*				m_pUI_HP = { nullptr };

	CHealth*			m_pHealth = { nullptr };

private:
	CUI_Font*			m_pFont = { nullptr };
//	_tchar				m_wszObjectLevel[MAX_PATH] = TEXT("");

private:
	HRESULT Add_Prototype();
	HRESULT Add_Components();
	HRESULT Read_File(const _tchar* pFilePath);
	CUI::UIDESC Load_File(const HANDLE hFile);

private:
	HRESULT Add_Fonts(_tchar* wszLevel);

public:
	static CUI_Group_HP* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END