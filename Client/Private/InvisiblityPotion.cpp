#include "InvisiblityPotion.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"

CInvisiblityPotion::CInvisiblityPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPotion(pDevice, pContext)
{
}

CInvisiblityPotion::CInvisiblityPotion(const CInvisiblityPotion& rhs)
	: CPotion(rhs)
	, m_fRecoveryAmount(rhs.m_fRecoveryAmount)
{
}

HRESULT CInvisiblityPotion::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.iCost = 500;											// 가격
	m_ItemCreateDesc.fDuration = 4.f;										//지속시간
	m_ItemCreateDesc.wstrKoreanName = TEXT("투명 마법약");					// 한글명
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_InvisibilityPotion.png"); // UI경로
	m_ItemCreateDesc.wstrModelPath = TEXT("../../Resources/Models/NonAnims/SM_SpherePrimitiveRegularNormals_01/SM_SpherePrimitiveRegularNormals_01.dat"); // 모델경로

	// 포션 정보
	m_PotionCreateDesc.Ingredients.push_back(INGREDIENT::LEAPING_TOADSTOOL_CAPS);	// 재료1
	m_PotionCreateDesc.Ingredients.push_back(INGREDIENT::KNOTGRASS_SPRIG);			// 재료2
	m_PotionCreateDesc.Ingredients.push_back(INGREDIENT::TROLL_BOGEYS);			// 재료3
	m_PotionCreateDesc.fManufacturingTime = 60.f;									// 제조 시간

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInvisiblityPotion::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CInvisiblityPotion::Use(_float3 vPlayPos)
{
	m_pPlayer->Set_Invisible(true);

}

void CInvisiblityPotion::Duration(_float fTimeDelta)
{
	m_fDuration += fTimeDelta;
	if (m_fDuration >= m_ItemCreateDesc.fDuration)
		m_pPlayer->Set_Invisible(false);
}

CInvisiblityPotion* CInvisiblityPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CInvisiblityPotion* pInstance = New CInvisiblityPotion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CInvisiblityPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInvisiblityPotion::Clone(void* pArg)
{
	CInvisiblityPotion* pInstance = New CInvisiblityPotion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CInvisiblityPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInvisiblityPotion::Free()
{
	__super::Free();
	Safe_Release(m_pPlayerHealthCom);
	Safe_Release(m_pPlayerInformation);
}