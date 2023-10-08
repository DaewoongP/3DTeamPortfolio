#include "ThunderBrewPotion.h"
#include "GameInstance.h"
#include "Player.h"
#include "MagicSlot.h"

CThunderBrewPotion::CThunderBrewPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPotion(pDevice, pContext)
{
}

CThunderBrewPotion::CThunderBrewPotion(const CThunderBrewPotion& rhs)
    : CPotion(rhs)
{
}

HRESULT CThunderBrewPotion::Initialize_Prototype(_uint iLevel)
{
    // 포션 정보
    m_PotionCreateDesc.wstrModelPath = TEXT("../../Resources/Models/NonAnims/SM_Thunderbrew_Bottle/SM_Thunderbrew_Bottle.dat"); // 모델경로
    m_PotionCreateDesc.Ingredients.push_back(INGREDIENT::LEECH_JUICE);	// 재료1
    m_PotionCreateDesc.Ingredients.push_back(INGREDIENT::SPIDER_FANG);	// 재료2
    m_PotionCreateDesc.fManufacturingTime = 20.f;							// 제조 시간

    if (FAILED(__super::Initialize_Prototype(iLevel)))
        return E_FAIL;

    return S_OK;
}

HRESULT CThunderBrewPotion::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

CThunderBrewPotion* CThunderBrewPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CThunderBrewPotion* pInstance = New CThunderBrewPotion(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX("Failed to Created CThunderBrewPotion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CThunderBrewPotion::Clone(void* pArg)
{
    CThunderBrewPotion* pInstance = New CThunderBrewPotion(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned CThunderBrewPotion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CThunderBrewPotion::Free(void)
{
    __super::Free();
}
