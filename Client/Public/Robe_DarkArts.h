#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CRobe_DarkArts final : public CGear_Item
{
protected:
	CRobe_DarkArts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRobe_DarkArts(const CRobe_DarkArts& rhs);
	virtual ~CRobe_DarkArts() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CRobe_DarkArts* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CRobe_DarkArts* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END