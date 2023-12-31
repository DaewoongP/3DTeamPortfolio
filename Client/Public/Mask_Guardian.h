#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CMask_Guardian final : public CGear_Item
{
protected:
	CMask_Guardian(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMask_Guardian(const CMask_Guardian& rhs);
	virtual ~CMask_Guardian() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CMask_Guardian* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CMask_Guardian* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END