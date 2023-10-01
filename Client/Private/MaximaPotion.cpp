#include "MaximaPotion.h"
#include"GameInstance.h"
#include"Player.h"
#include"Player_Information.h"  
#include"CoolTime.h"
#include"MagicSlot.h"

CMaximaPotion::CMaximaPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext):CItem(pDevice,pContext)
{
}

CMaximaPotion::CMaximaPotion(const CMaximaPotion& rhs) : CItem(rhs)
{
}

HRESULT CMaximaPotion::Initialize_Prototype(_uint iLevel, const _tchar* pUIImagePath)
{
    if (FAILED(__super::Initialize_Prototype(iLevel, pUIImagePath)))
        return E_FAIL;

    m_iLevel = iLevel;
    m_eItemType = ITEMTYPE::POTION;

    return S_OK;
}

HRESULT CMaximaPotion::Initialize(void* pArg)
{

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CMaximaPotion::Use(_float3 vPlayPos)
{
   // m_pCooltime->Set_Enable(true);
    

}

HRESULT CMaximaPotion::Add_Components()
{
    CCoolTime::COOLTIME_DESC MaxCool;
    MaxCool.fMaxCoolTime = 20.f;

    if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CoolTime"),
        TEXT("Com_CoolTime"), reinterpret_cast<CComponent**>(&m_pCooltime),&MaxCool)))
    {
        __debugbreak();
        return E_FAIL;
    }

    if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Player"),
        TEXT("Com_Player"), reinterpret_cast<CComponent**>(&m_pMagicSlot))))
    {
        __debugbreak();
        return E_FAIL;
    }


    return S_OK;
}

HRESULT CMaximaPotion::SetUp_ShaderResources()
{
    return S_OK;
}

CMaximaPotion* CMaximaPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, const _tchar* pUIImagePath)
{
    CMaximaPotion* pInstance = New CMaximaPotion(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(iLevel, pUIImagePath)))
    {
        MSG_BOX("Failed to Created CAccPotion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMaximaPotion::Clone(void* pArg)
{
    CMaximaPotion* pInstance = New CMaximaPotion(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned CAccPotion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMaximaPotion::Free(void)
{
    __super::Free();
    
    Safe_Release(m_pCooltime);
    Safe_Release(m_pMagicSlot);


}
