#pragma once
#include "Item.h"

BEGIN(Client)

class CIngredient abstract : public CItem
{
protected:

	typedef struct tagIngredientCreateDesc
	{
		INGREDIENT_TYPE			eIngredientType;
		INGREDIENT				eIngredient;
	}INGREDIENT_CRATE_DESC;

protected:
	CIngredient(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIngredient(const CIngredient& rhs);
	virtual ~CIngredient() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	INGREDIENT Get_Ingredient() {
		return m_IngredientCreateDesc.eIngredient;
	}

protected:
	//HRESULT Ready_Ingredient_UITexture();

	void Setting_By_Ingredient();

protected:
	INGREDIENT_CRATE_DESC m_IngredientCreateDesc;

public:
	virtual void Free(void) override;
};
END

