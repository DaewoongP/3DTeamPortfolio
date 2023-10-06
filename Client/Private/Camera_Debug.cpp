#include "..\Public\Camera_Debug.h"
#include "GameInstance.h"
#include "Player.h"


CCamera_Debug::CCamera_Debug(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCamera_Debug::CCamera_Debug(const CCamera_Debug& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCamera_Debug::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Debug::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));
	m_pTransform->Set_Position(_float3(0.f, 2.f, 0.f));

	m_fCameraNear = 0.1f;
#ifdef _DEBUG
	m_isDebug = true;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	pGameInstance->Set_DebugCam(m_isDebug);

	Safe_Release(pGameInstance);

#endif
	return S_OK;
}

void CCamera_Debug::Tick(_float fTimeDelta)
{
	Key_Input(fTimeDelta);

	if (m_isFixMouse)
	{
		Fix_Mouse();
		Mouse_Move(fTimeDelta);
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Set_CameraFar(1000.f);
	pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransform->Get_WorldMatrix_Inverse());
	pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), _float(g_iWinSizeX) / g_iWinSizeY, m_fCameraNear, 1000.f));

	Safe_Release(pGameInstance);

	__super::Tick(fTimeDelta);

#ifdef _DEBUG
	ADD_IMGUI([&] { this->Tick_ImGui(); });
#endif // _DEBUG
}

void CCamera_Debug::Key_Input(const _float& fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (true == m_isDebug)
	{
		if (pGameInstance->Get_DIKeyState(DIK_W, CInput_Device::KEY_PRESSING))
		{
			m_pTransform->Go_Straight(fTimeDelta);
		}

		if (pGameInstance->Get_DIKeyState(DIK_S, CInput_Device::KEY_PRESSING))
		{
			m_pTransform->Go_Backward(fTimeDelta);
		}

		if (pGameInstance->Get_DIKeyState(DIK_A, CInput_Device::KEY_PRESSING))
		{
			m_pTransform->Go_Left(fTimeDelta);
		}

		if (pGameInstance->Get_DIKeyState(DIK_D, CInput_Device::KEY_PRESSING))
		{
			m_pTransform->Go_Right(fTimeDelta);
		}

		if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON, CInput_Device::KEY_DOWN))
		{
			CGameObject* pObject = nullptr;
			/*if (pGameInstance->Mouse_RayCast(g_hWnd, m_pContext, &pObject, "Enemy_Body", 999.f, nullptr, nullptr, 999))
			{
				int a = 1;

			}*/
		}
	}

#ifdef _DEBUG
	if (pGameInstance->Get_DIKeyState(DIK_GRAVE, CInput_Device::KEY_DOWN))
	{
		if (true == m_isFixMouse)
			m_isFixMouse = false;
		else
			m_isFixMouse = true;
	}

	if (pGameInstance->Get_DIKeyState(DIK_F3, CInput_Device::KEY_DOWN))
	{
		if (true == m_isDebug)
			m_isDebug = false;
		else
			m_isDebug = true;

		pGameInstance->Set_DebugCam(m_isDebug);
	}
#endif
	Safe_Release(pGameInstance);
}

void CCamera_Debug::Mouse_Move(_float fTimeDelta)
{
	_long		dwMouseMove = 0;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y);

	if (dwMouseMove)
	{
		_float3	vRight = m_pTransform->Get_Right();

		m_pTransform->Turn(vRight, dwMouseMove * fTimeDelta * 0.1f);

		dwMouseMove = 0;
	}

	dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X);

	if (dwMouseMove)
	{
		_float3	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		m_pTransform->Turn(vUp, dwMouseMove * fTimeDelta * 0.1f);

		dwMouseMove = 0;
	}

	Safe_Release(pGameInstance);
}

void CCamera_Debug::Fix_Mouse(void)
{
	if (false == m_isFixMouse)
		return;

	POINT	ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

#ifdef _DEBUG
void CCamera_Debug::Tick_ImGui()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	RECT rc;
	ZEROMEM(&rc);
	GetWindowRect(g_hWnd, &rc);

	ImGui::SetNextWindowPos(ImVec2(0.f, 200.f));
	ImGui::SetNextWindowSize(ImVec2(300.f, 200.f));

	ImGui::Begin("Camera");
	_float3 vPos = m_pTransform->Get_Position();

	if (ImGui::InputFloat3("Debug Cam Pos", reinterpret_cast<_float*>(&vPos)))
	{
		m_pTransform->Set_Position(vPos);
	}

	_float fSpeed = m_pTransform->Get_Speed();
	if (ImGui::SliderFloat("Cam Speed", &fSpeed, 0.1f, 200.f))
	{
		m_pTransform->Set_Speed(fSpeed);
	}

	if (ImGui::Button("Teleport Pos to Cam"))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player"), TEXT("GameObject_Player")));
		if (nullptr != pPlayer)
		{
			_float3 vPos = m_pTransform->Get_Position();
			_float3 vLook = m_pTransform->Get_Look();
			pPlayer->Get_Transform()->Set_Position(vPos + vLook);
		}
	}
	
	ImGui::End();

	Safe_Release(pGameInstance);
}
#endif // _DEBUG

CCamera_Debug* CCamera_Debug::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Debug* pInstance = New CCamera_Debug(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCamera_Debug");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Debug::Clone(void* pArg)
{
	CCamera_Debug* pInstance = New CCamera_Debug(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCamera_Debug");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Debug::Free()
{
	__super::Free();
}
