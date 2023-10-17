#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CPants_Arcane final : public CGear_Item
{
protected:
	CPants_Arcane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPants_Arcane(const CPants_Arcane& rhs);
	virtual ~CPants_Arcane() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CPants_Arcane* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CPants_Arcane* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END