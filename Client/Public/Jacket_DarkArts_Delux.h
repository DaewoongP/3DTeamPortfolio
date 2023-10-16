#pragma once
#include "Gear_Item.h"

BEGIN(Client)

class CJacket_DarkArts_Delux final : public CGear_Item
{
protected:
	CJacket_DarkArts_Delux(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJacket_DarkArts_Delux(const CJacket_DarkArts_Delux& rhs);
	virtual ~CJacket_DarkArts_Delux() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Buy() override;
	virtual void Equipment() override;

public:
	static CJacket_DarkArts_Delux* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CJacket_DarkArts_Delux* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END