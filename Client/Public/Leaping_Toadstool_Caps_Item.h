#pragma once
#include "Ingredient.h"

BEGIN(Client)

class CLeaping_Toadstool_Caps_Item final : public CIngredient
{
protected:
	CLeaping_Toadstool_Caps_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLeaping_Toadstool_Caps_Item(const CLeaping_Toadstool_Caps_Item& rhs);
	virtual ~CLeaping_Toadstool_Caps_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CLeaping_Toadstool_Caps_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

