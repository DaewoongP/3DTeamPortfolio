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
	, m_fDuration(rhs.m_fDuration)
{
}

HRESULT CFocusPotion::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.iCost = 500;									// 가격
	m_ItemCreateDesc.wstrKoreanName = TEXT("집중력 물약");			// 한글명
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_AMFillPotion.png"); // UI경로

	// 포션 정보
	m_PotionCreateDesc.wstrModelPath = TEXT("../../Resources/Models/NonAnims/SM_SpherePrimitiveRegularNormals_01/SM_SpherePrimitiveRegularNormals_01.dat"); // 모델경로
	m_PotionCreateDesc.eModelType = CModel::TYPE_NONANIM; // 모델 타입
	m_PotionCreateDesc.PivotMatrix = _float4x4(); // 피벗 매트릭스
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

}

HRESULT CFocusPotion::Add_Components()
{
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
}