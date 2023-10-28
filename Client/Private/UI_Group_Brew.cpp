#include "UI_Group_Brew.h"
#include "GameInstance.h"
#include "UI_Back.h"
#include "UI_Effect_Back.h"
#include "UI_Font.h"

#include "Texture.h"
#include "PotionTap.h"
#include "Inventory.h"
#include "Player.h"
#include "Player_Information.h"

CUI_Group_Brew::CUI_Group_Brew(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Group_Brew::CUI_Group_Brew(const CUI_Group_Brew& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Group_Brew::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CUI_Group_Brew Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Group_Brew::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_DefaultTexture();
	Ready_ExplainTexture();
	Add_Fonts();

	wstring FirstTag = TEXT("First");
	wstring SecondTag = TEXT("Second");
	wstring ThirdTag = TEXT("Third");
	wstring FourthTag = TEXT("Fourth");
	wstring FifthTag = TEXT("Fifth");
	wstring SixthTag = TEXT("Sixth");

	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Brew_1.uidata"), FirstTag, WIGEN);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Brew_2.uidata"), SecondTag, EDURUS);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Brew_3.uidata"), ThirdTag, MAXIMA);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Brew_4.uidata"), FourthTag, INVISIBLE);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Brew_5.uidata"), FifthTag, FOCUS);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Brew_6.uidata"), SixthTag, THUNDERBREW);


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = { nullptr };

	// 모든 레벨을 조사해서 플레이어 주소를 가져옴.
	for (_uint i = 0; i < LEVEL_END; ++i)
	{
		pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(
			i
			, TEXT("Layer_Player")
			, TEXT("GameObject_Player")));

		if (pPlayer != nullptr)
			break;
	}

	// 플레이어 주소에 대한 유효성 검사.
	if (nullptr == pPlayer)
	{
		__debugbreak();
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	m_pPotionTap = pPlayer->Get_Player_Information()->Get_PotionTap();
	Safe_AddRef(m_pPotionTap);
	Safe_Release(pGameInstance);

	return S_OK;
}

