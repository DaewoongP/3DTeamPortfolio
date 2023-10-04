#pragma once
#include "Item.h"

BEGIN(Client)

class CIngredient abstract : public CItem
{
	typedef struct tagIngredientCreateDesc
	{
		INGREDIENT_TYPE			eIngredientType;
		INGREDIENT				eIngredient;
	}INGREDIENT_CREATE_DESC;

protected:
	CIngredient(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIngredient(const CIngredient& rhs);
	virtual ~CIngredient() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

protected:
	void Setting_By_Ingredient();

protected:
	INGREDIENT_CREATE_DESC	m_IngredientCreateDesc;
	INGREDIENT_TYPE			m_eIngredientType;
	INGREDIENT				m_eIngredient;

public:
	virtual void Free(void) override;
};
END

