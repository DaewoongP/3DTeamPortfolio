#pragma once
#include "Ingredient.h"

BEGIN(Client)

class CSpider_Fang_Item final : public CIngredient
{
protected:
	CSpider_Fang_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpider_Fang_Item(const CSpider_Fang_Item& rhs);
	virtual ~CSpider_Fang_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CSpider_Fang_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

