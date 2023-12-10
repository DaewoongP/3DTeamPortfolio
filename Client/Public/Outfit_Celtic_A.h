#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class COutfit_Celtic_A final : public CGear_Item
{
protected:
	COutfit_Celtic_A(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COutfit_Celtic_A(const COutfit_Celtic_A& rhs);
	virtual ~COutfit_Celtic_A() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static COutfit_Celtic_A* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual COutfit_Celtic_A* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END