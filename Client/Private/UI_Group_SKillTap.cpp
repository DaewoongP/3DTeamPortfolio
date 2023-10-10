#include "UI_Group_SKillTap.h"
#include "GameInstance.h"
#include "UI_Effect_Back.h"
#include "UI_Back.h"

CUI_Group_SkillTap::CUI_Group_SkillTap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Group_SkillTap::CUI_Group_SkillTap(const CUI_Group_SkillTap& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Group_SkillTap::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CUI_Group_SkillTap Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Group_SkillTap::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	Ready_DefaultTexture();

	Ready_SkillTextures();
	Ready_SkillSelect();

	return S_OK;
}

void CUI_Group_SkillTap::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//Swap_InventoryItem();
}

void CUI_Group_SkillTap::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Group_SkillTap::Render()
{
	return S_OK;
}

HRESULT CUI_Group_SkillTap::Add_Prototype()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		CUI_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		CUI_Effect_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CUI_Group_SkillTap::Create_Component_Select(const _tchar* pFIlePath, wstring wszTag, SKILLTAP eType)
{
	if (FAILED(Add_Components_Select(wszTag)))
		return E_FAIL;

	if (FAILED(Read_File_Select(pFIlePath, eType)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Group_SkillTap::Add_Components_Select(wstring wszTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Effect_Back* pFrame = nullptr;
	CUI_Back* pIcon = nullptr;
	CUI_Effect_Back* pBack = nullptr;


	wstring frame = TEXT("Com_UI_Effect_Back_Frame_");
	frame += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		frame.c_str(), reinterpret_cast<CComponent**>(&pFrame))))
	{
		MSG_BOX("Com_UI_SkillTap : Failed Clone Component (Com_UI_Back_Icon_)");
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}
	m_pSelectFrames.push_back(pFrame);

	wstring icon = TEXT("Com_UI_Back_Icon_");
	icon += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		icon.c_str(), reinterpret_cast<CComponent**>(&pIcon))))
	{
		MSG_BOX("Com_UI_SkillTap : Failed Clone Component (Com_UI_Effect_Back_Frame_)");
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}
	m_pSelectIcons.push_back(pIcon);

	wstring back = TEXT("Com_UI_Effect_Back_Back_");
	back += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		back.c_str(), reinterpret_cast<CComponent**>(&pBack))))
	{
		MSG_BOX("Com_UI_SkillTap : Failed Clone Component (Com_UI_Effect_Back_Back_)");
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}
	m_pSelectBacks.push_back(pBack);

	Safe_Release(pGameInstance);
	return S_OK;

}

HRESULT CUI_Group_SkillTap::Read_File_Select(const _tchar* pFilePath, SKILLTAP iIndex)
{
	if (nullptr == m_pSelectFrames[iIndex] || nullptr == m_pSelectIcons[iIndex] || nullptr == m_pSelectBacks[iIndex])
	{
		MSG_BOX("Failed Load");
		return E_FAIL;
	}

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

	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, szGroupName, dwStrByte, &dwByte, nullptr);

	m_pSelectFrames[iIndex]->Load(Load_File(hFile));
	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
	m_pSelectFrames[iIndex]->Set_Effecttype(CUI_Effect_Back::ALPHA);

	m_pSelectIcons[iIndex]->Load(Load_File(hFile, false));
	m_pSelectIcons[iIndex]->Set_Parent(m_pSelectFrames[iIndex]);

	m_pSelectBacks[iIndex]->Load(Load_File(hFile));
	m_pSelectBacks[iIndex]->Set_Parent(m_pSelectFrames[iIndex]);

	CloseHandle(hFile);

	return S_OK;
}

CUI::UIDESC CUI_Group_SkillTap::Load_File(const HANDLE hFile, _bool isDDS)
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

