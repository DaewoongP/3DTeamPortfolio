#include "UI_Group_Skill.h"
#include "GameInstance.h"


#include "UI_Back.h"
#include "UI_Effect_Back.h"

CUI_Group_Skill::CUI_Group_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Group_Skill::CUI_Group_Skill(const CUI_Group_Skill& rhs)
	: CGameObject(rhs)
	, m_ProtoTypeTags(rhs.m_ProtoTypeTags)
	, m_SpellProtoTypeTags(rhs.m_SpellProtoTypeTags)
	, m_SkillTextures(rhs.m_SkillTextures)
{
}

HRESULT CUI_Group_Skill::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CUI_Group_Skill Add ProtoType");
		return E_FAIL;
	}

	Add_SpellProtoTypeTag();
	Add_SpellProtoType();
	Add_SpellTexture();

	return S_OK;
}

HRESULT CUI_Group_Skill::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	//_tchar SecondTag[MAX_PATH] = TEXT("Second");
//	_tchar ThirdTag[MAX_PATH] = TEXT("Third");
	//_tchar FourthTag[MAX_PATH] = TEXT("Fourth");

	wstring SecondTag = TEXT("Second");
	wstring ThirdTag = TEXT("Third");
	wstring FourthTag = TEXT("Fourth");

	Create_First(pArg);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Skill_2.uidata"), SecondTag, SECOND);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Skill_3.uidata"), ThirdTag, THIRD);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Skill_4.uidata"), FourthTag, FOURTH);

	m_KeyList[FIRST] = BASICCAST;
	m_KeyList[SECOND] = BASICCAST;
	m_KeyList[THIRD] = BASICCAST;
	m_KeyList[FOURTH] = BASICCAST;

	return S_OK;
}

void CUI_Group_Skill::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	for (size_t i = 0; i < KEYLIST_END; i++)
	{
		Set_SpellTexture((KEYLIST)i, (SPELL)i);
	}
}

void CUI_Group_Skill::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CUI_Group_Skill::Set_SpellTexture(KEYLIST eKey, SPELL eSpell)
{
	if (nullptr == m_pMains[eKey] || eKey >= KEYLIST_END || eSpell >= SPELL_END)
		return;

	if (m_KeyList[eKey] == eSpell)
		return;
	
	m_KeyList[eKey] = eSpell;
	m_pMains[eKey]->Set_Texture(m_SkillTextures[eSpell]);
}


HRESULT CUI_Group_Skill::Add_Prototype()
{
	BEGININSTANCE;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Effect_Back"),
		CUI_Effect_Back::Create(m_pDevice, m_pContext), true)))
	{
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Back"),
		CUI_Back::Create(m_pDevice, m_pContext), true)))
	{
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

//HRESULT CUI_Group_Skill::Add_Components(const _tchar* wszTag)
HRESULT CUI_Group_Skill::Add_Components(wstring wszTag)
{
	BEGININSTANCE;
	CUI_Effect_Back* pMain = nullptr;
	CUI_Back* pFrame= nullptr;
	CUI_Back* pNumber = nullptr;

	//_tchar main[MAX_PATH] = TEXT("Com_UI_Effect_Back_Main_");
	//lstrcat(main, wszTag);
	//_tchar* pmainTag = pGameInstance->Make_WChar(main);
	wstring main = TEXT("Com_UI_Effect_Back_Main_");
	main += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Effect_Back"),
		main.c_str(), reinterpret_cast<CComponent**>(&pMain))))
	{
		MSG_BOX("Com_UI_Group_Skill : Failed Clone Component (Com_UI_Effect_Back_Main_)");
		ENDINSTANCE;
		return E_FAIL;
	}
	m_pMains.push_back(pMain);


	//_tchar frame[MAX_PATH] = TEXT("Com_UI_Back_Frame_");
	//lstrcat(main, wszTag);
	//_tchar* pframeTag = pGameInstance->Make_WChar(main);
	wstring frame = TEXT("Com_UI_Back_Frame_");
	frame += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Back"),
		frame.c_str(), reinterpret_cast<CComponent**>(&pFrame))))
	{
		MSG_BOX("Com_UI_Group_Skill : Failed Clone Component (Com_UI_Back_Frame)");
		ENDINSTANCE;
		return E_FAIL;
	}
	m_pFrames.push_back(pFrame);


	//_tchar number[MAX_PATH] = TEXT("Com_UI_Back_Number_");
	//lstrcat(main, wszTag);
	//_tchar* pnumberTag = pGameInstance->Make_WChar(main);
	wstring number = TEXT("Com_UI_Back_Number_");
	number += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Back"),
		number.c_str(), reinterpret_cast<CComponent**>(&pNumber))))
	{
		MSG_BOX("Com_UI_Group_Skill : Failed Clone Component (Com_UI_Back_Number)");
		ENDINSTANCE;
		return E_FAIL;
	}
	m_pNumbers.push_back(pNumber);

	ENDINSTANCE;

	return S_OK;
}

