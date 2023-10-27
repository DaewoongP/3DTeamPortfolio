#include "FlyGameManager.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Racer.h"
#include "Balloon.h"
#include "UI_Group_Timer.h"
#include "UI_Group_Score.h"

CFlyGameManager::CFlyGameManager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComposite(_pDevice, _pContext)
{
}

CFlyGameManager::CFlyGameManager(const CFlyGameManager& rhs)
	: CComposite(rhs)
{
}

void CFlyGameManager::Add_Score(_uint iNumber, _uint iScore)
{
	m_pScoreGroup[iNumber] += iScore;
}

void CFlyGameManager::ReplaceBallon()
{
	_uint iBalloonIndex = rand() % m_pAllBalloonGroup.size();
	while (true)
	{
		if (m_pAllBalloonGroup[iBalloonIndex]->isDead())
		{
			CBalloon::BALLOONINITDESC initDesc = {};
			ZEROMEM(&initDesc);
			initDesc.iScore = (rand() % 5) +1;
			_float fTempscale = Random_Generator(0.5f, 2.f);
			initDesc.vScale = _float3(fTempscale, fTempscale, fTempscale);
			initDesc.fForce = GetRandomFloat(40.f, 60.f);
			_float3 vTempPosition = {};
			//근처에 레이서가 없도록 해줍니다.
			_bool isFound = { true };

			while (isFound)
			{
				isFound = false;
				for (_uint i = 0; i < m_pRacerGroup.size(); i++)
				{
					vTempPosition = _float3(GetRandomFloat(77.f, 153.f), GetRandomFloat(10.f, 30.f), GetRandomFloat(74.f, 246.f));
					if (_float3(vTempPosition - m_pRacerGroup[i]->Get_Transform()->Get_Position()).Length() < 10.f)
					{
						isFound = true;
						break;
					}
				}
			}

			initDesc.vPosition = vTempPosition;
			m_pAllBalloonGroup[iBalloonIndex]->ResetBallon(initDesc);
			break;
		}
		else
		{
			iBalloonIndex = rand() % m_pAllBalloonGroup.size();
		}
	}
}


void CFlyGameManager::Racer_AddForce(_uint iNumber, _uint eType, _float fForce)
{
	_float3 vToward = {};
	switch (eType)
	{
	case Client::CBalloon::TYPE_UP:
		vToward = _float3(0, 1, 0);
		break;
	case Client::CBalloon::TYPE_DOWN:
		vToward = _float3(0, -1, 0);
		break;
	case Client::CBalloon::TYPE_RIGHT:
		vToward = _float3(1, 0, 0);
		break;
	case Client::CBalloon::TYPE_LEFT:
		vToward = _float3(-1, 0, 0);
		break;
	case Client::CBalloon::TYPE_FORWARD:
		vToward = _float3(0, 0, 1);
		break;
	case Client::CBalloon::TYPE_BACKWARD:
		vToward = _float3(0, 0, -1);
		break;
	case Client::CBalloon::TYPE_RANDOM:
		break;
		vToward = _float3(Random_Generator(-1.f, 1.f), Random_Generator(-1.f, 1.f), Random_Generator(-1.f, 1.f));
		vToward.Normalize();
	default:
		return;
	}
	static_cast<CRigidBody*>(m_pRacerOwnerGroup[iNumber]->Find_Component(TEXT("Com_RigidBody")))->Add_Force_OtherCall(vToward * fForce);
}

HRESULT CFlyGameManager::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFlyGameManager::Initialize(void* pArg)
{
	//오브제를 찾아옵니다.
	Find_Racer_Parent(LEVEL_SKY,TEXT("Layer_Monster"));
	Find_Racer_Parent(LEVEL_STATIC,TEXT("Layer_Player"));

	//설정될 레이서만큼 0짜리 사이즈를 만들어줍니다.
	m_pScoreGroup.resize(m_pRacerOwnerGroup.size());
	
	//여기서 레이서를 만들어줍니다.
	//풍선도 만들어줄건데 잠깐 꺼두겟음
	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Add_Components : CFlyGameManager");
		return E_FAIL;
	}

	//게임 타이머를 설정해줍니다.
	m_fGameTimer = 180.f;
	m_isGameContinue = true;

	_uint iIndex = (_uint)(m_pAllBalloonGroup.size()/2);

	CBalloon::BALLOONINITDESC initDesc = {};
	for (_uint i = 0; i < iIndex; i++)
	{
		ReplaceBallon();
	}

	return S_OK;
}

void CFlyGameManager::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (m_isGameContinue)
	{
		m_fGameTimer -= fTimeDelta;
		if (m_fGameTimer < 0)
		{
			m_isGameContinue = false;
		}
	}
}

void CFlyGameManager::Late_Tick(_float fTimeDelta)
{
	if (m_isGameContinue)
	{
		for (_uint i = 0; i < m_pRacerGroup.size(); i++)
		{
			m_pRacerGroup[i]->Get_Transform()->Set_Position(m_pRacerOwnerGroup[i]->Get_Transform()->Get_Position());
		}
	}
	__super::Late_Tick(fTimeDelta);
}

