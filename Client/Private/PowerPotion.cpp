#include "PowerPotion.h"

CPowerPotion::CPowerPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext):CItem(pDevice,pContext)
{
}

CPowerPotion::CPowerPotion(const CPowerPotion& rhs) : CItem(rhs)
{
}

HRESULT CPowerPotion::Initialize_Prototype(_uint iLevel, const _tchar* pUIImagePath)
{
    if (FAILED(__super::Initialize_Prototype(iLevel, pUIImagePath)))
        return E_FAIL;

    m_iLevel = iLevel;
    m_eItemType = ITEMTYPE::POTION;

    return S_OK;
}

HRESULT CPowerPotion::Initialize(void* pArg)
{

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CPowerPotion::Use(_float3 vPlayPos)
{
   

}

HRESULT CPowerPotion::Add_Components()
{
    if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Magic"),
        TEXT("Com_Magic"), reinterpret_cast<CComponent**>(&m_pMagic))))
    {
        __debugbreak();
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CPowerPotion::SetUp_ShaderResources()
{
    return S_OK;
}

CPowerPotion* CPowerPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, const _tchar* pUIImagePath)
{
    CPowerPotion* pInstance = New CPowerPotion(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(iLevel, pUIImagePath)))
    {
        MSG_BOX("Failed to Created CAccPotion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPowerPotion::Clone(void* pArg)
{
    CPowerPotion* pInstance = New CPowerPotion(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned CAccPotion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPowerPotion::Free(void)
{
    __super::Free();
    

}
