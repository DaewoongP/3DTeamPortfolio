#pragma once
#include "Quest.h"

BEGIN(Client)
class CUI_Font;

class CQuest_Potion final : public CQuest
{
private:
	explicit CQuest_Potion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CQuest_Potion() = default;

public:
	void Set_Clear() { m_isClear = true; }
	virtual _bool Is_Finished() override;

public:
	HRESULT Initialize();
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT		Add_Component();

public:
	virtual void	Clear_Quest();

private:
	_uint		m_iRequestSize = { 10 };
	_uint		m_iCurResourceSize = { 0 };
	_uint		m_iPreResourceSize = { 0 };

	_bool		m_isClear = { false };
	_bool		m_isCheckResource = { true };

	class CInventory* m_pInventory = { nullptr };

	CUI_Font* m_pExplainFont = { nullptr };
	CUI_Font* m_pCountFont = { nullptr };


public:
	static CQuest_Potion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END