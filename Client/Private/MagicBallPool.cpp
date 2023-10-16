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
#include "Lumos.h"
#include "Arrestomomentum.h"
#include "Descendo.h"
#include "Accio.h"
#include "Flipendo.h"
#include "Expelliarmus.h"
#include "Imperio.h"
#include "Crucio.h"
#include "Stupefy.h"
#include "Diffindo.h"
#include "Bombarda.h"

IMPLEMENT_SINGLETON(CMagicBallPool)

HRESULT CMagicBallPool::Initialize()
{
    std::lock_guard<std::mutex> lock(mtx);

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    // default 10°³ »ý¼º
    Create_InitMagic(Client::BASICCAST, TEXT("Prototype_GameObject_BasicCast"), 30);
    Create_InitMagic(Client::PROTEGO, TEXT("Prototype_GameObject_Protego"));
    Create_InitMagic(Client::REVELIO, TEXT("Prototype_GameObject_Revelio"));
    Create_InitMagic(Client::WINGARDIUMLEVIOSA, TEXT("Prototype_GameObject_Wingardiumleviosa"));
    Create_InitMagic(Client::LEVIOSO, TEXT("Prototype_GameObject_Levioso"));
    Create_InitMagic(Client::CONFRINGO, TEXT("Prototype_GameObject_Confringo"));
    Create_InitMagic(Client::FINISHER, TEXT("Prototype_GameObject_Finisher"));
    Create_InitMagic(Client::NCENDIO, TEXT("Prototype_GameObject_Ncendio"));
    Create_InitMagic(Client::LUMOS, TEXT("Prototype_GameObject_Lumos"));
    Create_InitMagic(Client::ARRESTOMOMENTUM, TEXT("Prototype_GameObject_Arrestomomentum"));
    Create_InitMagic(Client::DESCENDO, TEXT("Prototype_GameObject_Descendo"));
    Create_InitMagic(Client::ACCIO, TEXT("Prototype_GameObject_Accio"));
    Create_InitMagic(Client::FLIPENDO, TEXT("Prototype_GameObject_Flipendo")); 
    Create_InitMagic(Client::EXPELLIARMUS, TEXT("Prototype_GameObject_Expelliarmus"));
    Create_InitMagic(Client::IMPERIO, TEXT("Prototype_GameObject_Imperio"));
    Create_InitMagic(Client::CRUCIO, TEXT("Prototype_GameObject_Crucio")); 
    Create_InitMagic(Client::STUPEFY, TEXT("Prototype_GameObject_Stupefy"));
    Create_InitMagic(Client::DIFFINDO, TEXT("Prototype_GameObject_Diffindo"));
    Create_InitMagic(Client::BOMBARDA, TEXT("Prototype_GameObject_Bombarda"));
    Create_InitMagic(Client::PENSIVE_GROUND_BALL, TEXT("Prototype_GameObject_Pensive_Ground_Ball"),3);
    Safe_Release(pGameInstance);

    return S_OK;
}

CMagicBall* CMagicBallPool::Get_Magic(CMagicBall::MAGICBALLINITDESC& MagicBallDesc)
{
    CMagicBall* pMagicball = { nullptr };

    if (m_MagicPool[MagicBallDesc.eMagicTag].empty())
    {
        pMagicball = Create_Magic(MagicBallDesc.eMagicTag);
    }
    else 
    {
        pMagicball = m_MagicPool[MagicBallDesc.eMagicTag].front();
        m_MagicPool[MagicBallDesc.eMagicTag].pop();
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
    m_MagicPool[eSpell].push(pMagic);
}

CMagicBall* CMagicBallPool::Create_Magic(SPELL eSpell)
{
    CMagicBall* pMagicBall = { nullptr };

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    switch (eSpell)
    {
    case Client::ACCIO:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Accio")));
        break;
    case Client::ALOHOMORA:
        break;
    case Client::ALTERATION:
        break;
    case Client::ARRESTOMOMENTUM:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Arrestomomentum")));
        break;
    case Client::AVADAKEDAVRA:
        break;
    case Client::BOMBARDA:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Bombarda")));
        break;
    case Client::CONFRINGO:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Confringo")));
        break;
    case Client::CONJURATION:
        break;
    case Client::CRUCIO:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Crucio")));
        break;
    case Client::DEPULSO:
        break;
    case Client::DESCENDO:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Descendo")));
        break;
    case Client::DIFFINDO:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Diffindo")));
        break;
    case Client::DISILLUSIONMENT:
        break;
    case Client::EVAESCO:
        break;
    case Client::EXPELLIARMUS:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Expelliarmus")));
        break;
    case Client::FLIPENDO:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Flipendo")));
        break;
    case Client::GLACIUS:
        break;
    case Client::IMPERIO:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Imperio")));
        break;
    case Client::NCENDIO:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Ncendio")));
        break;
    case Client::LEVIOSO:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Levioso")));
        break;
    case Client::LUMOS:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Lumos")));
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
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Stupefy")));
        break;
    case Client::PETRIFICUSTOTALUS:
        break;
    case Client::MAGICTHROW:
        break;
    case Client::FINISHER:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Finisher")));
        break;
    case Client::PROJECTILE_WHITE:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Bombarda")));
        break;
    case Client::PROJECTILE_BLACK:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Bombarda")));
        break;
    case Client::PENSIVE_GROUND_BALL:
        pMagicBall = static_cast<CMagicBall*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Pensive_Ground_Ball")));
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
		m_MagicPool[eTag].push(static_cast<CMagicBall*>(pMagic));
	}

    Safe_Release(pGameInstance);
}

void CMagicBallPool::Free()
{
    for (auto& MagicQueue : m_MagicPool)
    {
        while (!MagicQueue.empty())
        {
            Safe_Release(MagicQueue.front());
            MagicQueue.pop();
        }
    }
}

