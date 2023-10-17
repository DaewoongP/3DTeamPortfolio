#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CHat_Wizard final : public CGear_Item
{
protected:
	CHat_Wizard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHat_Wizard(const CHat_Wizard& rhs);
	virtual ~CHat_Wizard() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CHat_Wizard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CHat_Wizard* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END