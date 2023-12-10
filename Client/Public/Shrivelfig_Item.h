#pragma once
#include "Ingredient.h"

BEGIN(Client)

class CShrivelfig_Item final : public CIngredient
{
protected:
	CShrivelfig_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShrivelfig_Item(const CShrivelfig_Item& rhs);
	virtual ~CShrivelfig_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CShrivelfig_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

