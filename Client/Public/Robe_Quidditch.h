#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CRobe_Quidditch final : public CGear_Item
{
protected:
	CRobe_Quidditch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRobe_Quidditch(const CRobe_Quidditch& rhs);
	virtual ~CRobe_Quidditch() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CRobe_Quidditch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CRobe_Quidditch* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END