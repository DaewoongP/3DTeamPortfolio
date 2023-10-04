#include "WiggenweldPotion.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"

CWiggenweldPotion::CWiggenweldPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPotion(pDevice, pContext)
{
}

CWiggenweldPotion::CWiggenweldPotion(const CWiggenweldPotion& rhs)
	: CPotion(rhs)
	, m_fRecoveryAmount(rhs.m_fRecoveryAmount)
{
}

HRESULT CWiggenweldPotion::Initialize_Prototype(_uint iLevel)
{
	// ������ ����
	m_ItemCreateDesc.iCost = 100;											// ����
	m_ItemCreateDesc.wstrKoreanName = TEXT("�������� ����");					// �ѱ۸�
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_WoundCleaning.png"); // UI���
	
	// ���� ����
	m_PotionCreateDesc.wstrModelPath = TEXT("../../Resources/Models/NonAnims/SM_Health_Bottle/SM_Health_Bottle.dat"); // �𵨰��
	m_PotionCreateDesc.Ingredients.push_back(INGREDIENT::HORKLUMP_JUICE);	// ���1
	m_PotionCreateDesc.Ingredients.push_back(INGREDIENT::DITTANY_LEAVES);	// ���2
	m_PotionCreateDesc.fManufacturingTime = 15.f;							// ���� �ð�
	
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWiggenweldPotion::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CWiggenweldPotion::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWiggenweldPotion::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWiggenweldPotion::Render()
{
	return __super::Render();
}

void CWiggenweldPotion::Use(_float3 vPlayPos)
{
	m_pPlayerInformation->Get_Health()->Heal(m_fRecoveryAmount);
}

CWiggenweldPotion* CWiggenweldPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CWiggenweldPotion* pInstance = New CWiggenweldPotion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CWiggenweldPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWiggenweldPotion::Clone(void* pArg)
{
	CWiggenweldPotion* pInstance = New CWiggenweldPotion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CWiggenweldPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWiggenweldPotion::Free()
{
	__super::Free();
}