#include "..\Public\Event_Vault_Torch.h"
#include "GameInstance.h"

#include "Vault_Torch.h"
#include "Trigger.h"

CEvent_Vault_Torch::CEvent_Vault_Torch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Vault_Torch::CEvent_Vault_Torch(const CEvent_Vault_Torch& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Vault_Torch::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);
	
	std::lock_guard<std::mutex> lock(mtx);

	BEGININSTANCE;
	auto pMapObjectLayer = pGameInstance->Find_Components_In_Layer(LEVEL_VAULT, TEXT("Layer_BackGround"));
	ENDINSTANCE;

	for (auto Pair : *pMapObjectLayer)
	{
		wstring wstrObjTag = Pair.first;
		// Vault_Torch를 찾는다.
		if (wstring::npos != wstrObjTag.find(TEXT("Vault_Torch")))
		{
			// 토치 인덱스 추출
			size_t findIndex = wstrObjTag.find(TEXT("Torch_")) + 6;

			wstring wsModelIndex = wstrObjTag.substr(findIndex);
			
			// 0 ~ 11번 횃불
			if (0 <= stoi(wsModelIndex) && 11 >= stoi(wsModelIndex))
			{
				m_pTorchs_1.emplace(wstrObjTag, static_cast<CVault_Torch*>(Pair.second));
				Safe_AddRef(Pair.second);
			}

			// 12 ~ 23번 횃불
			else if (12 <= stoi(wsModelIndex) && 23 >= stoi(wsModelIndex))
			{
				m_pTorchs_2.emplace(wstrObjTag, static_cast<CVault_Torch*>(Pair.second));
				Safe_AddRef(Pair.second);
			}
		}
	}

	return S_OK;
}

void CEvent_Vault_Torch::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Check_Event_On_1(fTimeDelta);
	Check_Event_On_2(fTimeDelta);
}

void CEvent_Vault_Torch::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CEvent_Vault_Torch::Check_Event_On_1(_float fTimeDelta)
{
	if (true == m_isOn_1)
		return;

	if (true == m_pTorch_Stage_1->Is_Collision())
	{
		m_fStage1Time += fTimeDelta;

		_uint iTorchIndex = (_uint)(m_fStage1Time * 10.f);

		// 0.2 초마다 불이 켜짐
		if (0 == (iTorchIndex % 2))
		{
			for (auto iter = m_pTorchs_1.begin(); iter != m_pTorchs_1.end();)
			{
				if (iTorchIndex == iter->second->Get_TorchIndex() || 
					iTorchIndex + 1 == iter->second->Get_TorchIndex())
				{
					// 앞쪽부터 횃불 두 개를 켠다.
					iter->second->Set_TorchOn(true);
					Safe_Release(iter->second);
					iter = m_pTorchs_1.erase(iter);
				}

				else
					++iter;
			}

			if (m_pTorch_Stage_1->isDead() &&
				0 == m_pTorchs_1.size())
				m_isOn_1 = true;
		}
	}
}

void CEvent_Vault_Torch::Check_Event_On_2(_float fTimeDelta)
{
	if (true == m_isOn_2)
		return;

	if (true == m_pTorch_Stage_2->Is_Collision())
	{
		m_fStage2Time += fTimeDelta;

		// 두번째 횃불의 인덱스는 12부터 시작한다.
		_uint iTorchIndex = (_uint)(m_fStage2Time * 10.f) + 12;

		// 0.2 초마다 불이 켜짐
		if (0 == (iTorchIndex % 2))
		{
			for (auto iter = m_pTorchs_2.begin(); iter != m_pTorchs_2.end();)
			{
				if (iTorchIndex == iter->second->Get_TorchIndex() ||
					iTorchIndex + 1 == iter->second->Get_TorchIndex())
				{
					// 앞쪽부터 횃불 두 개를 켠다.
					iter->second->Set_TorchOn(true);
					Safe_Release(iter->second);
					iter = m_pTorchs_2.erase(iter);
				}

				else
					++iter;
			}

			if (m_pTorch_Stage_2->isDead() &&
				0 == m_pTorchs_2.size())
				m_isOn_2 = true;
		}
	}
}

HRESULT CEvent_Vault_Torch::Add_Components()
{
	/* For.Trigger_Spawn_1 */
	CTrigger::TRIGGERDESC TriggerDesc;
	TriggerDesc.isCollisionToDead = true;
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Vault_Torch_On_1");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = _float3(3.5f, 2.5f, 3.5f);
	TriggerDesc.vTriggerWorldPos = _float3(38.1f, -1.25f, 38.1f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_Torch_On_1"), reinterpret_cast<CComponent**>(&m_pTorch_Stage_1), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Vault_Torch Failed Add_Components : Trigger_Torch_On_1");
		return E_FAIL;
	}

	TriggerDesc.isCollisionToDead = true;
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Vault_Torch_On_2");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = _float3(3.5f, 2.5f, 3.5f);
	TriggerDesc.vTriggerWorldPos = _float3(86.4f, -1.25f, 86.4f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_Torch_On_2"), reinterpret_cast<CComponent**>(&m_pTorch_Stage_2), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Vault_Torch Failed Add_Components : Trigger_Torch_On_2");
		return E_FAIL;
	}

	return S_OK;
}

CEvent_Vault_Torch* CEvent_Vault_Torch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Vault_Torch* pInstance = New CEvent_Vault_Torch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Vault_Torch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEvent_Vault_Torch* CEvent_Vault_Torch::Clone(void* pArg)
{
	CEvent_Vault_Torch* pInstance = New CEvent_Vault_Torch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Vault_Torch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Vault_Torch::Free()
{
	__super::Free();

	Safe_Release(m_pTorch_Stage_1);
	Safe_Release(m_pTorch_Stage_2);

	for (auto& Pair : m_pTorchs_1)
		Safe_Release(Pair.second);

	for (auto& Pair : m_pTorchs_2)
		Safe_Release(Pair.second);
}
