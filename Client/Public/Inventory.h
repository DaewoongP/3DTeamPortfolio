#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CItem;
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
	vector<CItem*>* Get_CurItem() {
		return &m_pPlayerCurItems;
	}
	
	_uint Get_Gold() { return m_iGold; }
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public: // 아이템 구입 로직
	_bool Can_Purchase(_uint iCost);
	void Pay_Money(_uint iCost);
	void Earn_Money(_uint iCost);

private:
	_bool				m_isOpen = { false };
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
	_uint m_iGold = { 5000 };

private:
	HRESULT Add_Components();

public:
	_bool	Add_Item(CItem* pItem, ITEMTYPE eType);
	_bool	Add_Item(const _tchar* pPrototypeTag, _uint iLevel = LEVEL_STATIC, void* pArg = nullptr);
	_bool	Add_Item(ITEM_ID eItemID, _uint iLevel = LEVEL_STATIC, void* pArg = nullptr);
	_bool	Setting_Item(ITEM_ID eItemID, _uint iLevel = LEVEL_STATIC, void* pArg = nullptr);


	void	Delete_Item(ITEM_ID eTargetItemID);
	_bool	Delete_Item(ITEMTYPE eType, CItem* pItem);
	_bool	Delete_Item(ITEMTYPE eType, _uint iIndex);
	void	Swap_Item(_uint Index, ITEMTYPE eType);


private:
	_bool		m_isSetting = { true };

public:
	static CInventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END