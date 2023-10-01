#pragma once

#include "Item.h"
#include "Engine_Defines.h"
#include "Client_Defines.h"



BEGIN(Client)
class CItemFactory final : CBase
{
	DECLARE_SINGLETON(CItemFactory);

public:
	CItemFactory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CItemFactory() = default;

	CItem* CreateItem();
};
END
