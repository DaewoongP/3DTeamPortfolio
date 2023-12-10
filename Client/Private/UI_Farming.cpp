#include "..\Public\UI_Farming.h"
#include "GameInstance.h"
#include "UI_Back.h"
#include "UI_Effect_Back.h"
#include "UI_Font.h"
#include "Item.h"
#include "UI_Dynamic_Back.h"

CUI_Farming::CUI_Farming(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Farming::CUI_Farming(const CUI_Farming& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Farming::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Farming::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Compoents()))
		return E_FAIL;

	Farming_Read_File(TEXT("../../Resources/GameData/UIData/UI_Group_Farming.uidata"));
	FontBack_Read_File(TEXT("../../Resources/GameData/UIData/UI_Group_Farming_Slide.uidata"));


	m_vFontBackOriginPos = m_pUIFarming_Font_Back->Get_XY();
	m_pUIFont->Set_vColor(_float4(0.196f, 0.804f, 0.196f, 1.f)); // 라임 그린
	//m_pUIFont->Set_Origin(_float2(100.f, 30.f));

	m_pUIFarmingFrame->PushBackNull();
	return S_OK;
}

void CUI_Farming::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (pGameInstance->Get_DIKeyState(DIK_M, CInput_Device::KEY_DOWN))
	{
		Play(nullptr);
	}
	Safe_Release(pGameInstance);
#endif // _DEBUG

	if (false == m_isEnable)
		return;

	__super::Tick(fTimeDelta);
	m_fTimeAcc += fTimeDelta;
	switch (m_eState)
	{
	case Client::CUI_Farming::LEFT:
		m_pUIFarming_Font_Back->Go_Left(80.f, fTimeDelta);
		if (m_fTimeAcc >= 1.f)
		{
			m_fTimeAcc = 0.f;
			m_eState = LOOP;
			m_pUIFont->Set_Text(m_wstrKoreanName.data());
			m_pUIFont->Left_Align();
		}
		break;
	case Client::CUI_Farming::LOOP:
		if (m_fTimeAcc >= 3.f)
		{
			m_fTimeAcc = 0.f;
			m_eState = RIGHT;
			m_pUIFont->Set_Text(TEXT(""));
		}
		break;
	case Client::CUI_Farming::RIGHT:
		m_pUIFarming_Font_Back->Go_Right(80.f, fTimeDelta);
		if (m_fTimeAcc >= 1.f)
		{
			m_fTimeAcc = 0.f;
			m_eState = STATE_END;
			m_isEnable = false;
		}
		break;
	case Client::CUI_Farming::STATE_END:
		break;
	default:
		break;
	}
	
	m_pUIFont->Set_vPos(m_pUIFarming_Font_Back->Get_XY());
}

void CUI_Farming::Late_Tick(_float fTimeDelta)
{
	if (false == m_isEnable)
		return;

	__super::Late_Tick(fTimeDelta);
}

void CUI_Farming::Play(CItem* pItem)
{
	if (nullptr == pItem)
		return;

	m_eState = LEFT;
	m_isEnable = true;
	m_fTimeAcc = 0.f;
	m_pUIFarmingBack->Play_FadeIn();
	m_pUIFarmingFrame->Play_FadeIn();
	m_pUIFarming_Font_Back->Play_FadeIn();
	m_pUIFarming_Font_TopFrame->Play_FadeIn();
	m_pUIFarming_Font_BotFrame->Play_FadeIn();
	m_pUIFarming_Font_Back->Set_XY(m_vFontBackOriginPos);
	m_pUIFarmingFrame->Set_Texture(1, pItem->Get_UITexture());
	m_pUIFont->Set_Text(TEXT(""));
	m_wstrKoreanName = pItem->Get_KoreanName();
	//m_pPlayedItem->Change_Position(
	//	m_vFontBackOriginPos.x,
	//	m_vFontBackOriginPos.y,
	//	0.85f);
}

HRESULT CUI_Farming::Farming_Read_File(const _tchar* pFilePath)
{
	/*if (nullptr == m_pIcons[iIndex] || nullptr == m_pFrames[iIndex] || nullptr == m_pCountBacks[iIndex]
		|| nullptr == m_pItemBacks[iIndex] || iIndex >= POTIONTAP::POTIONTAP_END)
	{
		MSG_BOX("Failed Load");
		return E_FAIL;
	}*/

	_ulong dwByte = 0;
	DWORD dwStrByte = 0;

	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed Load");
		CloseHandle(hFile);
		return E_FAIL;
	}

	_tchar szGroupName[MAX_PATH] = TEXT("");
	_uint iSize = 0
		;
	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, szGroupName, dwStrByte, &dwByte, nullptr);

	m_pUIFarmingBack->Load(Load_File(hFile, false));
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
	m_pUIFarmingFrame->Load(Load_File(hFile, false));
	m_pUIFarmingFrame->Set_Parent(m_pUIFarmingBack);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CUI_Farming::FontBack_Read_File(const _tchar* pFilePath)
{
	/*if (nullptr == m_pIcons[iIndex] || nullptr == m_pFrames[iIndex] || nullptr == m_pCountBacks[iIndex]
		|| nullptr == m_pItemBacks[iIndex] || iIndex >= POTIONTAP::POTIONTAP_END)
	{
		MSG_BOX("Failed Load");
		return E_FAIL;
	}*/

	_ulong dwByte = 0;
	DWORD dwStrByte = 0;

	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed Load");
		CloseHandle(hFile);
		return E_FAIL;
	}

	_tchar szGroupName[MAX_PATH] = TEXT("");
	_uint iSize = { 0 };
	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, szGroupName, dwStrByte, &dwByte, nullptr);

	m_pUIFarming_Font_Back->Load(Load_File(hFile, false));
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	m_pUIFarming_Font_TopFrame->Load(Load_File(hFile, false));
	m_pUIFarming_Font_TopFrame->Set_Parent(m_pUIFarming_Font_Back);

	m_pUIFarming_Font_BotFrame->Load(Load_File(hFile, false));
	m_pUIFarming_Font_BotFrame->Set_Parent(m_pUIFarming_Font_Back);

	CloseHandle(hFile);

	return S_OK;
}

