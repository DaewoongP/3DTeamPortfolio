#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CPants_Stealth final : public CGear_Item
{
protected:
	CPants_Stealth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPants_Stealth(const CPants_Stealth& rhs);
	virtual ~CPants_Stealth() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CPants_Stealth* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CPants_Stealth* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END