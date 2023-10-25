#include "FlyGameManager.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Racer.h"

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
	m_isGameContinue = false;


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

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Racer"),
			m_pRacerOwnerGroup[i]->Get_Tag(), reinterpret_cast<CComponent**>(&m_pRacerGroup[i]),&RacerInitDesc)))
		{
			MSG_BOX("Failed CFlyGameManager Add_Component : Racer");
			__debugbreak();
			return E_FAIL;
		}
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
	CComposite::Free();

	if (true == m_isCloned)
	{
		for (_uint i = 0; i < m_pRacerGroup.size(); i++)
		{
			Safe_Release(m_pRacerGroup[i]);
			Safe_Release(m_pRacerOwnerGroup[i]);
		}
	}
}
