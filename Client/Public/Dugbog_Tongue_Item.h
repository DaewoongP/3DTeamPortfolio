#pragma once
#include "Ingredient.h"

BEGIN(Client)

class CDugbog_Tongue_Item final : public CIngredient
{
protected:
	CDugbog_Tongue_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDugbog_Tongue_Item(const CDugbog_Tongue_Item& rhs);
	virtual ~CDugbog_Tongue_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CDugbog_Tongue_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

