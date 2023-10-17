#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CBoots_Merlin final : public CGear_Item
{
protected:
	CBoots_Merlin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoots_Merlin(const CBoots_Merlin& rhs);
	virtual ~CBoots_Merlin() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CBoots_Merlin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CBoots_Merlin* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END