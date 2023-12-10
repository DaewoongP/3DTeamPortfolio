#pragma once
#include "Ingredient.h"

BEGIN(Client)

class CDittany_Leaves_Item final : public CIngredient
{
protected:
	CDittany_Leaves_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDittany_Leaves_Item(const CDittany_Leaves_Item& rhs);
	virtual ~CDittany_Leaves_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CDittany_Leaves_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

