#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CJacket_Arcane_A final : public CGear_Item
{
protected:
	CJacket_Arcane_A(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJacket_Arcane_A(const CJacket_Arcane_A& rhs);
	virtual ~CJacket_Arcane_A() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CJacket_Arcane_A* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CJacket_Arcane_A* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END