HRESULT CFlyGameManager::Add_Components()
{
	m_pRacerGroup.resize(m_pRacerOwnerGroup.size());
	
	CRacer::RACERINITDESC RacerInitDesc = {};

	for (_int i = 0; i < m_pRacerOwnerGroup.size(); i++)
	{
		RacerInitDesc.iRacerNumber = i;
		if(i!= m_pRacerOwnerGroup.size() - 1)
			RacerInitDesc.eRacerType = CRacer::RACER_ENEMY;
		else
			RacerInitDesc.eRacerType = CRacer::RACER_PLAYER;

		if (FAILED(CComposite::Add_Component(LEVEL_SKY, TEXT("Prototype_GameObject_Racer"),
			m_pRacerOwnerGroup[i]->Get_Tag(), reinterpret_cast<CComponent**>(&m_pRacerGroup[i]),&RacerInitDesc)))
		{
			MSG_BOX("Failed CFlyGameManager Add_Component : Racers");
			__debugbreak();
			return E_FAIL;
		}
	}

	m_pAllBalloonGroup.resize(100);
	m_wszBalloonComponentTag.resize(m_pAllBalloonGroup.size());
	for (_int i = 0; i < m_wszBalloonComponentTag.size(); i++)
	{
		m_wszBalloonComponentTag[i] = TEXT("Com_Balloon_");
		m_wszBalloonComponentTag[i] += i;
	}

	for (_int i = 0; i < m_pAllBalloonGroup.size(); i++)
	{
		_tchar wszComProtoTag[MAX_PATH] = {};
		_uint iDevideby5 = m_pAllBalloonGroup.size() / 5;
		_uint iIndex = _int(i / iDevideby5);
		switch (iIndex)
		{
		case 0:
			lstrcpy(wszComProtoTag,TEXT("Prototype_GameObject_Balloon"));
			break;
		case 1:
			lstrcpy(wszComProtoTag, TEXT("Prototype_GameObject_BeastBalloon_A"));
			break;
		case 2:
			lstrcpy(wszComProtoTag, TEXT("Prototype_GameObject_BeastBalloon_B"));
			break;
		case 3:
			lstrcpy(wszComProtoTag, TEXT("Prototype_GameObject_BeastBalloon_C"));
			break;
		default:
			lstrcpy(wszComProtoTag, TEXT("Prototype_GameObject_BeastBalloon_D"));
			break;
		}
		CBalloon::BALLOONINITDESC initDesc = {};
		initDesc.vScale = _float3(Random_Generator(1.f,3.f), Random_Generator(1.f, 3.f), Random_Generator(1.f, 3.f));
		initDesc.vPosition = _float3(1000, 1000, 1000);

		if (FAILED(CComposite::Add_Component(LEVEL_SKY, wszComProtoTag,
			m_wszBalloonComponentTag[i].c_str(), reinterpret_cast<CComponent**>(&m_pAllBalloonGroup[i]), &initDesc)))
		{
			MSG_BOX("Failed CFlyGameManager Add_Component : Balloons");
			__debugbreak();
			return E_FAIL;
		}
	}

	CUI_Group_Score::UISCOREDESC ScoreDesc;
	ZEROMEM(&ScoreDesc);
	lstrcpy(ScoreDesc.wszFirstName, TEXT("포항김씨"));
	lstrcpy(ScoreDesc.wszSecondName, TEXT("박정환"));
	lstrcpy(ScoreDesc.wszThirdName, TEXT("심정환"));
	lstrcpy(ScoreDesc.wszFourthName, TEXT("안철민"));
	lstrcpy(ScoreDesc.wszFifthName, TEXT("장현우"));
	lstrcpy(ScoreDesc.wszSixthName, TEXT("전대인"));
	lstrcpy(ScoreDesc.wszSeventhName, TEXT("전윤혁"));
	lstrcpy(ScoreDesc.wszEighthName, TEXT("주성환"));
	lstrcpy(ScoreDesc.wszNinthName, TEXT("박대웅"));
	ScoreDesc.pScore = &m_pScoreGroup;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Score"),
		TEXT("Com_UI_Score"), reinterpret_cast<CComponent**>(&m_pUiScore), &ScoreDesc)))
	{
		MSG_BOX("Failed CFlyGameManager Add_Component : Com_UI_Score");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Timer"),
		TEXT("Com_UI_Timer"), reinterpret_cast<CComponent**>(&m_pUiTimer), &m_fGameTimer)))
	{
		MSG_BOX("Failed CFlyGameManager Add_Component : Com_UI_Timer");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

void CFlyGameManager::Find_Racer_Parent(_uint iLevel, const _tchar* wszLayerTag)
{
	BEGININSTANCE;
	unordered_map<const _tchar*, CComponent*>* pLayer = pGameInstance->Find_Components_In_Layer(iLevel, wszLayerTag);
	if (nullptr == pLayer)
	{
		ENDINSTANCE;
		return;
	}
	for (unordered_map<const _tchar*, CComponent*>::iterator iter = pLayer->begin(); iter != pLayer->end(); iter++)
	{
		CGameObject* pOwner = static_cast<CGameObject*>((*iter).second);
		m_pRacerOwnerGroup.push_back(pOwner);
		Safe_AddRef(pOwner);
	}
	ENDINSTANCE;
}

CFlyGameManager* CFlyGameManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFlyGameManager* pInstance = New CFlyGameManager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CFlyGameManager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CFlyGameManager::Clone(void* pArg)
{
	CFlyGameManager* pInstance = New CFlyGameManager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CFlyGameManager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlyGameManager::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		for (_uint i = 0; i < m_pRacerOwnerGroup.size(); i++)
		{
			Safe_Release(m_pRacerOwnerGroup[i]);
		}

		for (_uint i = 0; i < m_pRacerGroup.size(); i++)
		{
			Safe_Release(m_pRacerGroup[i]);
		}
		for (_uint i = 0; i < m_pAllBalloonGroup.size(); i++)
		{
			Safe_Release(m_pAllBalloonGroup[i]);
		}

		Safe_Release(m_pUiScore);
		Safe_Release(m_pUiTimer);
	}
}
