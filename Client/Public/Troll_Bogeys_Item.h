#pragma once
#include "Ingredient.h"

BEGIN(Client)

class CTroll_Bogeys_Item final : public CIngredient
{
protected:
	CTroll_Bogeys_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTroll_Bogeys_Item(const CTroll_Bogeys_Item& rhs);
	virtual ~CTroll_Bogeys_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CTroll_Bogeys_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

