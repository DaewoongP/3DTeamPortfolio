#include "ChineseChompingCabbageItem.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"

CChineseChompingCabbageItem::CChineseChompingCabbageItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool(pDevice, pContext)
{
}

CChineseChompingCabbageItem::CChineseChompingCabbageItem(const CChineseChompingCabbageItem& rhs)
	: CTool(rhs)
{
}

HRESULT CChineseChompingCabbageItem::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	m_ItemCreateDesc.eItemId = ITEM_ID_CHINESE_CHOMPING_CABBAGE;

	return S_OK;
}

HRESULT CChineseChompingCabbageItem::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CChineseChompingCabbageItem::Use(_float3 vPlayPos)
{

#ifdef _DEBUG
	cout_float3("중국산 깨무는 양배추 소환", vPlayPos);
#endif // _DEBUG

}

void CChineseChompingCabbageItem::CreateTool()
{
}

HRESULT CChineseChompingCabbageItem::Add_Components()
{


	return S_OK;
}

CChineseChompingCabbageItem* CChineseChompingCabbageItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CChineseChompingCabbageItem* pInstance = New CChineseChompingCabbageItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CChineseChompingCabbageItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChineseChompingCabbageItem::Clone(void* pArg)
{
	CChineseChompingCabbageItem* pInstance = New CChineseChompingCabbageItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CChineseChompingCabbageItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChineseChompingCabbageItem::Free()
{
	__super::Free();
}