void CUI_Group_Brew::Tick(_float fTimeDelta)
{
	if (!m_isOpen)
		return;

	__super::Tick(fTimeDelta);

	if (nullptr == m_pInventory)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		m_pInventory = static_cast<CInventory*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Inventory"), TEXT("GameObject_Inventory")));
		Safe_AddRef(m_pInventory);

		Safe_Release(pGameInstance);
	}

	_uint iIndex = 0;
	for (auto& pIcon : m_pIcons)
	{
		if (pIcon->Get_Collision())
		{
			if (m_eCurExplain != BREW_END)
				m_pIcons[m_eCurExplain]->Set_Texture(nullptr);

			m_eCurExplain = (BREWPOTION)iIndex;
			pIcon->Set_Texture(m_pExplainTextures[iIndex]);

			if (pIcon->Get_Clicked())
			{
				// 제조하게끔 하는 로직.
				_bool isSuccess = { false };
				
				switch (iIndex)
				{
				case WIGEN:
					if (1 > m_pInventory->Get_Resource(INGREDIENT::HORKLUMP_JUICE))
					{
						break;
					}
					if (1 > m_pInventory->Get_Resource(INGREDIENT::DITTANY_LEAVES))
					{
						break;
					}
					m_pPotionTap->Add_Potion(POTIONTAP::HEALTH_POTION);
					m_pInventory->Delete_Item(ITEM_ID::ITEM_ID_HORKLUMP_JUICE);
					m_pInventory->Delete_Item(ITEM_ID::ITEM_ID_DITTANY_LEAVES);
					break;
				case EDURUS:
					if (1 > m_pInventory->Get_Resource(INGREDIENT::ASHWINDER_EGGS))
					{
						break;
					}
					if (1 > m_pInventory->Get_Resource(INGREDIENT::MONGREL_FUR))
					{
						break;
					}

					m_pPotionTap->Add_Potion(POTIONTAP::ENDURUS_POTION);
					m_pInventory->Delete_Item(ITEM_ID::ITEM_ID_ASHWINDER_EGGS);
					m_pInventory->Delete_Item(ITEM_ID::ITEM_ID_MONGREL_FUR);
					break;
				case MAXIMA:
					if (1 > m_pInventory->Get_Resource(INGREDIENT::LEECH_JUICE))
					{
						break;
					}
					if (1 > m_pInventory->Get_Resource(INGREDIENT::SPIDER_FANG))
					{
						break;
					}
					m_pPotionTap->Add_Potion(POTIONTAP::MAXIMA_POTION);
					m_pInventory->Delete_Item(ITEM_ID::ITEM_ID_LEECH_JUICE);
					m_pInventory->Delete_Item(ITEM_ID::ITEM_ID_SPIDER_FANG);
					break;
				case INVISIBLE:
					if (1 > m_pInventory->Get_Resource(INGREDIENT::LEAPING_TOADSTOOL_CAPS))
					{
						break;
					}
					if (1 > m_pInventory->Get_Resource(INGREDIENT::KNOTGRASS))
					{
						break;
					}
					m_pPotionTap->Add_Potion(POTIONTAP::INVISIBILITY_POTION);
					m_pInventory->Delete_Item(ITEM_ID::ITEM_ID_LEAPING_TOADSTOOL_CAPS);
					m_pInventory->Delete_Item(ITEM_ID::ITEM_ID_KNOTGRASS);
					break;
				case FOCUS:
					if (1 > m_pInventory->Get_Resource(INGREDIENT::LACEWING_FLIES))
					{
						break;
					}
					if (1 > m_pInventory->Get_Resource(INGREDIENT::FLUXWEED_STEM))
					{
						break;
					}
					m_pPotionTap->Add_Potion(POTIONTAP::FOCUS_POTION);
					m_pInventory->Delete_Item(ITEM_ID::ITEM_ID_LACEWING_FLIES);
					m_pInventory->Delete_Item(ITEM_ID::ITEM_ID_FLUXWEED_STEM);
					break;
				case THUNDERBREW:
					if (1 > m_pInventory->Get_Resource(INGREDIENT::LEECH_JUICE))
					{
						break;
					}
					if (1 > m_pInventory->Get_Resource(INGREDIENT::SHRIVELFIG))
					{
						break;
					}
					m_pPotionTap->Add_Potion(POTIONTAP::THUNDERBREW_POTION);
					m_pInventory->Delete_Item(ITEM_ID::ITEM_ID_LEECH_JUICE);
					m_pInventory->Delete_Item(ITEM_ID::ITEM_ID_SHRIVELFIG);
					break;
				default:
					break;
				}
			}
		}
		++iIndex;
	}

	Set_Font();
}

