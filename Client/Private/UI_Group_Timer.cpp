#include "UI_Group_Timer.h"
#include "GameInstance.h"
#include "UI_Back.h"
#include "UI_Font.h"

CUI_Group_Timer::CUI_Group_Timer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Group_Timer::CUI_Group_Timer(const CUI_Group_Timer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Group_Timer::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CUI_Group_Timer Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Group_Timer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_pTime = (_float*)pArg;
		if (m_pTime == nullptr)
			return E_FAIL;
	}

	if (FAILED(Add_Fonts(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_Group_Timer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Group_Timer::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pTime)
	{
		wstring time = to_wstring(*m_pTime);
		m_pUI_Time->Set_Text(time.substr(0, time.find('.') + 3).c_str());
	}

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Group_Timer::Add_Prototype()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		CUI_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CUI_Group_Timer::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_Alarm"), reinterpret_cast<CComponent**>(&m_pUI_Alarm))))
	{
		MSG_BOX("Com_CUI_Timger : Failed Clone Component (Com_UI_Alarm)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	CUI::UIDESC UIDesc;
	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 580.f };
	UIDesc.fY = { 50.f };
	UIDesc.fZ = { 0.8f };
	UIDesc.fSizeX = { 64.f };
	UIDesc.fSizeY = { 64.f };
	_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/UI_Edit/UI_T_TimeCost.png");
	lstrcpy(UIDesc.szTexturePath, szTexturePath);

	m_pUI_Alarm->Load(UIDesc);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CUI_Group_Timer::Add_Fonts(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Font::FONTDESC Desc;
	ZeroMemory(&Desc, sizeof(CUI_Font::FONTDESC));
	if (m_pTime)
		lstrcpy(Desc.m_pText, to_wstring(*m_pTime).c_str());
	else
		lstrcpy(Desc.m_pText, TEXT(""));

	Desc.m_vPos = { 703.f, 38.f };
	Desc.m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	Desc.m_fRotation = { 0.f };
	Desc.m_vOrigin = { 0.f, 0.f };
	Desc.m_vScale = { 0.8f, 0.8f };

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"),
		TEXT("Com_UI_Font_Time"), reinterpret_cast<CComponent**>(&m_pUI_Time), &Desc)))
	{
		MSG_BOX("CUI_Group_Timer : Failed Clone Component (Com_UI_Font_Name)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CUI_Group_Timer* CUI_Group_Timer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_Timer* pInstance = New CUI_Group_Timer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_Timer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Group_Timer::Clone(void* pArg)
{
	CUI_Group_Timer* pInstance = New CUI_Group_Timer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Group_Timer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Group_Timer::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Alarm);
	Safe_Release(m_pUI_Time);
}