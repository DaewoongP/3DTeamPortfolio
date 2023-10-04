#include "..\Public\Item.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"
#include "CustomModel.h"

CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CItem::CItem(const CItem& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
	, m_ItemCreateDesc(rhs.m_ItemCreateDesc)
{
}

HRESULT CItem::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iLevel = iLevel;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel,
		m_ItemCreateDesc.wstrUIPath.data()), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 현재 레벨 -> Layer_Player -> '플레이어의 주소'를 받아옴.
	m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(pGameInstance->Get_CurrentLevelIndex()
		, TEXT("Layer_Player")
		, TEXT("GameObject_Player")));
	
	// 플레이어 주소에 대한 유효성 검사.
	if (nullptr == m_pPlayer)
	{
		__debugbreak();
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	// 필요한 플레이어의 컴포넌트들을 받아옴.
	m_pPlayerModel = static_cast<CCustomModel*>(m_pPlayer->Find_Component(TEXT("Com_Model_CustomModel_Player")));
	m_pPlayerTransform = m_pPlayer->Get_Transform();
	m_pPlayerInformation = static_cast<CPlayer_Information*>(m_pPlayer->Find_Component(TEXT("Com_Player_Information")));

	return S_OK;
}

HRESULT CItem::Add_Components()
{
	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_ItemCreateDesc.wstrUIPath.data()).c_str()
		, TEXT("Com_UITexture"), reinterpret_cast<CComponent**>(&m_pUITexture)), E_FAIL);

	return S_OK;
}

void CItem::Free(void)
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pUITexture);
	}
}
