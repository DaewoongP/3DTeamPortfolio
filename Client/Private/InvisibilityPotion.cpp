#include "InvisibilityPotion.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"

CInvisibilityPotion::CInvisibilityPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPotion(pDevice, pContext)
{
}

CInvisibilityPotion::CInvisibilityPotion(const CInvisibilityPotion& rhs)
	: CPotion(rhs)
{
}

HRESULT CInvisibilityPotion::Initialize_Prototype(_uint iLevel)
{	
	// 포션 정보
	m_PotionCreateDesc.wstrModelPath = TEXT("../../Resources/Models/NonAnims/SM_Invisibility_Bottle/SM_Invisibility_Bottle.dat"); // 모델경로
	m_PotionCreateDesc.Ingredients.push_back(INGREDIENT::LEAPING_TOADSTOOL_CAPS);	// 재료1
	m_PotionCreateDesc.Ingredients.push_back(INGREDIENT::KNOTGRASS);			// 재료2
	m_PotionCreateDesc.Ingredients.push_back(INGREDIENT::TROLL_BOGEYS);				// 재료3
	m_PotionCreateDesc.fManufacturingTime = 60.f;									// 제조 시간
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInvisibilityPotion::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CInvisibilityPotion::Duration(_float fTimeDelta)
{
	//m_fDuration += fTimeDelta;
	//if (m_fDuration >= m_ItemCreateDesc.fDuration)
	//	m_pPlayer->Set_Invisible(false);
}

CInvisibilityPotion* CInvisibilityPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CInvisibilityPotion* pInstance = New CInvisibilityPotion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CInvisibilityPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInvisibilityPotion::Clone(void* pArg)
{
	CInvisibilityPotion* pInstance = New CInvisibilityPotion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CInvisibilityPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInvisibilityPotion::Free()
{
	__super::Free();
}