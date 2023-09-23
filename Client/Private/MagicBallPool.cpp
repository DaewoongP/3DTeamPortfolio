#include "MagicBallPool.h"
#include "GameInstance.h"

#include "BasicCast.h"
#include "Protego.h"
#include "Revelio.h"

CMagicBallPool::CMagicBallPool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CComponent(pDevice, pContext)
{
}

HRESULT CMagicBallPool::Initialize_Prototype()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    Create_InitMagic(pGameInstance, BASICCAST, TEXT("Prototype_GameObject_BaseAttack"));
    Create_InitMagic(pGameInstance, PROTEGO, TEXT("Prototype_GameObject_Protego"));
    Create_InitMagic(pGameInstance, REVELIO, TEXT("Prototype_GameObject_Revelio"));

    Safe_Release(pGameInstance);
    return S_OK;
}

CMagicBall* CMagicBallPool::GetMagic(SPELL tag)
{
    if (m_MagicPoolVec[tag].empty()) 
    {
        return Create_Magic(tag);
    }
    else 
    {
        CMagicBall* magicball = m_MagicPoolVec[tag].back();
        m_MagicPoolVec[tag].pop_back();
        return magicball;
    }
}

void CMagicBallPool::ReturnMagic(CMagicBall* magic, SPELL tag)
{
    m_MagicPoolVec[tag].push_back(magic);
}

CMagicBall* CMagicBallPool::Create_Magic(SPELL tag)
{
    CMagicBall* magicBall = nullptr;
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);
    switch (tag)
    {
    case Client::ACCIO:
        break;
    case Client::ALOHOMORA:
        break;
    case Client::ALTERATION:
        break;
    case Client::ARRESTOMOMENTUM:
        break;
    case Client::AVADAKEDAVRA:
        break;
    case Client::BOMBARDA:
        break;
    case Client::CONFRINGO:
        break;
    case Client::CONJURATION:
        break;
    case Client::CRUCIO:
        break;
    case Client::DEPULSO:
        break;
    case Client::DESCENDO:
        break;
    case Client::DIFFINDO:
        break;
    case Client::DISILLUSIONMENT:
        break;
    case Client::EVAESCO:
        break;
    case Client::EXPELLIARMUS:
        break;
    case Client::FLIPENDO:
        break;
    case Client::GLACIUS:
        break;
    case Client::IMPERIO:
        break;
    case Client::NCENDIO:
        break;
    case Client::LEVIOSO:
        break;
    case Client::LUMOS:
        break;
    case Client::PROTEGO:
        magicBall = dynamic_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_Revelio")));
        break;
    case Client::REPARO:
        break;
    case Client::REVELIO:
        magicBall = dynamic_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_Protego")));
        break;
    case Client::WINGARDIUMLEVIOSA:
        break;
    case Client::BASICCAST:
        magicBall = dynamic_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_BaseAttack")));
        break;
    case Client::STUPEFY:
        break;
    case Client::PETRIFICUSTOTALUS:
        break;
    case Client::MAGICTHROW:
        break;
    case Client::FINISHER:
        break;
    case Client::SPELL_END:
        break;
    default:
        break;
    }
    Safe_Release(pGameInstance);
    return magicBall;
}

void CMagicBallPool::Create_InitMagic(CGameInstance* pGameInstance, SPELL tag,const _tchar* tagName)
{
    CComponent* magic = nullptr;
    for (_uint i = 0; i < 10; i++)
    {
        magic = pGameInstance->Clone_Component(LEVEL_CLIFFSIDE, tagName);
        m_MagicPoolVec[tag].push_back(dynamic_cast<CMagicBall*>(magic));
    }
    return;
}


CMagicBallPool* CMagicBallPool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMagicBallPool* pInstance = New CMagicBallPool(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created CMagicBall");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CMagicBallPool::Clone(void* pArg)
{
    AddRef();
    return this;
}

void CMagicBallPool::Free()
{
    for (auto magicGroup : m_MagicPoolVec)
    {
        for (auto magic : magicGroup)
        {
            Safe_Release(magic);
        }
    }
}

