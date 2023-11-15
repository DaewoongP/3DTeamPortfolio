#include "..\Public\Quest_Manager.h"
#include "GameInstance.h"
#include "Player.h"
#include "Card_Fig.h"

IMPLEMENT_SINGLETON(CQuest_Manager)

_bool CQuest_Manager::Is_Quest_Finished(const _tchar* szQuestTag)
{
	CQuest* pQuest = Find_Quest(szQuestTag);
	if (nullptr == pQuest)
		return false;

	return pQuest->Is_Finished();
}

HRESULT CQuest_Manager::Add_Quest(const _tchar* szQuestTag, class CQuest* pQuest)
{
	m_Quests.emplace(szQuestTag, pQuest);

	return S_OK;
}

void CQuest_Manager::Unlock_Quest(const _tchar* szQuestTag)
{
	CQuest* pQuest = Find_Quest(szQuestTag);

	QUESTSTATE state =pQuest->Get_State();

	if (nullptr == pQuest || QUESTSTATE_CLEAR == state )
		return;

	pQuest->Set_State(QUESTSTATE::QUESTSTATE_UNLOCK);
}

void CQuest_Manager::Clear_Quest(const _tchar* szQuestTag)
{
	CQuest* pQuest = Find_Quest(szQuestTag);
	if (nullptr == pQuest)
		return;

	pQuest->Clear_Quest();
	pQuest->Set_State(QUESTSTATE::QUESTSTATE_CLEAR);
}

void CQuest_Manager::Tick(_float fTimeDelta)
{
	Cheat_Quest();

	for (auto& Pair : m_Quests)
	{
		Pair.second->Tick(fTimeDelta);
	}

	if (isInit)
	{
		if (Cheat_Init_Quest() == false)
			return;
		isInit = false;
	}
}

void CQuest_Manager::Late_Tick(_float fTimeDelta)
{
}

_bool CQuest_Manager::Cheat_Init_Quest()
{
	_uint iSize = m_Quests.size();
	if (iSize != 5)
		return false;

	// Ŭ�����ϰ������ ����  �ּ��ż���
	BEGININSTANCE;

	/*Clear_Quest(TEXT("Quest_Save_Fig"));
	Unlock_Quest(TEXT("Quest_Potion"));

	Clear_Quest(TEXT("Quest_Potion"));
	Unlock_Quest(TEXT("Quest_Town"));
	CPlayer* pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("GameObject_Player")));
	if (pPlayer == nullptr)
	{
		ENDINSTANCE;
		return false;
	}

	static_cast<CCard_Fig*>(pPlayer->Find_Component(TEXT("Card_Fig")))->Set_ShowCard(true);

	Clear_Quest(TEXT("Quest_Town"));
	Unlock_Quest(TEXT("Quest_Secret"));

	Clear_Quest(TEXT("Quest_Secret"));
	Unlock_Quest(TEXT("Quest_Bone"));*/

	//Clear_Quest(TEXT("Quest_Bone"));

	ENDINSTANCE;

	return true;
}
void CQuest_Manager::Cheat_Quest()
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// ������� ��������
	// ���ϴ� ����Ʈ���� Ŭ���� �� �Ǳױ������� ����Ʈ �ް� �����ϱ�.
	if (pGameInstance->Get_DIKeyState(DIK_LMENU, CInput_Device::KEY_PRESSING))
	{
		// �Ǳ� �����ϱ� ����Ʈ �Ϸ�.
		if (pGameInstance->Get_DIKeyState(DIK_1, CInput_Device::KEY_DOWN))
		{
			// �Ǳ� ����
			Clear_Quest(TEXT("Quest_Save_Fig"));
			Unlock_Quest(TEXT("Quest_Potion"));
		}
		else if (pGameInstance->Get_DIKeyState(DIK_2, CInput_Device::KEY_DOWN))
		{
			//  ���� �� Ŭ����, Ŭ���� �Ŀ��� �κ��丮 �� ���������.
			Clear_Quest(TEXT("Quest_Potion"));
			Unlock_Quest(TEXT("Quest_Town"));

				// ī����� �� ����
				CPlayer* pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("GameObject_Player")));
			static_cast<CCard_Fig*>(pPlayer->Find_Component(TEXT("Card_Fig")))->Set_ShowCard(true);
		}
		else if (pGameInstance->Get_DIKeyState(DIK_3, CInput_Device::KEY_DOWN))
		{
			// Ʈ����°� Ŭ����
			Clear_Quest(TEXT("Quest_Town"));
			Unlock_Quest(TEXT("Quest_Secret"));

		}
		else if (pGameInstance->Get_DIKeyState(DIK_4, CInput_Device::KEY_DOWN))
		{
			// ȸ�� Ŭ
			Clear_Quest(TEXT("Quest_Secret"));
			Unlock_Quest(TEXT("Quest_Bone"));

		}
		else if (pGameInstance->Get_DIKeyState(DIK_5, CInput_Device::KEY_DOWN))
		{
			Clear_Quest(TEXT("Quest_Bone"));
		}

	}

	Safe_Release(pGameInstance);

}

CQuest* CQuest_Manager::Find_Quest(const _tchar* szQuestTag)
{
	std::lock_guard<std::mutex> lock(mtx);
	auto Pair = find_if(m_Quests.begin(), m_Quests.end(), CTag_Finder(szQuestTag));

	if (m_Quests.end() == Pair)
		return nullptr;

	return Pair->second;
}

void CQuest_Manager::Free()
{
	for (auto& Pair : m_Quests)
	{
		Safe_Release(Pair.second);
	}

	m_Quests.clear();
}
