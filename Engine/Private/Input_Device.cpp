#include "Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
	: m_pInputSDK(nullptr)
	, m_pKeyBoard(nullptr)
	, m_pMouse(nullptr)
{
	ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
	ZeroMemory(&m_byPreKeyState, sizeof(m_byPreKeyState));
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
	ZeroMemory(&m_PreMouseState, sizeof(DIMOUSESTATE));
}
_bool CInput_Device::Get_DIKeyState(_ubyte ubyKeyID, KEYSTATE eState)
{
	switch (eState)
	{
	case KEY_DOWN:
		return Key_Down(ubyKeyID);
	case KEY_PRESSING:
		return Key_Pressing(ubyKeyID);
	case KEY_UP:
		return Key_Up(ubyKeyID);
	default:
		return false;
	}

	return false;
}
_bool CInput_Device::Get_DIMouseState(MOUSEKEYSTATE eMouseID, KEYSTATE eState)
{
	switch (eState)
	{
	case KEY_DOWN:
		return Mouse_Down(eMouseID);
	case KEY_PRESSING:
		return Mouse_Pressing(eMouseID);
	case KEY_UP:
		return Mouse_Up(eMouseID);
	default:
		return false;
	}

	return false;
}

_long CInput_Device::Get_DIMouseMove(MOUSEMOVESTATE eMouseMoveID)
{
	return *(((_long*)&m_MouseState) + eMouseMoveID);
}

HRESULT CInput_Device::Ready_Input_Device(HINSTANCE hInst, HWND hWnd)
{
	// �θ� �İ�ü ����
	FAILED_CHECK_RETURN(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr), E_FAIL);

	// �� ��ǻ�Ϳ� ����Ǿ� �ִ� Ű���� ��ü ����
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr), E_FAIL);

	// ������ Ű���� ��ü�� ���� ������ �İ�ü���� ����
	FAILED_CHECK_RETURN(m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard), E_FAIL);

	// ������ Ű���� ��ü�� ���� ������ ����
	FAILED_CHECK_RETURN(m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE), E_FAIL);

	// ������ Ű���� �İ�ü�� Ȱ��ȭ
	FAILED_CHECK_RETURN(m_pKeyBoard->Acquire(), E_FAIL);

	// �� ��ǻ�Ϳ� ����Ǿ� �ִ� ���콺 ��ü ����
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr), E_FAIL);

	// ������ ���콺 ��ü�� ���� ������ �İ�ü���� ����
	FAILED_CHECK_RETURN(m_pMouse->SetDataFormat(&c_dfDIMouse), E_FAIL);

	// ������ ���콺 ��ü�� ���� ������ ����
	FAILED_CHECK_RETURN(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE), E_FAIL);

	// ������ ���콺 �İ�ü�� Ȱ��ȭ
	FAILED_CHECK_RETURN(m_pMouse->Acquire(), E_FAIL);

	return S_OK;
}

void CInput_Device::Tick(void)
{
	memcpy(m_byPreKeyState, m_byKeyState, sizeof(_byte) * MAX_DIK);
	memcpy(&m_PreMouseState, &m_MouseState, sizeof(DIMOUSESTATE));

	m_pKeyBoard->GetDeviceState(MAX_DIK, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
}

_bool CInput_Device::Key_Down(const _ubyte& ubyKey)
{
	if (!(m_byPreKeyState[ubyKey] & 0x80) && (m_byKeyState[ubyKey] & 0x80))
		return true;

	return false;
}

_bool CInput_Device::Key_Pressing(const _ubyte& ubyKey)
{
	if ((m_byPreKeyState[ubyKey] & 0x80) && (m_byKeyState[ubyKey] & 0x80))
		return true;

	return false;
}

_bool CInput_Device::Key_Up(const _ubyte& ubyKey)
{
	if ((m_byPreKeyState[ubyKey] & 0x80) && !(m_byKeyState[ubyKey] & 0x80))
		return true;

	return false;
}

_bool CInput_Device::Mouse_Down(MOUSEKEYSTATE eMouseID)
{
	if (!m_PreMouseState.rgbButtons[eMouseID] && m_MouseState.rgbButtons[eMouseID])
		return true;

	return false;
}

_bool CInput_Device::Mouse_Pressing(MOUSEKEYSTATE eMouseID)
{
	if (m_PreMouseState.rgbButtons[eMouseID] && m_MouseState.rgbButtons[eMouseID])
		return true;

	return false;
}

_bool CInput_Device::Mouse_Up(MOUSEKEYSTATE eMouseID)
{
	if (m_PreMouseState.rgbButtons[eMouseID] && !m_MouseState.rgbButtons[eMouseID])
		return true;

	return false;
}

void CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
