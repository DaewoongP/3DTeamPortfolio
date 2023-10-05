#include "EdurusPotion.h"
#include"GameInstance.h"
#include"Player.h"
#include"Player_Information.h"  
#include"CoolTime.h"
#include"Defence.h"

CEdurusPotion::CEdurusPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPotion(pDevice, pContext)
{
}

CEdurusPotion::CEdurusPotion(const CEdurusPotion& rhs)
	: CPotion(rhs)
{
}

HRESULT CEdurusPotion::Initialize_Prototype(_uint iLevel)
{	
	// 아이템 정보
	m_ItemCreateDesc.iCost = 300;											// 가격
	m_ItemCreateDesc.fDuration = 20.f;										//지속시간
	m_ItemCreateDesc.wstrKoreanName = TEXT("에두루스 마법약");					// 한글명
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_Edurus.png"); // UI경로
	
	// 포션 정보
	m_PotionCreateDesc.Ingredients.push_back(INGREDIENT::ASHWINDER_EGGS);	// 재료1
	m_PotionCreateDesc.Ingredients.push_back(INGREDIENT::MONGREL_FUR);	// 재료2
	m_PotionCreateDesc.fManufacturingTime = 30.f;							// 제조 시간
	m_PotionCreateDesc.wstrModelPath = TEXT("../../Resources/Models/NonAnims/SM_Edurus_Bottle/SM_Edurus_Bottle.dat"); // 모델경로

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEdurusPotion::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CEdurusPotion::Duration(_float fTimeDelta)
{
	m_fDuration += fTimeDelta;
	if (m_fDuration >= m_ItemCreateDesc.fDuration)
	{
		m_pPlayer->Set_DefUp(false);
	}
}

void CEdurusPotion::Use(_float3 vPlayPos)
{
	__super::Use(vPlayPos);

	m_pPlayer->Set_DefUp(true);

	m_pDefence->Set_PotionDefence(15);
}

HRESULT CEdurusPotion::Add_Components()
{

	CCoolTime::COOLTIME_DESC MaxCool;
	MaxCool.fMaxCoolTime = 20.f;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CoolTime"),
		TEXT("Com_CoolTime"), reinterpret_cast<CComponent**>(&m_pCooltime), &MaxCool)))
	{
		__debugbreak();
		return E_FAIL;
	}

	CDefence::DEFFENCEDESC DefValue;
	DefValue.iDeffence = 0;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Defence"),
		TEXT("Com_Defence"), reinterpret_cast<CComponent**>(&m_pDefence),&DefValue)))
	{
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEdurusPotion::SetUp_ShaderResources()
{
	return S_OK;
}

CEdurusPotion* CEdurusPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CEdurusPotion* pInstance = New CEdurusPotion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CEdurusPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEdurusPotion::Clone(void* pArg)
{
	CEdurusPotion* pInstance = New CEdurusPotion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEdurusPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEdurusPotion::Free(void)
{
	__super::Free();

	Safe_Release(m_pCooltime);
	Safe_Release(m_pDefence);
}
