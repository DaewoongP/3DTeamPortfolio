//#include "Menu_Gear.h"
//#include "GameInstance.h"
//#include "UI_Effect_Back.h"
//#include "UI_Back.h"
//
//CMenu_Gear::CMenu_Gear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//	: CGameObject(pDevice, pContext)
//{
//}
//
//CMenu_Gear::CMenu_Gear(const CMenu_Gear& rhs)
//	: CGameObject(rhs)
//{
//}
//
//HRESULT CMenu_Gear::Initialize_Prototype()
//{
//	__super::Initialize_Prototype();
//
//	if (FAILED(Add_Prototype()))
//	{
//		MSG_BOX("Failed CMenu_Gear Add ProtoType");
//		return E_FAIL;
//	}
//
//	return S_OK;
//}
//
//HRESULT CMenu_Gear::Initialize(void* pArg)
//{
//	if (FAILED(__super::Initialize(pArg)))
//		return E_FAIL;
//
//	if (FAILED(Add_Components()))
//		return E_FAIL;
//
//	if (FAILED(Read_File(reinterpret_cast<const _tchar*>(pArg))))
//		return E_FAIL;
//
//	return S_OK;
//}
//
//void CMenu_Gear::Tick(_float fTimeDelta)
//{
//	__super::Tick(fTimeDelta);
//
//}
//
//void CMenu_Gear::Late_Tick(_float fTimeDelta)
//{
//	__super::Late_Tick(fTimeDelta);
//}
//
//HRESULT CMenu_Gear::Render()
//{
//	
//		return S_OK;
//}
//
//HRESULT CMenu_Gear::Add_Prototype()
//{
//	CGameInstance* pGameInstance = CGameInstance::GetInstance();
//	Safe_AddRef(pGameInstance);
//
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
//		CUI_Back::Create(m_pDevice, m_pContext), true)))
//	{
//		Safe_Release(pGameInstance);
//		return E_FAIL;
//	}
//
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Effect_Back"),
//		CUI_Effect_Back::Create(m_pDevice, m_pContext), true)))
//	{
//		Safe_Release(pGameInstance);
//		return E_FAIL;
//	}
//
//	Safe_Release(pGameInstance);
//
//	return S_OK;
//}
//
//HRESULT CMenu_Gear::Add_Components()
//{
//	CGameInstance* pGameInstance = CGameInstance::GetInstance();
//	Safe_AddRef(pGameInstance);
//
//	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"), TEXT("Layer_Menu_UI"), TEXT("Com_UI_Effect_Back_Menu_Frame"))))
//	{
//		MSG_BOX("Com_Menu : Failed Clone Component (Com_UI_Effect_Back_Menu_Frame)");
//		Safe_Release(pGameInstance);
//		return E_FAIL;
//	}
//
//	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"), TEXT("Layer_Menu_UI"), TEXT("Com_UI_Effect_Back_Menu_Q"))))
//	{
//		MSG_BOX("Com_Menu : Failed Clone Component (Com_UI_Effect_Back_Menu_Keys)");
//		Safe_Release(pGameInstance);
//		return E_FAIL;
//	}
//
//	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"), TEXT("Layer_Menu_UI"), TEXT("Com_UI_Effect_Back_Menu_E"))))
//	{
//		MSG_BOX("Com_Menu : Failed Clone Component (Com_UI_Effect_Back_Menu_Keys)");
//		Safe_Release(pGameInstance);
//		return E_FAIL;
//	}
//
//	wstring text = TEXT("Gear");
//	Add_TextComponent(text);
//	text = TEXT("Inventory");
//	Add_TextComponent(text);
//	text = TEXT("Quest");
//	Add_TextComponent(text);
//	text = TEXT("Setting");
//	Add_TextComponent(text);
//
//	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"), TEXT("Layer_Menu_UI"), TEXT("Com_UI_Effect_Back_Menu_Back"))))
//	{
//		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Menu_Back)");
//		Safe_Release(pGameInstance);
//		return E_FAIL;
//	}
//
//	Safe_Release(pGameInstance);
//
//	return S_OK;
//}
//
//HRESULT CMenu_Gear::Add_TextComponent(wstring wszTag)
//{
//	CGameInstance* pGameInstance = CGameInstance::GetInstance();
//	Safe_AddRef(pGameInstance);
//
//	CUI_Effect_Back* pText = nullptr;
//
//	wstring text = TEXT("Com_UI_Effect_Back_Text_");
//	text += wszTag;
//	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Effect_Back"),
//		text.c_str(), reinterpret_cast<CComponent**>(&pText))))
//	{
//		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Text)");
//		Safe_Release(pGameInstance);
//		return E_FAIL;
//	}
//	m_pTexts.push_back(pText);
//
//	Safe_Release(pGameInstance);
//	return S_OK;
//}
//
//HRESULT CMenu_Gear::Read_File(const _tchar* pFilePath)
//{
//
//	return S_OK;
//}
//
//CUI::UIDESC CMenu_Gear::Load_File(const HANDLE hFile)
//{
//	CUI::UIDESC UIDesc;
//	ZEROMEM(&UIDesc);
//
//	_ulong dwByte = 0;
//	DWORD dwStrByte = 0;
//	_tchar szTextureName[MAX_PATH] = TEXT("");
//	_tchar szAlphaPrototypeTag[MAX_PATH] = TEXT("");
//	_bool isParent, isAlpha, isSave;
//	_int eID;
//
//	ReadFile(hFile, &UIDesc.vCombinedXY, sizeof(_float2), &dwByte, nullptr);
//	ReadFile(hFile, &UIDesc.fX, sizeof(_float), &dwByte, nullptr);
//	ReadFile(hFile, &UIDesc.fY, sizeof(_float), &dwByte, nullptr);
//	ReadFile(hFile, &UIDesc.fZ, sizeof(_float), &dwByte, nullptr);
//	ReadFile(hFile, &UIDesc.fSizeX, sizeof(_float), &dwByte, nullptr);
//	ReadFile(hFile, &UIDesc.fSizeY, sizeof(_float), &dwByte, nullptr);
//	ReadFile(hFile, szTextureName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
//	ReadFile(hFile, UIDesc.szTexturePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
//	ReadFile(hFile, &isParent, sizeof(_bool), &dwByte, nullptr);
//	ReadFile(hFile, &isAlpha, sizeof(_bool), &dwByte, nullptr);
//	ReadFile(hFile, &UIDesc.vColor, sizeof(_float4), &dwByte, nullptr);
//	ReadFile(hFile, szAlphaPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
//	ReadFile(hFile, UIDesc.szAlphaTexturePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
//	ReadFile(hFile, &eID, sizeof(_int), &dwByte, nullptr);
//	ReadFile(hFile, &isSave, sizeof(_bool), &dwByte, nullptr);
//
//	return UIDesc;
//}
//
//CMenu_Gear* CMenu_Gear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//{
//	CMenu_Gear* pInstance = new CMenu_Gear(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype()))
//	{
//		MSG_BOX("Failed to Created CMenu_Gear");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//CGameObject* CMenu_Gear::Clone(void* pArg)
//{
//	CMenu_Gear* pInstance = new CMenu_Gear(*this);
//
//	if (FAILED(pInstance->Initialize(pArg)))
//	{
//		MSG_BOX("Failed to Cloned CMenu_Gear");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//void CMenu_Gear::Free()
//{
//	__super::Free();
//
//	Safe_Release(m_pShaderCom);
//	Safe_Release(m_pRendererCom);
//	Safe_Release(m_pVIBufferCom);
//	Safe_Release(m_pTexture);
//}