void CUI_Group_Brew::Late_Tick(_float fTimeDelta)
{
	if (!m_isOpen)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Group_Brew::Render()
{
	return S_OK;
}

HRESULT CUI_Group_Brew::Add_Prototype()
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

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"),
		CUI_Font::Create(m_pDevice, m_pContext, TEXT("../../Resources/Fonts/NexonGothic.spritefont")), true)))
	{
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CUI::UIDESC CUI_Group_Brew::Load_File(const HANDLE hFile, _bool isDDS)
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

HRESULT CUI_Group_Brew::Ready_DefaultTexture()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_Back_Default"), reinterpret_cast<CComponent**>(&m_pBack))))
	{
		MSG_BOX("Com_UI_Group_Brew : Failed Clone Component (Com_UI_Back_Default)");
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
	_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/Brewing/Brew_Default.png");
	lstrcpy(UIDesc.szTexturePath, szTexturePath);

	m_pBack->Load(UIDesc);

	_tchar szFilePath[MAX_PATH] = TEXT("");
	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Cursor.uidata"));
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Cursor"),
		TEXT("Com_UI_PotionTap_Cursor"), reinterpret_cast<CComponent**>(&m_pCursor), szFilePath)))
	{
		MSG_BOX("Com_UI_Group_Brew : Failed Clone Component (Com_UI_Back_Default)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CUI_Group_Brew::Ready_ExplainTexture()
{
	m_pExplainTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/Brewing/WigenWorld.png")));

	m_pExplainTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/Brewing/Edurus.png")));

	m_pExplainTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/Brewing/Maxima.png")));

	m_pExplainTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/Brewing/Invisible.png")));

	m_pExplainTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/Brewing/Focus.png")));

	m_pExplainTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/Brewing/ThunderBrew.png")));

	return S_OK;
}

HRESULT CUI_Group_Brew::Add_Fonts()
{
	CUI_Font::FONTDESC Desc;
	lstrcpy(Desc.m_pText, TEXT("dd"));
	Desc.m_vPos = { 939.f, 515.f };
	Desc.m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	Desc.m_fRotation = { 0.f };
	Desc.m_vOrigin = { 0.f, 0.f };
	Desc.m_vScale = { 0.4f, 0.4f };

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"),
		TEXT("Com_UI_Font_First"), reinterpret_cast<CComponent**>(&m_pFirstFont), &Desc)))
	{
		MSG_BOX("Com_UI_Group_Brew : Failed Clone Component (Com_UI_Font_First)");
		__debugbreak();
		return E_FAIL;
	}

	lstrcpy(Desc.m_pText, TEXT("dd"));
	Desc.m_vPos = { 939.f, 550.f };
	Desc.m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	Desc.m_fRotation = { 0.f };
	Desc.m_vOrigin = { 0.f, 0.f };
	Desc.m_vScale = { 0.4f, 0.4f };

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"),
		TEXT("Com_UI_Font_Second"), reinterpret_cast<CComponent**>(&m_pSecondFont), &Desc)))
	{
		MSG_BOX("Com_UI_Group_Brew : Failed Clone Component (Com_UI_Font_Second)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Group_Brew::Create_Component(const _tchar* pFIlePath, wstring wszTag, BREWPOTION eType)
{
	if (FAILED(Add_Components(wszTag, eType)))
		return E_FAIL;

	if (FAILED(Read_File(pFIlePath, eType)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Group_Brew::Add_Components(wstring wszTag, BREWPOTION eType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Effect_Back* pFrame = nullptr;
	CUI_Effect_Back* pIcon = nullptr;

	wstring frame = TEXT("Com_UI_Effect_Back_Frame_");
	frame += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		frame.c_str(), reinterpret_cast<CComponent**>(&pFrame))))
	{
		MSG_BOX("Com_UI_Group_Brew : Failed Clone Component (Com_UI_Effect_Back_Frame_)");
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}
	m_pFrames.push_back(pFrame);

	wstring icon = TEXT("Com_UI_Effect_Back_Icon_");
	icon += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		icon.c_str(), reinterpret_cast<CComponent**>(&pIcon))))
	{
		MSG_BOX("Com_UI_Group_Brew : Failed Clone Component (Com_UI_Back_Icon_)");
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}
	m_pIcons.push_back(pIcon);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CUI_Group_Brew::Read_File(const _tchar* pFilePath, BREWPOTION iIndex)
{
	if (nullptr == m_pIcons[iIndex] || nullptr == m_pFrames[iIndex] || iIndex >= BREWPOTION::BREW_END)
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

	m_pFrames[iIndex]->Load(Load_File(hFile, true));

	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	m_pIcons[iIndex]->Load(Load_File(hFile, true));
	m_pIcons[iIndex]->Set_Parent(m_pFrames[iIndex]);
	CUI_Image::IMAGEDESC ImageDesc;
	ImageDesc.vCombinedXY = m_pIcons[iIndex]->Get_CombinedXY();
	ImageDesc.fX = 1040.f;
	ImageDesc.fY = 480.f;
	ImageDesc.fZ = 0.1f;
	ImageDesc.fSizeX = 400.f;
	ImageDesc.fSizeY = 315.f;

	m_pIcons[iIndex]->Set_ImageCom(ImageDesc);
	//m_pIcons[iIndex]->Set_Effecttype(CUI_Effect_Back::SKILL);
	//m_pIcons[iIndex]->Set_ImageComShader(CUI_Image::SHADERTYPE::SKILL);

	//m_pIcons[iIndex]->Set_Texture(m_pExplainTextures[iIndex]);
	
	CloseHandle(hFile);

	return S_OK;
}

