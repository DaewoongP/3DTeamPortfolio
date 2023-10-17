#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CMask_Demiguise final : public CGear_Item
{
protected:
	CMask_Demiguise(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMask_Demiguise(const CMask_Demiguise& rhs);
	virtual ~CMask_Demiguise() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CMask_Demiguise* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CMask_Demiguise* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END