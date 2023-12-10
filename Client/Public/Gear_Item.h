#pragma once
#include "Item.h"
BEGIN(Client)

class CGear_Item abstract : public CItem
{
protected:

	typedef struct tagGear_ItemCreateDesc
	{
	}GEAR_ITEM_CRATE_DESC;

protected:
	CGear_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGear_Item(const CGear_Item& rhs);
	virtual ~CGear_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Equipment() PURE;

protected:
	GEAR_ITEM_CRATE_DESC m_Gear_ItemCreateDesc;

public:
	virtual void Free(void) override;
};
END

