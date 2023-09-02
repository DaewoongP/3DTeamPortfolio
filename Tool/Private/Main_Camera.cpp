#include "Main_Camera.h"
#include "GameInstance.h"

void CMain_Camera::Set_MoveSpeed(const _float& _fMoveSpeed)
{
	m_fMoveSpeed = _fMoveSpeed;
	m_pTransform->Set_Speed(m_fMoveSpeed);
}

HRESULT CMain_Camera::Initialize(void* pArg)
{
	return S_OK;
}

void CMain_Camera::Tick(_float _TimeDelta)
{
}

void CMain_Camera::Key_Input(_float _TimeDelta)
{
	BEGININSTANCE
	
	if (pGameInstance->Get_DIKeyState(DIK_W,CInput_Device::KEY_PRESSING))
	{
		m_pTransform->Go_Straight(_TimeDelta);
	}
	if (pGameInstance->Get_DIKeyState(DIK_S, CInput_Device::KEY_PRESSING))
	{
		m_pTransform->Go_Backward(_TimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_A, CInput_Device::KEY_PRESSING))
	{
		m_pTransform->Go_Left(_TimeDelta);
	}
	if (pGameInstance->Get_DIKeyState(DIK_D, CInput_Device::KEY_PRESSING))
	{
		m_pTransform->Go_Right(_TimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_Q, CInput_Device::KEY_PRESSING))
	{
		m_pTransform->Move_Direction(m_pTransform->Get_Up(),_TimeDelta);
	}
	if (pGameInstance->Get_DIKeyState(DIK_E, CInput_Device::KEY_PRESSING))
	{
		m_pTransform->Move_Direction(-m_pTransform->Get_Up(), _TimeDelta);
	}

	ENDINSTANCE
}

void CMain_Camera::Mouse_Input(_float _TimeDelta)
{
	_long		dwMouseMove = 0;

	BEGININSTANCE

	dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y);

	if (dwMouseMove)
	{
		_float3	vRight = m_pTransform->Get_Right();

		m_pTransform->Turn(vRight, dwMouseMove * _TimeDelta * 0.1f);

		dwMouseMove = 0;
	}

	dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X);

	if (dwMouseMove)
	{
		_float3	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		m_pTransform->Turn(vUp, dwMouseMove * _TimeDelta * 0.1f);

		dwMouseMove = 0;
	}

	ENDINSTANCE
}



CMain_Camera* CMain_Camera::Create()
{
	return nullptr;
}

void CMain_Camera::Free()
{
}
