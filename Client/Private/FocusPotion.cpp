#include "FocusPotion.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"
#include"CoolTime.h"

CFocusPotion::CFocusPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPotion(pDevice, pContext)
{
}

CFocusPotion::CFocusPotion(const CFocusPotion& rhs)
	: CPotion(rhs)
{
}

HRESULT CFocusPotion::Initialize_Prototype(_uint iLevel)
{
	// 포션 정보
	m_PotionCreateDesc.wstrModelPath = TEXT("../../Resources/Models/NonAnims/SM_Focus_Bottle/SM_Focus_Bottle.dat"); // 모델경로
	m_PotionCreateDesc.Ingredients.push_back(FLUXWEED_STEM);		// 재료1
	m_PotionCreateDesc.Ingredients.push_back(DITTANY_LEAVES);		// 재료2
	m_PotionCreateDesc.fManufacturingTime = 60.f;					// 제조시간

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFocusPotion::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pLifeTime->Set_MaxCoolTime(m_fDuration + 2.f);

	return S_OK;
}

void CFocusPotion::Duration(_float fTimeDelta)
{
	/*m_fDuration += fTimeDelta;
	if (m_fDuration >= m_ItemCreateDesc.fDuration)
		m_pPlayer->Set_FocusOn(false);*/
}

CFocusPotion* CFocusPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CFocusPotion* pInstance = New CFocusPotion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CFocusPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFocusPotion::Clone(void* pArg)
{
	CFocusPotion* pInstance = New CFocusPotion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CFocusPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFocusPotion::Free()
{
	__super::Free();
}
