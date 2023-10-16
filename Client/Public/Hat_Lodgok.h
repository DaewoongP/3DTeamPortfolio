#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CHat_Lodgok final : public CGear_Item
{
protected:
	CHat_Lodgok(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHat_Lodgok(const CHat_Lodgok& rhs);
	virtual ~CHat_Lodgok() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CHat_Lodgok* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CHat_Lodgok* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END