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
		HAND,
		FACE,
		HEAD,
		NECK,
		BACK,
		OUTFIT,
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
	ITEMTYPE		m_eCurOpenItemtype = { ITEMTYPE_END };

private:
	// UI 컴포넌트
	CUI_Inventory* m_pInventory[ITEMTYPE_END];
	// 실질적인 아이템 저장소
	vector<vector<CGameObject*>>	m_pItems;

private:
	_uint iGearMax = { 20 };
	_uint iResourceMax = { 30 };

private:
	HRESULT Add_Components();

public:
	void	Add_Item(CGameObject* pItem, ITEMTYPE eType);
	void	Delete_Item(ITEMTYPE eType, _uint iIndex);

public:
	static CInventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END