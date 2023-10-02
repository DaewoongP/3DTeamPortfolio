#include "Item.h"
#include "GameInstance.h"

CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CItem::CItem(const CItem& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
	, m_wstrPrototypeName(rhs.m_wstrPrototypeName)
	, m_eItemType(rhs.m_eItemType)
{
}

HRESULT CItem::Initialize_Prototype(_uint iLevel, const _tchar* pUIImagePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	m_iLevel = iLevel;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (nullptr == pUIImagePath)
	{
		FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel,
			TEXT("../../Resources/Default/Game/Textures/Logo/Logo.png")), E_FAIL);
		m_wstrPrototypeName = TEXT("Prototype_Component_Texture_Logo");
	}
	else
	{
		FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel,
			pUIImagePath), E_FAIL);
		m_wstrPrototypeName = ToPrototypeTag(TEXT("Prototype_Component_Texture"), pUIImagePath);
	}

	Safe_Release(pGameInstance);
	
	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CItem::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}


HRESULT CItem::Add_Components()
{
	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, m_wstrPrototypeName.data()
		, TEXT("Com_UITexture"), reinterpret_cast<CComponent**>(&m_pUITexture)), E_FAIL);

	return S_OK;
}

void CItem::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pUITexture);
	}
}