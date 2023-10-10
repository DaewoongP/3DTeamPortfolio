#pragma once
class Mongrel_Fur_Item
{
};

#pragma once
#include "Ingredient.h"

BEGIN(Client)

class CMongrel_Fur_Item final : public CIngredient
{
protected:
	CMongrel_Fur_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMongrel_Fur_Item(const CMongrel_Fur_Item& rhs);
	virtual ~CMongrel_Fur_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CMongrel_Fur_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END