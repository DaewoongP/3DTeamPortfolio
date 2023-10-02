#pragma once

#include "Item.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CCoolTime;
class CHealth;
END

BEGIN(Client)

class CIngredient abstract : public CItem
{
protected:
	typedef struct tagIngredientCreateDesc
	{
		INGREDIENT		eIngredient = { INGREDIENT_END };
		INGREDIENT_TYPE eIngredientType = { INGREDIENT_TYPE_END };
	}INGREDIENT_CREATE_DESC;

protected:
	typedef struct tagIngredientCloneDesc : public tagItemCloneDesc
	{
		
	}INGREDIENT_CLONE_DESC;

private:
	CIngredient(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIngredient(const CIngredient& rhs);
	virtual ~CIngredient() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

private:
	HRESULT Ready_Ingredient_UITexture();
	void Setting_By_Ingredient();

protected:
	INGREDIENT_CREATE_DESC m_IngredientDesc = { INGREDIENT_CREATE_DESC() };

public:
	virtual void Free(void) override;
};
END

