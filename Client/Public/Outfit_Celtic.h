#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class COutfit_Celtic final : public CGear_Item
{
protected:
	COutfit_Celtic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COutfit_Celtic(const COutfit_Celtic& rhs);
	virtual ~COutfit_Celtic() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static COutfit_Celtic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual COutfit_Celtic* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END