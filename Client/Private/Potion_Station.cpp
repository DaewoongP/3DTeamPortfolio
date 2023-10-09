#include "..\Public\Potion_Station.h"
#include "GameInstance.h"
#include "Player.h"
#include "PotionStationCamera.h"

CPotion_Station::CPotion_Station(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CPotion_Station::CPotion_Station(const CPotion_Station& rhs)
	: CMapObject(rhs)
{
}

HRESULT CPotion_Station::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

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

	Potionstation_Camera_Desc.vAt = {98.124, 8.180, 77.079 };
	Potionstation_Camera_Desc.pSuperDesc = CameraDesc;

	pGameInstance->Add_Camera(TEXT("Potion_Station_Camera"), CPotionStationCamera::Create(m_pDevice, m_pContext,&Potionstation_Camera_Desc));

	Safe_Release(pGameInstance);

	return S_OK;
}

void CPotion_Station::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	if (m_isInteractable = fabs((m_pTransform->Get_Position() - m_pPlayerTransform->Get_Position()).Length()) < 2.f)
	{
		cout << "상호작용 버튼띄워짐." << '\n';
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_isInteractable && pGameInstance->Get_DIKeyState(DIK_L, CInput_Device::KEY_DOWN))
	{
		cout << "UI 인벤토리 띄워졌어요." << '\n';
		cout << "캐릭터를 조종 못하게했어요." << '\n';
		cout << "플레이어가 스르륵 사라져요" << '\n';

		pGameInstance->Set_Camera(TEXT("Potion_Station_Camera"),0.5f);
	}
	if (pGameInstance->Get_DIKeyState(DIK_J, CInput_Device::KEY_DOWN))
	{
		pGameInstance->Set_Camera(TEXT("Player_Camera"),0.0f);

	}

	Safe_Release(pGameInstance);
}
     
void CPotion_Station::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

CPotion_Station* CPotion_Station::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPotion_Station* pInstance = New CPotion_Station(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
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
	}
}
