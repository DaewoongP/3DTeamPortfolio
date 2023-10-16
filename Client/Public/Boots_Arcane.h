#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CBoots_Arcane final : public CGear_Item
{
protected:
	CBoots_Arcane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoots_Arcane(const CBoots_Arcane& rhs);
	virtual ~CBoots_Arcane() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CBoots_Arcane* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CBoots_Arcane* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END