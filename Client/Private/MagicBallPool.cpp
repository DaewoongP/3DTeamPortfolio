#include "MagicBallPool.h"
#include "GameInstance.h"

#include "BasicCast.h"
#include "Protego.h"
#include "Revelio.h"
#include "Levioso.h"
#include "Confringo.h"
#include "Finisher.h"
#include "Wingardiumleviosa.h"
#include "Ncendio.h"

IMPLEMENT_SINGLETON(CMagicBallPool)

HRESULT CMagicBallPool::Initialize()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    // default 10°³ »ý¼º
    Create_InitMagic(Client::BASICCAST, TEXT("Prototype_GameObject_BasicCast"), 3);
    Create_InitMagic(Client::PROTEGO, TEXT("Prototype_GameObject_Protego"));
    Create_InitMagic(Client::REVELIO, TEXT("Prototype_GameObject_Revelio"));
    Create_InitMagic(Client::WINGARDIUMLEVIOSA, TEXT("Prototype_GameObject_Wingardiumleviosa"));
    Create_InitMagic(Client::LEVIOSO, TEXT("Prototype_GameObject_Levioso"));
    Create_InitMagic(Client::CONFRINGO, TEXT("Prototype_GameObject_Confringo"));
    Create_InitMagic(Client::FINISHER, TEXT("Prototype_GameObject_Finisher"));
    Create_InitMagic(Client::NCENDIO, TEXT("Prototype_GameObject_Ncendio"));

    Safe_Release(pGameInstance);

    return S_OK;
}

CMagicBall* CMagicBallPool::Get_Magic(CMagicBall::MAGICBALLINITDESC& MagicBallDesc)
{
    CMagicBall* pMagicball = { nullptr };

    if (m_MagicPoolVec[MagicBallDesc.eMagicTag].empty())
    {
        pMagicball = Create_Magic(MagicBallDesc.eMagicTag, MagicBallDesc);
    }
    else 
    {
        pMagicball = m_MagicPoolVec[MagicBallDesc.eMagicTag].front();
        m_MagicPoolVec[MagicBallDesc.eMagicTag].pop();
    }

    if (FAILED(pMagicball->Reset(MagicBallDesc)))
    {
        MSG_BOX("Failed Set MagicBall Init Setting");
        return nullptr;
    }

    return pMagicball;
}

void CMagicBallPool::Return_Magic(CMagicBall* pMagic, SPELL eSpell)
{
    Safe_AddRef(pMagic);
    m_MagicPoolVec[eSpell].push(pMagic);
}

CMagicBall* CMagicBallPool::Create_Magic(SPELL eSpell, CMagicBall::MAGICBALLINITDESC& MagicBallDesc)
{
    CMagicBall* pMagicBall = { nullptr };

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    switch (eSpell)
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
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Ncendio")));
        break;
    case Client::LEVIOSO:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Levioso")));
        break;
    case Client::LUMOS:
        break;
    case Client::PROTEGO:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Protego")));
        break;
    case Client::REPARO:
        break;
    case Client::REVELIO:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Revelio")));
        break;
    case Client::WINGARDIUMLEVIOSA:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Wingardiumleviosa")));
        break;
    case Client::BASICCAST:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BasicCast")));
        break;
    case Client::STUPEFY:
        break;
    case Client::PETRIFICUSTOTALUS:
        break;
    case Client::MAGICTHROW:
        break;
    case Client::FINISHER:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Finisher")));
        break;
    case Client::SPELL_END:
        MSG_BOX("Invalid Tag Value");
        break;
    default:
        MSG_BOX("Invalid Tag Value");
        break;
    }

    Safe_Release(pGameInstance);

    return pMagicBall;
}

void CMagicBallPool::Create_InitMagic(SPELL eTag, const _tchar* szTagName, _uint iNumPool)
{
    CComponent* pMagic = { nullptr };

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

	for (_uint i = 0; i < iNumPool; ++i)
	{
		pMagic = pGameInstance->Clone_Component(LEVEL_STATIC, szTagName);
		m_MagicPoolVec[eTag].push(static_cast<CMagicBall*>(pMagic));
	}

    Safe_Release(pGameInstance);
}

void CMagicBallPool::Free()
{
    for (auto& MagicQueue : m_MagicPoolVec)
    {
        while (!MagicQueue.empty())
        {
            Safe_Release(MagicQueue.front());
            MagicQueue.pop();
        }
    }
}

