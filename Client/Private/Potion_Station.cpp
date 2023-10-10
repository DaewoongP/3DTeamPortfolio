#include "..\Public\Potion_Station.h"
#include "GameInstance.h"
#include "Player.h"
#include "PotionStationCamera.h"
#include "UI_Group_Brew.h"
#include "ParticleSystem.h"

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

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_GreenBall")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_GreenBall")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/PotionStation/GreenBall/"), m_iLevel))))
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

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 모든 레벨을 조사해서 플레이어 주소를 가져옴.
	for (_uint i = 0; i < LEVEL_END; ++i)
	{
		m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(
			i
			, TEXT("Layer_Player")
			, TEXT("GameObject_Player")));

		if (m_pPlayer != nullptr)
			break;
	}

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

	//카메라 추가
	CCamera::CAMERADESC CameraDesc;

	CameraDesc.m_fAspect = _float(g_iWinSizeX) / _float(g_iWinSizeY);
	CameraDesc.m_fFovY = XMConvertToRadians(90.f);
	CameraDesc.m_fNear = 0.1f;
	CameraDesc.m_fFar = 1000.f;

	CPotionStationCamera::POTIONSTATION_CAMERA_DESC Potionstation_Camera_Desc;

	Potionstation_Camera_Desc.vAt = { 98.124f, 8.180f, 77.079f };
	Potionstation_Camera_Desc.pSuperDesc = CameraDesc;

	pGameInstance->Add_Camera(TEXT("Potion_Station_Camera"), CPotionStationCamera::Create(m_pDevice, m_pContext, &Potionstation_Camera_Desc));

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
		cout << "상호작용 UI" << '\n';

		// 거리가 멀면 상호작용 불가능해짐
		if (fabs((m_pTransform->Get_Position() - m_pPlayerTransform->Get_Position()).Length()) >= 2.f)
			m_eState = IDLE;

		// E버튼 누르면 활성화
		if (pGameInstance->Get_DIKeyState(DIK_E, CInput_Device::KEY_DOWN))
		{
			m_pCUI_Group_Brew->Set_isOpen(true);
			m_pParticleSystem->Play(_float3(99.574f, 7.540f, 77.789f));

			pGameInstance->Set_Camera(TEXT("Potion_Station_Camera"), 0.5f);
			m_eState = SHOW;
		}
		break;
	case Client::CPotion_Station::SHOW:
		// E버튼 누르면 비활성화
		if (pGameInstance->Get_DIKeyState(DIK_E, CInput_Device::KEY_DOWN))
		{
			m_pCUI_Group_Brew->Set_isOpen(false);
			m_pParticleSystem->Stop();

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
}

#ifdef _DEBUG
void CPotion_Station::Tick_Imgui(_float fTimeDelta)
{
	CImGui_Manager::NextWindow_LeftBottom();
	CImGui_Manager::MatrixImgui(m_pParticleSystem->Get_Transform()->Get_WorldMatrixRef(), "Particle");
}
#endif // _DEBUG

HRESULT CPotion_Station::Add_Components()
{
	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_UI_Group_Brew")
		, TEXT("Com_UI_Group_Brew"), reinterpret_cast<CComponent**>(&m_pCUI_Group_Brew)));

	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_GreenBall")
		, TEXT("Com_Particle_GreenBall"), reinterpret_cast<CComponent**>(&m_pParticleSystem)));

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
		Safe_Release(m_pPlayer);
		Safe_Release(m_pPlayerTransform);
		Safe_Release(m_pCUI_Group_Brew);
		Safe_Release(m_pParticleSystem);
	}
}
