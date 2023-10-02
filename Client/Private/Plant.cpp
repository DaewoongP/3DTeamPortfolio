#include "..\Public\Plant.h"

CPlant::CPlant(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CPlant::CPlant(const CPlant& rhs)
	: CItem(rhs)
{
}

HRESULT CPlant::Initialize_Prototype(_uint iLevel)
{
	return S_OK;
}

HRESULT CPlant::Initialize(void* pArg)
{
	return S_OK;
}

void CPlant::Free(void)
{
	__super::Free();
}