HRESULT CUI_Group_SkillTap::Ready_DefaultTexture()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_Back_Default"), reinterpret_cast<CComponent**>(&m_pBack))))
	{
		MSG_BOX("Com_CUI_SkillTap : Failed Clone Component (Com_UI_Back_Default)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	CUI::UIDESC UIDesc;
	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 640.f };
	UIDesc.fY = { 360.f };
	UIDesc.fZ = { 0.8f };
	UIDesc.fSizeX = { 1280.f };
	UIDesc.fSizeY = { 720.f };
	_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/SkillTap/SkillTap_Default.png");
	lstrcpy(UIDesc.szTexturePath, szTexturePath);

	m_pBack->Load(UIDesc);
	

	_tchar szFilePath[MAX_PATH] = TEXT("");
	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Cursor.uidata"));
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Cursor"),
		TEXT("Com_UI_PotionTap_Cursor"), reinterpret_cast<CComponent**>(&m_pCursor), szFilePath)))
	{
		MSG_BOX("Com_UI_Potion_Tap : Failed Clone Component (Com_UI_Back_Default)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CUI_Group_SkillTap::Ready_SkillTextures()
{
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_arrestomomentum.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_glacius.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_levioso.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_spell_transformationoverland.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_depulso.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_Descendo.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_flipendo.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_confringo.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_diffindo.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_expelliarmus.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_expulso.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_incendio.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_disillusionment.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_lumos.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_reparo.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_wingardium.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_avadakedavra.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_spell_Crucio.png")));
	m_SkillTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/SkillTap/UI_T_imperius.png")));

	return S_OK;
}

HRESULT CUI_Group_SkillTap::Ready_SkillSelect()
{
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_0.uidata"), TEXT("0"), ARRESTOMOMENTUM);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_1.uidata"), TEXT("1"), GALCIUS);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_2.uidata"), TEXT("2"), LEVIOSO);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_3.uidata"), TEXT("3"), TRANSFORMATIONOVERLAND);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_4.uidata"), TEXT("4"), ACCIO);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_5.uidata"), TEXT("5"), DEPULSO);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_6.uidata"), TEXT("6"), DESCENDO);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_7.uidata"), TEXT("7"), FILPENDO);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_8.uidata"), TEXT("8"), CONFRINGO);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_9.uidata"), TEXT("9"), DIFFINDO);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_10.uidata"), TEXT("10"), EXPELLIAMUS);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_11.uidata"), TEXT("11"), EXPULSO);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_12.uidata"), TEXT("12"), INCENDIO);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_13.uidata"), TEXT("13"), DISILLUSIONMENT);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_14.uidata"), TEXT("14"), LUMOS);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_15.uidata"), TEXT("15"), REPARE);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_16.uidata"), TEXT("16"), WINGARDIUM);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_17.uidata"), TEXT("17"), AVADAKEDAVRA);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_18.uidata"), TEXT("18"), CRUCIO);
	Create_Component_Select(TEXT("../../Resources/GameData/UIData/UI_Group_SkillTap_19.uidata"), TEXT("19"), IMPERIUS);

	return S_OK;
}

HRESULT CUI_Group_SkillTap::Ready_SkillSlot()
{


	return S_OK;
}


CUI_Group_SkillTap* CUI_Group_SkillTap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_SkillTap* pInstance = new CUI_Group_SkillTap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_SkillTap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Group_SkillTap::Clone(void* pArg)
{
	CUI_Group_SkillTap* pInstance = new CUI_Group_SkillTap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Group_SkillTap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Group_SkillTap::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);

	for (auto& pFrame : m_pSelectFrames)
	{
		Safe_Release(pFrame);
	}

	for (auto& pIcon : m_pSelectIcons)
	{
		Safe_Release(pIcon);
	}

	for (auto& pBack : m_pSelectBacks)
	{
		Safe_Release(pBack);
	}



	for (auto& pMain : m_pSlotMains)
	{
		Safe_Release(pMain);
	}

	for (auto& pFrame : m_pSlotFrames)
	{
		Safe_Release(pFrame);
	}

	for (auto& pNumber : m_pSlotNumbers)
	{
		Safe_Release(pNumber);
	}

	for (auto& pTexture : m_SkillTextures)
	{
		Safe_Release(pTexture);
	}

	Safe_Release(m_pBack);
	Safe_Release(m_pCursor);
}
