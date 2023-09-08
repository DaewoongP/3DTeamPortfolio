#include "..\Public\Camera_Debug.h"
#include "GameInstance.h"

#ifdef _DEBUG
#include "Test_Player.h"
#endif // _DEBUG


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

	m_fCameraNear = 1.f;

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
	Debug_ImGui(fTimeDelta);
#endif // _DEBUG


}

void CCamera_Debug::Key_Input(const _float& fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

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

	if (pGameInstance->Get_DIKeyState(DIK_GRAVE, CInput_Device::KEY_DOWN))
	{
		if (true == m_isFixMouse)
			m_isFixMouse = false;
		else
			m_isFixMouse = true;
	}

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
void CCamera_Debug::Debug_ImGui(_float fTimeDelta)
{
	ImGui::Begin("Camera Debug");

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCamera_Debug* pCam = static_cast<CCamera_Debug*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Debug"), TEXT("GameObject_Camera_Debug")));

	_float fSpeed = pCam->Get_Speed();

	ImGui::Checkbox("Fix Mouse : ~ ", pCam->Get_FixMouse());

	ImGui::SameLine();
	ImGui::SetNextItemWidth(200.f);
	if (ImGui::SliderFloat("Camera Speed", &fSpeed, 0.f, 250.f, "%.1f"))
	{
		pCam->Set_Speed(fSpeed);
	}

	CTest_Player* pTest_Player = static_cast<CTest_Player*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Debug"), TEXT("GameObject_Test_Player")));

	if (ImGui::Checkbox("3rd Camera", &m_is3rdCam))
	{
		CTransform* pTransform = pTest_Player->Get_Transform();
		m_v3rdCamOffset -= pTransform->Get_Look() * 5.f;
		m_v3rdCamOffset += pTransform->Get_Up() * 5.f;
	}
	if (true == m_is3rdCam)
	{
		ImGui::SameLine();
		ImGui::Checkbox("Fix Camera Move", &m_isFix3rdCam);
		_float3 vPlayerPos = pTest_Player->Get_Transform()->Get_Position();
		if (true == m_isFix3rdCam)
		{
			pCam->LookAt(vPlayerPos);
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(150.f);
		ImGui::InputFloat3("3rd Cam Offset", reinterpret_cast<_float*>(&m_v3rdCamOffset), "%.1f");
		pCam->Set_Position(vPlayerPos + m_v3rdCamOffset);
	}

	if (ImGui::InputFloat("Cam near", &m_fCameraNear, 0.1f))
	{
		if (m_fCameraNear < 0.1f)
		{
			m_fCameraNear = 0.1f;
		}
	}

	if (ImGui::Button("Goto 0"))
	{
		pCam->Set_Position(_float3(0.5f, 5.f, -5.f));
		pCam->LookAt(_float3(0.f, 0.f, 0.f));
		
	}

	

	Safe_Release(pGameInstance);

	ImGui::End();
}
#endif //_DEBUG
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
