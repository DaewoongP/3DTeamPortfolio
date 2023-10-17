#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CJacket_DragonGrab_A final : public CGear_Item
{
protected:
	CJacket_DragonGrab_A(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJacket_DragonGrab_A(const CJacket_DragonGrab_A& rhs);
	virtual ~CJacket_DragonGrab_A() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CJacket_DragonGrab_A* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CJacket_DragonGrab_A* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END