#include "UI_Group_Enemy_HP.h"
#include "GameInstance.h"
#include "UI_Effect_Back.h"
#include "UI_HP.h"
#include "UI_Font.h"

CUI_Group_Enemy_HP::CUI_Group_Enemy_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Group_Enemy_HP::CUI_Group_Enemy_HP(const CUI_Group_Enemy_HP& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Group_Enemy_HP::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CUI_Group_Enemy_HP Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Group_Enemy_HP::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		ENEMYHPDESC* hpDesc = static_cast<ENEMYHPDESC*>(pArg);

		eEnemyType = hpDesc->eType;
		lstrcpy(m_wszObjectLevel, hpDesc->wszObjectLevel);
		lstrcpy(m_wszObjectName, hpDesc->wszObjectName);
	}

	switch (eEnemyType)
	{
	case Client::CUI_Group_Enemy_HP::MONSTER:
		if (FAILED(Read_File(TEXT("../../Resources/GameData/UIData/UI_Group_Monster_HP.uidata"))))
			return E_FAIL;
		break;
	case Client::CUI_Group_Enemy_HP::BOSS:
		if (FAILED(Read_File(TEXT("../../Resources/GameData/UIData/UI_Group_Boss_HP.uidata"))))
			return E_FAIL;
		break;
	case Client::CUI_Group_Enemy_HP::ENEMYTYPE_END:
		break;
	default:
		break;
	}

	if (FAILED(Add_Fonts(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_Group_Enemy_HP::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Group_Enemy_HP::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Group_Enemy_HP::Add_Prototype()
{
	BEGININSTANCE;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Effect_Back"),
		CUI_Effect_Back::Create(m_pDevice, m_pContext), true)))
	{
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_HP"),
		CUI_HP::Create(m_pDevice, m_pContext), true)))
	{
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CUI_Group_Enemy_HP::Add_Components()
{
	BEGININSTANCE;

	if (FAILED(CComposite::Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Effect_Back"),
		TEXT("Com_UI_Enemy_HP_Frame"), reinterpret_cast<CComponent**>(&m_pUI_Effect_Back))))
	{
		MSG_BOX("CUI_Group_Enemy_HP : Failed Clone Component (Com_UI_MiniMap_Frame)");
		ENDINSTANCE;
		return E_FAIL;
	}

	CUI_HP::HPTYPE eType = CUI_HP::HPTYPE::MONSTER;
	if (FAILED(CComposite::Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_HP"),
		TEXT("Com_UI_Enemy_HP"), reinterpret_cast<CComponent**>(&m_pUI_HP), &eType)))
	{
		MSG_BOX("CUI_Group_Enemy_HP : Failed Clone Component (Com_UI_MiniMap_Player)");
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CUI_Group_Enemy_HP::Read_File(const _tchar* pFilePath)
{
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

	m_pUI_Effect_Back->Load(Load_File(hFile));


	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	m_pUI_HP->Load(Load_File(hFile));
	m_pUI_HP->Set_Parent(m_pUI_Effect_Back);

	CloseHandle(hFile);

	return S_OK;
}

CUI::UIDESC CUI_Group_Enemy_HP::Load_File(const HANDLE hFile)
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

	fs::path fsPath = UIDesc.szTexturePath;

	std::wstring wszExtension = fsPath.extension();
	if (wszExtension == TEXT(".png"))
	{
		fsPath.replace_extension(L".dds");
		lstrcpy(UIDesc.szTexturePath, fsPath.c_str());
	}

	return UIDesc;
}

HRESULT CUI_Group_Enemy_HP::Add_Fonts(void* pArg)
{
	BEGININSTANCE;
	
	CUI_Font* pName = nullptr;
	CUI_Font* pLevel = nullptr;

	CUI_Font::FONTDESC Desc;
	lstrcpy(Desc.m_pText, m_wszObjectLevel);
	if (eEnemyType == MONSTER)
	{
		Desc.m_vPos = { 511.f, 73.f };
		Desc.m_vColor = _float4(0.f, 1.f, 0.f, 1.f);
		Desc.m_fRotation = { 0.f };
		Desc.m_vOrigin = { 0.f, 0.f };
		Desc.m_vScale = { 0.4f, 0.4f };
	}
	else
	{
		Desc.m_vPos = { 435.f, 73.f };
		Desc.m_vColor = _float4(0.f, 1.f, 0.f, 1.f);
		Desc.m_fRotation = { 0.f };
		Desc.m_vOrigin = { 0.f, 0.f };
		Desc.m_vScale = { 0.4f, 0.4f };
	}

	if (FAILED(CComposite::Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Font"),
		TEXT("Com_UI_Font_Level"), reinterpret_cast<CComponent**>(&pLevel), &Desc)))
	{
		MSG_BOX("CUI_Group_Enemy_HP : Failed Clone Component (Com_UI_Font_Level)");
		ENDINSTANCE;
		return E_FAIL;
	}
	m_Fonts.push_back(pLevel);



	ZeroMemory(&Desc, sizeof(CUI_Font::FONTDESC));
	lstrcpy(Desc.m_pText, m_wszObjectName);
	Desc.m_vPos = { 640.f, 50.f };
	Desc.m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	Desc.m_fRotation = { 0.f };
	Desc.m_vOrigin = { 0.f, 0.f };
	Desc.m_vScale = { 0.5f, 0.5f };

	if (FAILED(CComposite::Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Font"),
		TEXT("Com_UI_Font_Name"), reinterpret_cast<CComponent**>(&pName), &Desc)))
	{
		MSG_BOX("CUI_Group_Enemy_HP : Failed Clone Component (Com_UI_Font_Name)");
		ENDINSTANCE;
		return E_FAIL;
	}
	m_Fonts.push_back(pName);


	ENDINSTANCE;

	return S_OK;
}

CUI_Group_Enemy_HP* CUI_Group_Enemy_HP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_Enemy_HP* pInstance = New CUI_Group_Enemy_HP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_Enemy_HP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Group_Enemy_HP::Clone(void* pArg)
{
	CUI_Group_Enemy_HP* pInstance = New CUI_Group_Enemy_HP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Group_Enemy_HP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Group_Enemy_HP::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Effect_Back);
	Safe_Release(m_pUI_HP);

	for (auto& pFonts : m_Fonts)
	{
		Safe_Release(pFonts);
	}
	m_Fonts.clear();
}