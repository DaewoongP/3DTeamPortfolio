#pragma once

#include "Ingredient.h"

BEGIN(Client)
class CHorklumpJuice_Item final : public CIngredient
{
public:
	typedef struct tagHorklumpJuice_ItemCloneDesc : public tagIngredientCloneDesc
	{
		// 아무것도 없어도 만들어놓기.
	}CLONE_DESC;

private:
	CHorklumpJuice_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHorklumpJuice_Item(const CHorklumpJuice_Item& rhs);
	virtual ~CHorklumpJuice_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CHorklumpJuice_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END