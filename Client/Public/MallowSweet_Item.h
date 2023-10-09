#pragma once
#include "Ingredient.h"

BEGIN(Client)

class CMallowSweet_Item final : public CIngredient
{
protected:
	CMallowSweet_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMallowSweet_Item(const CMallowSweet_Item& rhs);
	virtual ~CMallowSweet_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CMallowSweet_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

