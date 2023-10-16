#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CHat_Fedora final : public CGear_Item
{
protected:
	CHat_Fedora(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHat_Fedora(const CHat_Fedora& rhs);
	virtual ~CHat_Fedora() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CHat_Fedora* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CHat_Fedora* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END