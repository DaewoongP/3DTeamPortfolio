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
	// 아이템 정보
	m_ItemCreateDesc.iCost = 500;									// 가격
	m_ItemCreateDesc.fDuration = 15.f;								//지속시간
	m_ItemCreateDesc.wstrKoreanName = TEXT("집중력 물약");			// 한글명
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_AMFillPotion.png"); // UI경로

	// 포션 정보
	m_PotionCreateDesc.wstrModelPath = TEXT("../../Resources/Models/NonAnims/SM_Health_Bottle/SM_Health_Bottle.dat"); // 모델경로
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

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CFocusPotion::Use(_float3 vPlayPos)
{
	__super::Use(vPlayPos);

	cout << "쿨타임이 감소했어요" << '\n';
	m_pPlayer->Set_FocusOn(true);
}

void CFocusPotion::Duration(_float fTimeDelta)
{
	m_fDuration += fTimeDelta;
	if (m_fDuration >= m_ItemCreateDesc.fDuration)
		m_pPlayer->Set_FocusOn(false);
}

HRESULT CFocusPotion::Add_Components()
{
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CoolTime"),
		TEXT("Com_CoolTime"), reinterpret_cast<CComponent**>(&m_pCoolTime))))
	{
		__debugbreak();
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CFocusPotion::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);



	Safe_Release(pGameInstance);
	return S_OK;
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
	Safe_Release(m_pCoolTime);
}
