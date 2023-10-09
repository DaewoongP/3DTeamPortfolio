#pragma once
#include "Component.h"
#include "Client_Defines.h"
#include "GameObject.h"
#include "Item.h"

BEGIN(Client)
class CUI_Inventory;

class CInventory final : public CGameObject
{
public:
	void Set_CurItemtype(ITEMTYPE eType) {
		m_eCurOpenItemtype = eType;
	}

	void	Set_Open(_bool isOpen) {
		m_isOpen = isOpen;
	}

private:
	CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventory(const CInventory& rhs);
	virtual ~CInventory() = default;

public:
	_bool Get_Open() { return m_isOpen; }
	_uint Get_Resource(INGREDIENT eType) {
		return m_ResourcesCount[eType];
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	_bool			m_isOpen = { false };
	ITEMTYPE			m_eCurOpenItemtype = { ITEMTYPE_END };

private:
	// UI 컴포넌트
	CUI_Inventory*				m_pUI_Inventory[ITEMTYPE_END];
	// 실질적인 아이템 저장소
	vector<vector<CItem*>>			m_pItems;
	// 현재 플레이어의 장비 포인터
	vector<CItem*>				m_pPlayerCurItems;


	
private: // For Resources
	vector<_uint> m_ResourcesCount;

private:
	_uint iGearMax = { 20 };
	_uint iResourceMax = { 30 };

private:
	HRESULT Add_Components();

public:
	void	Add_Item(CItem* pItem, ITEMTYPE eType);
	void	Delete_Item(ITEMTYPE eType, _uint iIndex);
	void	Swap_Item(_uint Index, ITEMTYPE eType);

public:
	static CInventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END