void CUI_Group_Brew::Set_Font()
{
	switch (m_eCurExplain)
	{
	case Client::CUI_Group_Brew::WIGEN:
	{
		Set_Font_Text(HORKLUMP_JUICE, FIRST);
		Set_Font_Text(DITTANY_LEAVES, SECOND);
	}
		break;
	case Client::CUI_Group_Brew::EDURUS:
	{
		Set_Font_Text(ASHWINDER_EGGS, FIRST);
		Set_Font_Text(MONGREL_FUR, SECOND);
	}
		break;
	case Client::CUI_Group_Brew::MAXIMA:
	{
		Set_Font_Text(LEECH_JUICE, FIRST);
		Set_Font_Text(SPIDER_FANG, SECOND);
	}
		break;
	case Client::CUI_Group_Brew::INVISIBLE:
	{
		Set_Font_Text(LEAPING_TOADSTOOL_CAPS, FIRST);
		Set_Font_Text(KNOTGRASS, SECOND);
	}
		break;
	case Client::CUI_Group_Brew::FOCUS:
	{
		Set_Font_Text(LACEWING_FLIES, FIRST);
		Set_Font_Text(FLUXWEED_STEM, SECOND);
	}
		break;
	case Client::CUI_Group_Brew::THUNDERBREW:
	{
		Set_Font_Text(LEECH_JUICE, FIRST);
		Set_Font_Text(SHRIVELFIG, SECOND);
	}
		break;
	case Client::CUI_Group_Brew::BREW_END:
		break;
	default:
		break;
	}
}

void CUI_Group_Brew::Set_Font_Text(INGREDIENT eType, FONT eFont)
{
	if (nullptr == m_pInventory)
		return;

	wstring wstrNumber = to_wstring(m_pInventory->Get_Resource(eType));
	wstring slash = TEXT(" / ");
	wstring wstrMaxNumber = TEXT("1");
	wstrNumber += slash += wstrMaxNumber;

	if (eFont == FIRST)
	{
		m_pFirstFont->Set_Text(wstrNumber);

		if (m_pInventory->Get_Resource(eType) == 0)
			m_pFirstFont->Set_vColor(_float4(1.f, 0.f, 0.f, 1.f));
		else
			m_pFirstFont->Set_vColor(_float4(0.f, 1.f, 0.f, 1.f));
	}
	else
	{
		m_pSecondFont->Set_Text(wstrNumber);

		if (m_pInventory->Get_Resource(eType) == 0)
			m_pSecondFont->Set_vColor(_float4(1.f, 0.f, 0.f, 1.f));
		else
			m_pSecondFont->Set_vColor(_float4(0.f, 1.f, 0.f, 1.f));

	}
}

CUI_Group_Brew* CUI_Group_Brew::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_Brew* pInstance = new CUI_Group_Brew(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_Brew");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Group_Brew::Clone(void* pArg)
{
	CUI_Group_Brew* pInstance = new CUI_Group_Brew(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Group_Brew");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Group_Brew::Free()
{
	__super::Free();

	for (auto& pFrames : m_pFrames)
	{
		Safe_Release(pFrames);
	}

	for (auto& pIcon : m_pIcons)
	{
		Safe_Release(pIcon);
	}

	Safe_Release(m_pBack);
	Safe_Release(m_pCursor);
	Safe_Release(m_pInventory);
	Safe_Release(m_pPotionTap);
	Safe_Release(m_pFirstFont);
	Safe_Release(m_pSecondFont);

	for (auto& pExplain : m_pExplainTextures)
	{
		Safe_Release(pExplain);
	}
}