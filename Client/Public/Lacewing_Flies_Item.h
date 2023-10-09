#pragma once
#include "Ingredient.h"

BEGIN(Client)

class CLacewing_Flies_Item final : public CIngredient
{
protected:
	CLacewing_Flies_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLacewing_Flies_Item(const CLacewing_Flies_Item& rhs);
	virtual ~CLacewing_Flies_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CLacewing_Flies_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

