#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CJacket_Celtic final : public CGear_Item
{
protected:
	CJacket_Celtic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJacket_Celtic(const CJacket_Celtic& rhs);
	virtual ~CJacket_Celtic() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CJacket_Celtic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CJacket_Celtic* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END