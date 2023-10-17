#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CPants_Merlin final : public CGear_Item
{
protected:
	CPants_Merlin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPants_Merlin(const CPants_Merlin& rhs);
	virtual ~CPants_Merlin() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CPants_Merlin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CPants_Merlin* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END