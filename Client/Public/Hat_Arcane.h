#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CHat_Arcane final : public CGear_Item
{
protected:
	CHat_Arcane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHat_Arcane(const CHat_Arcane& rhs);
	virtual ~CHat_Arcane() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CHat_Arcane* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CHat_Arcane* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END