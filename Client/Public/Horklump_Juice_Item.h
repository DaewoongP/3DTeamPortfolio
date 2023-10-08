#pragma once
#include "Ingredient.h"

BEGIN(Client)

class CHorklump_Juice_Item final : public CIngredient
{
protected:
	CHorklump_Juice_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHorklump_Juice_Item(const CHorklump_Juice_Item& rhs);
	virtual ~CHorklump_Juice_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CHorklump_Juice_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

