#include "..\Public\Potion_Station.h"
#include "GameInstance.h"
#include "Player.h"
#include "UI_Group_Brew.h"
#include "ParticleSystem.h"
#include "UI_Interaction.h"

CPotion_Station::CPotion_Station(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CPotion_Station::CPotion_Station(const CPotion_Station& rhs)
	: CMapObject(rhs)
	, m_iLevel(rhs.m_iLevel)
{
}

HRESULT CPotion_Station::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iLevel = iLevel;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Group_Brew")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel
			, TEXT("Prototype_GameObject_UI_Group_Brew")
			, CUI_Group_Brew::Create(m_pDevice, m_pContext))))
		{
			__debugbreak();
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_WaterSmoke")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_WaterSmoke")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/PotionStation/WaterSmoke/"), m_iLevel))))
		{
			__debugbreak();
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_PotFire")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_PotFire")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/PotionStation/PotFire/"), m_iLevel))))
		{
			__debugbreak();
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CPotion_Station::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CPotion_Station Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	m_pWaterSmoke->Disable();
	m_pPotFire->Disable();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(
		LEVEL_STATIC
		, TEXT("Layer_Player")
		, TEXT("GameObject_Player")));

	// 플레이어 주소에 대한 유효성 검사.
	if (nullptr == m_pPlayer)
	{
		__debugbreak();
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_AddRef(m_pPlayer);

	m_pPlayerTransform = m_pPlayer->Get_Transform();
	Safe_AddRef(m_pPlayerTransform);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CPotion_Station::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	switch (m_eState)
	{
	case Client::CPotion_Station::IDLE:
		// 거리가 가까우면 상호작용 가능해짐.
		if (fabs((m_pTransform->Get_Position() - m_pPlayerTransform->Get_Position()).Length()) < 2.f)
			m_eState = INTERACTABLE;
		break;
	case Client::CPotion_Station::INTERACTABLE:
		m_pUI_Interaction->Tick(fTimeDelta);

		// 거리가 멀면 상호작용 불가능해짐
		if (fabs((m_pTransform->Get_Position() - m_pPlayerTransform->Get_Position()).Length()) >= 2.f)
			m_eState = IDLE;

		// F버튼 누르면 활성화
		if (pGameInstance->Get_DIKeyState(DIK_F, CInput_Device::KEY_DOWN))
		{
			m_pCUI_Group_Brew->Set_isOpen(true);
			m_pWaterSmoke->Play(_float3(99.574f, 7.540f, 77.789f));
			m_pPotFire->Play(_float3(99.144f, 7.160f, 77.909f));
			pGameInstance->Set_Camera(TEXT("Potion_Station_Camera"), 0.5f);
			m_eState = SHOW;
		}
		break;
	case Client::CPotion_Station::SHOW:
		// E버튼 누르면 비활성화
		if (pGameInstance->Get_DIKeyState(DIK_F, CInput_Device::KEY_DOWN))
		{
			m_pCUI_Group_Brew->Set_isOpen(false);
			m_pWaterSmoke->Stop();
			m_pPotFire->Stop();
			pGameInstance->Set_Camera(TEXT("Player_Camera"), 1.f);
			m_eState = IDLE;
		}
		break;
	default:
		break;
	}

	Safe_Release(pGameInstance);

#ifdef _DEBUG
	ADD_IMGUI([&] { this->Tick_Imgui(fTimeDelta); });
#endif // _DEBUG
}

void CPotion_Station::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	switch (m_eState)
	{
	case Client::CPotion_Station::IDLE:
		break;
	case Client::CPotion_Station::INTERACTABLE:
		m_pUI_Interaction->Late_Tick(fTimeDelta);
		break;
	case Client::CPotion_Station::SHOW:
		break;
	case Client::CPotion_Station::STATE_END:
		break;
	default:
		break;
	}
}

#ifdef _DEBUG
void CPotion_Station::Tick_Imgui(_float fTimeDelta)
{
	CImGui_Manager::NextWindow_LeftBottom();
	CImGui_Manager::MatrixImgui(m_pPotFire->Get_Transform()->Get_WorldMatrixRef(), "Particle");
}
#endif // _DEBUG

HRESULT CPotion_Station::Add_Components()
{
	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_UI_Group_Brew")
		, TEXT("Com_UI_Group_Brew"), reinterpret_cast<CComponent**>(&m_pCUI_Group_Brew)));

	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_WaterSmoke")
		, TEXT("Com_Particle_WaterSmoke"), reinterpret_cast<CComponent**>(&m_pWaterSmoke)));

	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_PotFire")
		, TEXT("Com_Particle_PotFire"), reinterpret_cast<CComponent**>(&m_pPotFire)));


	/* Com_UI_Interaction */
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Interaction::INTERACTIONDESC pDesc;
	lstrcpy(pDesc.m_wszName, TEXT("포션 스테이션"));
	lstrcpy(pDesc.m_wszFunc, TEXT("제조하기"));
	pDesc.m_WorldMatrix = m_pTransform->Get_WorldMatrixPtr();

	m_pUI_Interaction = static_cast<CUI_Interaction*>(pGameInstance->Clone_Component(LEVEL_STATIC,
		TEXT("Prototype_GameObject_UI_Interaction"), &pDesc));

	if (m_pUI_Interaction == nullptr)
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

		Safe_Release(pGameInstance);

	return S_OK;
}

CPotion_Station* CPotion_Station::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CPotion_Station* pInstance = New CPotion_Station(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CPotion_Station");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPotion_Station::Clone(void* pArg)
{
	CPotion_Station* pInstance = New CPotion_Station(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPotion_Station");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPotion_Station::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		// DEELTE CAM 만들기ㅣ;ㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣㅣ
		Safe_Release(m_pPlayer);
		Safe_Release(m_pPlayerTransform);
		Safe_Release(m_pCUI_Group_Brew);
		Safe_Release(m_pWaterSmoke);
		Safe_Release(m_pPotFire);
	}

	Safe_Release(m_pUI_Interaction);

}
