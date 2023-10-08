#pragma once
#include "Ingredient.h"

BEGIN(Client)

class CFluxweed_Stem_Item final : public CIngredient
{
protected:
	CFluxweed_Stem_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFluxweed_Stem_Item(const CFluxweed_Stem_Item& rhs);
	virtual ~CFluxweed_Stem_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CFluxweed_Stem_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

