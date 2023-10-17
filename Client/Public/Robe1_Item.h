#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CRobe1_Item final : public CGear_Item
{
protected:
	CRobe1_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRobe1_Item(const CRobe1_Item& rhs);
	virtual ~CRobe1_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CRobe1_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END