HRESULT CUI_Group_Skill::Read_File(const _tchar* pFilePath, KEYLIST iIndex)
{
	if (nullptr == m_pMains[iIndex] || nullptr == m_pFrames[iIndex] || nullptr == m_pNumbers[iIndex] || iIndex >= KEYLIST_END)
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

	m_pMains[iIndex]->Load(Load_File(hFile));
	CUI_Image::IMAGEDESC ImageDesc;
	ImageDesc.vCombinedXY = m_pMains[iIndex]->Get_CombinedXY();
	ImageDesc.fX = m_pMains[iIndex]->Get_XY().x;
	ImageDesc.fY = m_pMains[iIndex]->Get_XY().y;
	ImageDesc.fZ = 0.f;
	ImageDesc.fSizeX = m_pMains[iIndex]->Get_SizeXY().x;
	ImageDesc.fSizeY = m_pMains[iIndex]->Get_SizeXY().y;

	m_pMains[iIndex]->Set_ImageCom(ImageDesc);
	
	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	m_pFrames[iIndex]->Load(Load_File(hFile));
	m_pFrames[iIndex]->Set_Parent(m_pMains[iIndex]);

	m_pNumbers[iIndex]->Load(Load_File(hFile));
	m_pNumbers[iIndex]->Set_Parent(m_pMains[iIndex]);

	CloseHandle(hFile);

	return S_OK;
}

CUI::UIDESC CUI_Group_Skill::Load_File(const HANDLE hFile)
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

	return UIDesc;
}

HRESULT CUI_Group_Skill::Create_First(void* pArg)
{
	//_tchar FirstTag[MAX_PATH] = TEXT("First");
	wstring wstrTag = TEXT("Front");
	if (FAILED(Add_Components(wstrTag)))
		return E_FAIL;

	if (FAILED(Read_File(reinterpret_cast<const _tchar*>(pArg), FIRST)))
		return E_FAIL;

	return S_OK;
}

//HRESULT CUI_Group_Skill::Create_Component(const _tchar* pFIlePath, const _tchar* wszTag, KEYLIST eType)
HRESULT CUI_Group_Skill::Create_Component(const _tchar* pFIlePath, wstring wszTag, KEYLIST eType)
{

	if (FAILED(Add_Components(wszTag)))
		return E_FAIL;

	if (FAILED(Read_File(pFIlePath, eType)))
		return E_FAIL;

	return S_OK;
}

void CUI_Group_Skill::Add_SpellProtoTypeTag()
{
	BEGININSTANCE;

	_tchar pName[MAX_PATH] = TEXT("");
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Accio_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Alohomora_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Alteration_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_ArrestoMomentum_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_AvadaKedavra_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Bombarda_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));

	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Confringo_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Conjuration_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Crucio_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Depulso_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Descendo_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Diffindo_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));

	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Disillusionment_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Evanesco_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Expelliarmus_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Flipendo_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Glacius_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Imperio_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));

	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Incendio_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Levioso_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Lumos_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Protego_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Reparo_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Revelio_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));

	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_Transformation_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_TransformationOverlandOverlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));
	lstrcpy(pName, TEXT("Prototype_Component_Texture_UI_T_spellmeter_WingardiumLeviosa_Overlay"));
	m_SpellProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));

	ENDINSTANCE
}

HRESULT CUI_Group_Skill::Add_SpellProtoType()
{
	BEGININSTANCE

	for (size_t i = 0; i < BASICCAST; i++)
	{
		CComponent* pComponenet = pGameInstance->Find_Prototype(LEVEL_CLIFFSIDE, m_SpellProtoTypeTags[i]);

		if (pComponenet == nullptr)
		{
			wstring strPath = L"Texture_";
			wstring strFullPath = m_SpellProtoTypeTags[i];

			if (strFullPath.find(strPath) != std::wstring::npos)
			{
				_tchar wszPath[MAX_PATH] = TEXT("");
				std::wstring result = strFullPath.substr(strFullPath.find(strPath) + strPath.length());
				wstring strFilePath = TEXT("../../Resources/UI/Game/UI/Icons/Spells/SpellMeters/");
				strFilePath = strFilePath + result + TEXT(".png");
				lstrcpy(wszPath, strFilePath.c_str());
				if (FAILED(pGameInstance->Add_Prototype(LEVEL_CLIFFSIDE, m_SpellProtoTypeTags[i],
					CTexture::Create(m_pDevice, m_pContext, wszPath))))
				{
					MSG_BOX("Failed Create Texture Component");
				}
			}
		}
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CUI_Group_Skill::Add_SpellTexture()
{
	BEGININSTANCE;

	_uint iSize = m_SpellProtoTypeTags.size();

	for (size_t i = 0; i < iSize; i++)
	{
		CTexture* pTexture = nullptr;

		if (FAILED(CComposite::Add_Component(LEVEL_CLIFFSIDE, m_SpellProtoTypeTags[i],
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&pTexture))))
		{
			MSG_BOX("Failed CUI_Image Add_Component : (Com_Texture)");
			return E_FAIL;
		}

		m_SkillTextures.push_back(pTexture);
	}

	ENDINSTANCE;

	return S_OK;
}

CUI_Group_Skill* CUI_Group_Skill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_Skill* pInstance = New CUI_Group_Skill(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_Skill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Group_Skill::Clone(void* pArg)
{
	CUI_Group_Skill* pInstance = New CUI_Group_Skill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Group_Skill");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CUI_Group_Skill::Free()
{
	__super::Free();

	for (auto& iter : m_pMains)
	{
		Safe_Release(iter);
	}
	m_pMains.clear();

	for (auto& iter : m_pFrames)
	{
		Safe_Release(iter);
	}
	m_pFrames.clear();

	for (auto& iter : m_pNumbers)
	{
		Safe_Release(iter);
	}
	m_pNumbers.clear();

	for (auto& iter : m_SkillTextures)
	{
		Safe_Release(iter);
	}
	m_SkillTextures.clear();

}

