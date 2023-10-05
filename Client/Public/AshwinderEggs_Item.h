#pragma once
#include "Ingredient.h"

BEGIN(Client)

class CAshwinderEggs_Item final : public CIngredient
{
protected:
	CAshwinderEggs_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAshwinderEggs_Item(const CAshwinderEggs_Item& rhs);
	virtual ~CAshwinderEggs_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CAshwinderEggs_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

