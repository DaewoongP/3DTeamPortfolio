#include "UI_Group_Skill.h"
#include "GameInstance.h"


#include "UI_Back.h"
#include "UI_Effect_Back.h"

CUI_Group_Skill::CUI_Group_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Group(pDevice, pContext)
{
}

CUI_Group_Skill::CUI_Group_Skill(const CUI_Group_Skill& rhs)
	: CUI_Group(rhs)
	, m_ProtoTypeTags(rhs.m_ProtoTypeTags)
	, m_SpellProtoTypeTags(rhs.m_SpellProtoTypeTags)
	, m_SkillTextures(rhs.m_SkillTextures)
{
}

HRESULT CUI_Group_Skill::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_ProtoType()))
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

	Load_Skill_1(pArg);
	Load_Skill_2();
	Load_Skill_3();
	Load_Skill_4();

	m_KeyList[FIRST] = BASICCAST;
	m_KeyList[SECOND] = BASICCAST;
	m_KeyList[THIRD] = BASICCAST;
	m_KeyList[FOURTH] = BASICCAST;


//	Set_SpellTexture(FIRST, REVELIO);

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

HRESULT CUI_Group_Skill::Add_ProtoType()
{
	BEGININSTANCE

		_tchar pName[MAX_PATH] = TEXT("");
	lstrcpy(pName, TEXT("Prototype_GameObject_UI_Back"));

	CComponent* pComponent = pGameInstance->Find_Prototype(LEVEL_MAINGAME, pName);

	if (nullptr == pComponent)
	{
		pGameInstance->Add_Prototype(LEVEL_MAINGAME, pName, CUI_Back::Create(m_pDevice, m_pContext));
	}
	m_ProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));


	lstrcpy(pName, TEXT("Prototype_GameObject_UI_Effect_Back"));

	pComponent = pGameInstance->Find_Prototype(LEVEL_MAINGAME, pName);

	if (nullptr == pComponent)
	{
		pGameInstance->Add_Prototype(LEVEL_MAINGAME, pName, CUI_Effect_Back::Create(m_pDevice, m_pContext));
	}
	m_ProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));

	ENDINSTANCE

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
		CComponent* pComponenet = pGameInstance->Find_Prototype(LEVEL_MAINGAME, m_SpellProtoTypeTags[i]);

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
				if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, m_SpellProtoTypeTags[i],
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

		if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, m_SpellProtoTypeTags[i],
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

void CUI_Group_Skill::Load_Skill_1(void* pArg)
{/*
	m_hFile = (HFILE)pArg;*/

	BEGININSTANCE

	_tchar wszGroupName[MAX_PATH] = TEXT("");
	DWORD dwStrByte;
	DWORD dwByte = 0;
	ReadFile(pArg, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(pArg, wszGroupName, dwStrByte, &dwByte, nullptr);

	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[1], TEXT("Layer_UI"),
		TEXT("GameObject_UI_Skill_Back_1"), pArg);

	CUI_Effect_Back* pMain = dynamic_cast<CUI_Effect_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Back_1")));
	
	m_pMains.push_back(pMain);
	Safe_AddRef(pMain);

	_uint iSize = 0;
	ReadFile(pArg, &iSize, sizeof(iSize), &dwByte, nullptr);
	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[0], TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Frame_1"), pArg);

	CUI_Back* pFrame = dynamic_cast<CUI_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Frame_1")));
	pFrame->Set_Parent(m_pMains[FIRST]);
	Safe_AddRef(m_pMains[FIRST]);
	m_pFrames.push_back(pFrame);
	Safe_AddRef(pFrame);

 	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[0], TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Number_1"), pArg);

	CUI_Back* pNumber = dynamic_cast<CUI_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Number_1")));
	pNumber->Set_Parent(m_pMains[FIRST], true);
	Safe_AddRef(m_pMains[FIRST]);
	m_pNumbers.push_back(pNumber);
	Safe_AddRef(pNumber);

	ENDINSTANCE;

	CloseHandle(pArg);
}

