#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CRobe_Arcane final : public CGear_Item
{
protected:
	CRobe_Arcane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRobe_Arcane(const CRobe_Arcane& rhs);
	virtual ~CRobe_Arcane() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CRobe_Arcane* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CRobe_Arcane* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END