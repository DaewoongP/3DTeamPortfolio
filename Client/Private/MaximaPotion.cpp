#include "MaximaPotion.h"
#include"GameInstance.h"
#include"Player.h"
#include"MagicSlot.h"

CMaximaPotion::CMaximaPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPotion(pDevice,pContext)
{
}

CMaximaPotion::CMaximaPotion(const CMaximaPotion& rhs)
    : CPotion(rhs)
{
}

HRESULT CMaximaPotion::Initialize_Prototype(_uint iLevel)
{
    // 아이템 정보
    m_ItemCreateDesc.iCost = 300;											// 가격
    m_ItemCreateDesc.fDuration = 20.f;                                      //지속시간
    m_ItemCreateDesc.wstrKoreanName = TEXT("막시마 마법약");					// 한글명
    m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_Maxima.png"); // UI경로
    
    // 포션 정보
    m_PotionCreateDesc.Ingredients.push_back(INGREDIENT::LEECH_JUICE);	// 재료1
    m_PotionCreateDesc.Ingredients.push_back(INGREDIENT::SPIDER_FANG);	// 재료2
    m_PotionCreateDesc.fManufacturingTime = 30.f;							// 제조 시간
    m_PotionCreateDesc.wstrModelPath = TEXT("../../Resources/Models/NonAnims/SM_Maxima_Bottle/SM_Maxima_Bottle.dat"); // 모델경로

    if (FAILED(__super::Initialize_Prototype(iLevel)))
        return E_FAIL;
    return S_OK;
}

HRESULT CMaximaPotion::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CMaximaPotion::Use(_float3 vPlayPos)
{
    __super::Use(vPlayPos);

    m_pPlayer->Set_PowerUp(true);
}

void CMaximaPotion::Duration(_float fTimeDelta)
{
    m_fDuration += fTimeDelta;
    if (m_fDuration >= m_ItemCreateDesc.fDuration)
        m_pPlayer->Set_PowerUp(false);
}

CMaximaPotion* CMaximaPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CMaximaPotion* pInstance = New CMaximaPotion(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX("Failed to Created CMaximaPotion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMaximaPotion::Clone(void* pArg)
{
    CMaximaPotion* pInstance = New CMaximaPotion(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned CMaximaPotion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMaximaPotion::Free(void)
{
    __super::Free();
    
    Safe_Release(m_pMagicSlot);
}
