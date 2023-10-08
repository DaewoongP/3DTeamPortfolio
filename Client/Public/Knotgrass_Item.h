#pragma once
#include "Ingredient.h"

BEGIN(Client)

class CKnotgrass_Item final : public CIngredient
{
protected:
	CKnotgrass_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKnotgrass_Item(const CKnotgrass_Item& rhs);
	virtual ~CKnotgrass_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CKnotgrass_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