HRESULT CUI_Group_Skill::Load_Skill_2()
{
	_tchar pFilePath[MAX_PATH] = TEXT("../../Resources/GameData/UIData/UI_Group_Skill_2.uidata");
	_ulong dwByte = 0;
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed Load");
		CloseHandle(hFile);
		return E_FAIL;
	}

	BEGININSTANCE;

	_tchar wszGroupName[MAX_PATH] = TEXT("");
	DWORD dwStrByte;
	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, wszGroupName, dwStrByte, &dwByte, nullptr);

	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[1], TEXT("Layer_UI"),
		TEXT("GameObject_UI_Skill_Back_2"), hFile);

	CUI_Effect_Back* pMain = dynamic_cast<CUI_Effect_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Back_2")));

	m_pMains.push_back(pMain);
	Safe_AddRef(pMain);

	_uint iSize = 0;
	ReadFile(hFile, &iSize, sizeof(iSize), &dwByte, nullptr);
	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[0], TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Frame_2"), hFile);

	CUI_Back* pFrame = dynamic_cast<CUI_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Frame_2")));
	pFrame->Set_Parent(m_pMains[SECOND]);
	Safe_AddRef(m_pMains[SECOND]);
	m_pFrames.push_back(pFrame);
	Safe_AddRef(pFrame);

	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[0], TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Number_2"), hFile);

	CUI_Back* pNumber = dynamic_cast<CUI_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Number_2")));
	pNumber->Set_Parent(m_pMains[SECOND], true);
	Safe_AddRef(m_pMains[SECOND]);
	m_pNumbers.push_back(pNumber);
	Safe_AddRef(pNumber);

	ENDINSTANCE;

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CUI_Group_Skill::Load_Skill_3()
{
	_tchar pFilePath[MAX_PATH] = TEXT("../../Resources/GameData/UIData/UI_Group_Skill_3.uidata");
	_ulong dwByte = 0;
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed Load");
		CloseHandle(hFile);
		return E_FAIL;
	}

	BEGININSTANCE;

	_tchar wszGroupName[MAX_PATH] = TEXT("");
	DWORD dwStrByte;
	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, wszGroupName, dwStrByte, &dwByte, nullptr);

	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[1], TEXT("Layer_UI"),
		TEXT("GameObject_UI_Skill_Back_3"), hFile);

	CUI_Effect_Back* pMain = dynamic_cast<CUI_Effect_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Back_3")));

	m_pMains.push_back(pMain);
	Safe_AddRef(pMain);

	_uint iSize = 0;
	ReadFile(hFile, &iSize, sizeof(iSize), &dwByte, nullptr);
	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[0], TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Frame_3"), hFile);

	CUI_Back* pFrame = dynamic_cast<CUI_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Frame_3")));
	pFrame->Set_Parent(m_pMains[THIRD]);
	Safe_AddRef(m_pMains[THIRD]);
	m_pFrames.push_back(pFrame);
	Safe_AddRef(pFrame);

	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[0], TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Number_3"), hFile);

	CUI_Back* pNumber = dynamic_cast<CUI_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Number_3")));
	pNumber->Set_Parent(m_pMains[THIRD], true);
	Safe_AddRef(m_pMains[THIRD]);
	m_pNumbers.push_back(pNumber);
	Safe_AddRef(pNumber);

	ENDINSTANCE;

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CUI_Group_Skill::Load_Skill_4()
{
	_tchar pFilePath[MAX_PATH] = TEXT("../../Resources/GameData/UIData/UI_Group_Skill_4.uidata");
	_ulong dwByte = 0;
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed Load");
		CloseHandle(hFile);
		return E_FAIL;
	}

	BEGININSTANCE;

	_tchar wszGroupName[MAX_PATH] = TEXT("");
	DWORD dwStrByte;
	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, wszGroupName, dwStrByte, &dwByte, nullptr);

	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[1], TEXT("Layer_UI"),
		TEXT("GameObject_UI_Skill_Back_4"), hFile);

	CUI_Effect_Back* pMain = dynamic_cast<CUI_Effect_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Back_4")));

	m_pMains.push_back(pMain);
	Safe_AddRef(pMain);

	_uint iSize = 0;
	ReadFile(hFile, &iSize, sizeof(iSize), &dwByte, nullptr);
	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[0], TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Frame_4"), hFile);

	CUI_Back* pFrame = dynamic_cast<CUI_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Frame_4")));
	pFrame->Set_Parent(m_pMains[FOURTH]);
	Safe_AddRef(m_pMains[FOURTH]);
	m_pFrames.push_back(pFrame);
	Safe_AddRef(pFrame);

	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[0], TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Number_4"), hFile);

	CUI_Back* pNumber = dynamic_cast<CUI_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Skill_Number_4")));
	pNumber->Set_Parent(m_pMains[FOURTH], true);
	Safe_AddRef(m_pMains[FOURTH]);
	m_pNumbers.push_back(pNumber);
	Safe_AddRef(pNumber);

	ENDINSTANCE;

	return S_OK;
}


CUI_Group_Skill* CUI_Group_Skill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_Skill* pInstance = new CUI_Group_Skill(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_Skill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Group_Skill::Clone(void* pArg)
{
	CUI_Group_Skill* pInstance = new CUI_Group_Skill(*this);

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
	for (auto& iter : m_pFrames)
	{
		Safe_Release(iter);
	}
	for (auto& iter : m_pNumbers)
	{
		Safe_Release(iter);
	}
	for (auto& iter : m_SkillTextures)
	{
		Safe_Release(iter);
	}
}

