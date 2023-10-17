#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CHat_DarkArts_Delux final : public CGear_Item
{
protected:
	CHat_DarkArts_Delux(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHat_DarkArts_Delux(const CHat_DarkArts_Delux& rhs);
	virtual ~CHat_DarkArts_Delux() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CHat_DarkArts_Delux* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CHat_DarkArts_Delux* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END