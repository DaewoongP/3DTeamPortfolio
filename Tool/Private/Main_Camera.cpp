#include "Main_Camera.h"
#include "GameInstance.h"

CMain_Camera::CMain_Camera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CCamera(_pDevice, _pContext)
{

}

void CMain_Camera::Set_MoveSpeed(const _float& _fMoveSpeed)
{
	m_fMoveSpeed = _fMoveSpeed;
	m_pTransform->Set_Speed(m_fMoveSpeed);
}

HRESULT CMain_Camera::Initialize(void* pArg)
{
	CCamera::CAMERADESC* pCameraDesc = (CCamera::CAMERADESC*)pArg;

	CCamera::Initialize(*pCameraDesc);

	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));
	m_pTransform->Set_Position(_float3(0.f, 2.f, 0.f));

	return S_OK;
}

void CMain_Camera::Tick(const _float& _TimeDelta)
{
	Key_Input(_TimeDelta);

	if (true == m_isLimit)
	{
		Mouse_Input(_TimeDelta);
	}

	CCamera::Tick(_TimeDelta);
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

void CMain_Camera::IsLimitButton()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_GRAVE, CInput_Device::KEY_DOWN))
	{
		if (true == m_isLimit)
			m_isLimit = false;
		else
			m_isLimit = true;
	}

	ENDINSTANCE;
}



CMain_Camera* CMain_Camera::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg)
{
	CMain_Camera* pInstance = New CMain_Camera(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create CMain_Camera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMain_Camera::Free()
{
	CCamera::Free();
}
