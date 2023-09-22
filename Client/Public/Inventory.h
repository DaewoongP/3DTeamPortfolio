#pragma once
#include "Component.h"
#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Client)
class CUI_Inventory;

class CInventory final : public CGameObject
{
public:
	enum ITEMTYPE
	{
		GEAR,
		RESOURCE,
		POTION,
		ITEMTYPE_END
	};
private:
	CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventory(const CInventory& rhs);
	virtual ~CInventory() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	_bool			m_isOpened = { false };

private:
	vector<vector<CGameObject*>>	m_pItems;
	CUI_Inventory*					m_pGearInventory[ITEMTYPE_END];

private:
	HRESULT Add_Components();

public:
	void	Add_Item();
	void	Delete_Item();

public:
	static CInventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END