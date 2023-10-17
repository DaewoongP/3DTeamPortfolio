#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CBoots_Default final : public CGear_Item
{
protected:
	CBoots_Default(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoots_Default(const CBoots_Default& rhs);
	virtual ~CBoots_Default() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CBoots_Default* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CBoots_Default* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END