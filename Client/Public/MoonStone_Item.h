#pragma once
#include "Ingredient.h"

BEGIN(Client)

class CMoonStone_Item final : public CIngredient
{
protected:
	CMoonStone_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMoonStone_Item(const CMoonStone_Item& rhs);
	virtual ~CMoonStone_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CMoonStone_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END