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

	BEGININSTANCE;
	auto pMapObjectLayer = pGameInstance->Find_Components_In_Layer(LEVEL_VAULT, TEXT("Layer_BackGround"));
	ENDINSTANCE;

	for (auto Pair : *pMapObjectLayer)
	{
		wstring wstrObjTag = Pair.first;
		// Vault_Torch를 찾는다.
		if (wstring::npos != wstrObjTag.find(TEXT("Vault_Torch")))
		{
			m_pTorchs.emplace(wstrObjTag, static_cast<CVault_Torch*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
	}

	return S_OK;
}

void CEvent_Vault_Torch::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Check_Event_On_1();
	Check_Event_On_2();
}

void CEvent_Vault_Torch::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CEvent_Vault_Torch::Check_Event_On_1()
{
	if (true == m_isOn_1)
		return;
}

void CEvent_Vault_Torch::Check_Event_On_2()
{
	if (true == m_isOn_1)
		return;
}

HRESULT CEvent_Vault_Torch::Add_Components()
{
	/* For.Trigger_Spawn_1 */
	CTrigger::TRIGGERDESC TriggerDesc;
	TriggerDesc.isCollisionToDead = true;
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Vault_Torch_On_1");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = _float3(3.5f, 2.5f, 3.5f);
	TriggerDesc.vTriggerWorldPos = _float3(29.7f, -1.25f, 30.3f);

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
	TriggerDesc.vTriggerWorldPos = _float3(82.1, -1.25f, 81.2f);

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
	for (auto& Pair : m_pTorchs)
		Safe_Release(Pair.second);
}
