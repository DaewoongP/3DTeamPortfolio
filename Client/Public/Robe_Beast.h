#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CRobe_Beast final : public CGear_Item
{
protected:
	CRobe_Beast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRobe_Beast(const CRobe_Beast& rhs);
	virtual ~CRobe_Beast() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CRobe_Beast* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CRobe_Beast* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END