CUI::UIDESC CUI_Farming::Load_File(const HANDLE hFile, _bool isDDS)
{
	CUI::UIDESC UIDesc;
	ZEROMEM(&UIDesc);
	_ulong dwByte = 0;
	DWORD dwStrByte = 0;
	_tchar szTextureName[MAX_PATH] = TEXT("");
	_tchar szAlphaPrototypeTag[MAX_PATH] = TEXT("");
	_bool isParent, isAlpha, isSave;
	_int eID;

	ReadFile(hFile, &UIDesc.vCombinedXY, sizeof(_float2), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.fX, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.fY, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.fZ, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.fSizeX, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.fSizeY, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, szTextureName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, UIDesc.szTexturePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &isParent, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &isAlpha, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.vColor, sizeof(_float4), &dwByte, nullptr);
	ReadFile(hFile, szAlphaPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, UIDesc.szAlphaTexturePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &eID, sizeof(_int), &dwByte, nullptr);
	ReadFile(hFile, &isSave, sizeof(_bool), &dwByte, nullptr);

	if (isDDS)
	{
		fs::path fsPath = UIDesc.szTexturePath;

		std::wstring wszExtension = fsPath.extension();
		if (wszExtension == TEXT(".png"))
		{
			fsPath.replace_extension(L".dds");
			lstrcpy(UIDesc.szTexturePath, fsPath.c_str());
		}
	}

	return UIDesc;
}

HRESULT CUI_Farming::Add_Compoents()
{
	BEGININSTANCE;
	// For. Com_UIFarmingBack
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_UI_Dynamic_Back"),
		TEXT("Com_UIFarmingBack"), reinterpret_cast<CComponent**>(&m_pUIFarmingBack))))
	{
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}

	// For. Com_UIFarmingFrame
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_UI_Dynamic_Back"),
		TEXT("Com_UIFarmingFrame"), reinterpret_cast<CComponent**>(&m_pUIFarmingFrame))))
	{
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}

	// For. Com_UIFarming_Font_Back
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_UI_Dynamic_Back"),
		TEXT("Com_UIFarming_Font_Back"), reinterpret_cast<CComponent**>(&m_pUIFarming_Font_Back))))
	{
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}

	// For. Com_UIFarming_Font_TopFrame
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_UI_Dynamic_Back"),
		TEXT("Com_UIFarming_Font_TopFrame"), reinterpret_cast<CComponent**>(&m_pUIFarming_Font_TopFrame))))
	{
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}

	// For. Com_UIFarming_Font_BotFrame
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_UI_Dynamic_Back"),
		TEXT("Com_UIFarming_Font_BotFrame "), reinterpret_cast<CComponent**>(&m_pUIFarming_Font_BotFrame))))
	{
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}

	// For. m_pUIFont
	CUI_Font::FONTDESC Desc;
	Desc.m_vColor = _float4(0.f, 1.f, 0.f, 1.f);
	Desc.m_fRotation = { 0.f };
	Desc.m_vOrigin = { 0.f, 0.f };
	Desc.m_vScale = { 0.4f, 0.4f };
	Desc.m_vPos = { 662.f, 144.f };
	lstrcpy(Desc.m_pText, TEXT(""));
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"),
		TEXT("Com_UIFont"), reinterpret_cast<CComponent**>(&m_pUIFont), &Desc)))
	{
		MSG_BOX("CUI_Farming : Failed Clone Component (Com_UIFont)");
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}

	ENDINSTANCE;
	return S_OK;
}

CUI_Farming* CUI_Farming::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Farming* pInstance = New CUI_Farming(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create CUI_Farming");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Farming::Clone(void* pArg)
{
	CUI_Farming* pInstance = New CUI_Farming(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone CUI_Farming");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Farming::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pUIFarmingBack);
		Safe_Release(m_pUIFarmingFrame);
		Safe_Release(m_pUIFarming_Font_Back);
		Safe_Release(m_pUIFarming_Font_TopFrame);
		Safe_Release(m_pUIFarming_Font_BotFrame);
		Safe_Release(m_pUIFont);
	}
}
