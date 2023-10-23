#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class COutfit_Arcane final : public CGear_Item
{
protected:
	COutfit_Arcane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COutfit_Arcane(const COutfit_Arcane& rhs);
	virtual ~COutfit_Arcane() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static COutfit_Arcane* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual COutfit_Arcane* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END