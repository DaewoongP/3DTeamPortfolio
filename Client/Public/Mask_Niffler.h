#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CMask_Niffler final : public CGear_Item
{
protected:
	CMask_Niffler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMask_Niffler(const CMask_Niffler& rhs);
	virtual ~CMask_Niffler() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CMask_Niffler* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CMask_Niffler* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END