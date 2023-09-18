#include "UI_Button.h"
#include "Input_Device.h"

CUI_Button::CUI_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CUI_Button::CUI_Button(const CUI_Button& rhs)
	: CComponent(rhs)
{
}

HRESULT CUI_Button::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Button::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_Button::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Button::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

_bool CUI_Button::Collision_Rect(HWND hWnd, _float2 vPos, _float2 vSize)
{
	_bool		isIn = false;

	POINT		ptMouse;
	GetCursorPos(&ptMouse);

	ScreenToClient(hWnd, &ptMouse);

	RECT		rcUI;

	SetRect(&rcUI, _int(vPos.x - vSize.x * 0.5f), _int(vPos.y - vSize.y * 0.5f),
		_int(vPos.x + vSize.x * 0.5f), _int(vPos.y + vSize.y * 0.5f));

	isIn = PtInRect(&rcUI, ptMouse);

	return isIn;
}


_bool CUI_Button::Click(HWND hWnd, _float2 vPos, _float2 vSize)
{

	CInput_Device* pInputDevice = CInput_Device::GetInstance();
	Safe_AddRef(pInputDevice);

	if (Collision_Rect(hWnd, vPos, vSize))
	{
		if (pInputDevice->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
		{
			Safe_Release(pInputDevice);
			return true;
		}
	}

	Safe_Release(pInputDevice);

	return false;
}

CUI_Button* CUI_Button::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Button* pInstance = New CUI_Button(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Button");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CUI_Button::Clone(void* pArg)
{
	CUI_Button* pInstance = New CUI_Button(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Button");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Button::Free()
{
	__super::Free();
}
