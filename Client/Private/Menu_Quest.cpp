#include "Menu_Quest.h"
#include "GameInstance.h"

#include "Quest_Manager.h"

#include "UI_Effect_Back.h"
#include "UI_Back.h"

CMenu_Quest::CMenu_Quest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMenu_Quest::CMenu_Quest(const CMenu_Quest& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMenu_Quest::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CMenu_Quest Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMenu_Quest::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Offset();
	Ready_DefaultTexture();
	
	Add_Components(TEXT("First"), QUEST_FIG);
	Add_Components(TEXT("Second"), QUEST_POTION);
	Add_Components(TEXT("Third"), QUEST_TOWN);
	Add_Components(TEXT("Fourth"), QUEST_SECRET);
	Add_Components(TEXT("Fifth"), QUEST_BONE);

	return S_OK;
}

void CMenu_Quest::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_uint iIndex = 0;
	for (auto& pExplain : m_pExplainBack)
	{
		if (m_isCurrentQuest[iIndex])
			pExplain->Tick(fTimeDelta);
		++iIndex;
	}

	Quest_Check();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
	{
		for (auto& pExplain : m_pExplainBack)
		{
			if (pExplain->Get_Clicked())
			{
				if (pExplain->Switch_ImageShow())
					pExplain->CUI::Set_Texture(1);
				else
					pExplain->CUI::Set_Texture(0);
			}
		}
	}

	Safe_Release(pGameInstance);
}

void CMenu_Quest::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_uint iIndex = 0;
	for (auto& pExplain : m_pExplainBack)
	{
		if (m_isCurrentQuest[iIndex])
			pExplain->Late_Tick(fTimeDelta);
		++iIndex;
	}
}

HRESULT CMenu_Quest::Render()
{

	return S_OK;
}

void CMenu_Quest::Set_Open(_bool isOpen)
{
	m_isOpen = isOpen;
}

void CMenu_Quest::Set_FontState(QUESTLIST eQuest, QUESTSTATE eState)
{
	if (nullptr != m_pTexts[eQuest])
	{
		m_pTexts[eQuest]->Set_Texture(eState);
		m_pTexts[eQuest]->Set_XY(m_fOffset[eQuest].vPos);
		m_pTexts[eQuest]->Set_Size(m_fOffset[eQuest].vSize);

		if (eState == QUESTSTATE_UNLOCK)
			m_isCurrentQuest[eQuest] = true;
		else
			m_isCurrentQuest[eQuest] = false;
	}
}

void CMenu_Quest::Quest_Check()
{
	CQuest_Manager* pQuest_Manager = CQuest_Manager::GetInstance();
	Safe_AddRef(pQuest_Manager);

	CQuest* pQuest = pQuest_Manager->Find_Quest(TEXT("Quest_Save_Fig"));

	if (pQuest->Get_State() != QUESTSTATE_QUESTIONMARK)
		Set_FontState(QUEST_FIG, pQuest->Get_State());

	pQuest = pQuest_Manager->Find_Quest(TEXT("Quest_Potion"));

	if (pQuest->Get_State() != QUESTSTATE_QUESTIONMARK)
		Set_FontState(QUEST_POTION, pQuest->Get_State());

	pQuest = pQuest_Manager->Find_Quest(TEXT("Quest_Town"));

	if (pQuest->Get_State() != QUESTSTATE_QUESTIONMARK)
		Set_FontState(QUEST_POTION, pQuest->Get_State());

	pQuest = pQuest_Manager->Find_Quest(TEXT("Quest_Secret"));

	if (pQuest->Get_State() != QUESTSTATE_QUESTIONMARK)
		Set_FontState(QUEST_POTION, pQuest->Get_State());

	pQuest = pQuest_Manager->Find_Quest(TEXT("Quest_Bone"));

	if (pQuest->Get_State() != QUESTSTATE_QUESTIONMARK)
		Set_FontState(QUEST_POTION, pQuest->Get_State());

	Safe_Release(pQuest_Manager);
}

HRESULT CMenu_Quest::Add_Prototype()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		CUI_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		CUI_Effect_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMenu_Quest::Ready_DefaultTexture()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	wstring tag;
	CUI_Back* pBack = nullptr;

	tag = TEXT("Com_UI_Back_Quest_Frame") + Generate_HashtagW(true);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"), TEXT("Com_UI_Back_Quest_Frame"), reinterpret_cast<CComponent**>(&pBack))))
	{
		MSG_BOX("Com_Menu_Quest : Failed Clone Component (Com_UI_Back_Quest_Frame)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}
	m_pUIs.push_back(pBack);

	CUI::UIDESC UIDesc;
	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 640.f };
	UIDesc.fY = { 300.f };
	UIDesc.fZ = { 0.9f };
	UIDesc.fSizeX = { 1280.f };
	UIDesc.fSizeY = { 720.f };
	_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/UI_Edit/Quest_Frame_Capture.png");
	lstrcpy(UIDesc.szTexturePath, szTexturePath);
	pBack->Load(UIDesc);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CMenu_Quest::Add_Components(wstring wszTag, QUESTLIST eType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Back* pText = nullptr;

	wstring text = TEXT("Com_UI_Back_Text_");
	text += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		text.c_str(), reinterpret_cast<CComponent**>(&pText))))
	{
		MSG_BOX("Com_Menu_Quest : Failed Clone Component (Com_UI_Back_Main_)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	CUI::UIDESC UIDesc;
	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 280.f };
	UIDesc.fY = { 245.f + eType * 30.f };
	UIDesc.fZ = { 0.88f };
	UIDesc.fSizeX = { 74.f };
	UIDesc.fSizeY = { 44.f };
	_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/Quest/Quest_QuestionMark.png");
	lstrcpy(UIDesc.szTexturePath, szTexturePath);
	pText->Load(UIDesc);

	switch (eType)
	{
	case Client::QUEST_FIG:
	{
		pText->Add_Texture(TEXT("../../Resources/UI/Game/Quest/1_Fig_Font.png"));
		pText->Add_Texture(TEXT("../../Resources/UI/Game/Quest/1_Fig_Font_False.png"));
	}
		break;
	case Client::QUEST_POTION:
	{
		pText->Add_Texture(TEXT("../../Resources/UI/Game/Quest/2_PotionQuest_Font.png"));
		pText->Add_Texture(TEXT("../../Resources/UI/Game/Quest/2_PotionQuest_Font_False.png"));
	}
		break;
	case Client::QUEST_TOWN:
	{
		pText->Add_Texture(TEXT("../../Resources/UI/Game/Quest/3_Twon_Font.png"));
		pText->Add_Texture(TEXT("../../Resources/UI/Game/Quest/3_Twon_Font_False.png"));
	}
		break;
	case Client::QUEST_SECRET:
	{
		pText->Add_Texture(TEXT("../../Resources/UI/Game/Quest/4_Secret_Font.png"));
		pText->Add_Texture(TEXT("../../Resources/UI/Game/Quest/4_Secret_Font_False.png"));
	}
		break;
	case Client::QUEST_BONE:
	{
		pText->Add_Texture(TEXT("../../Resources/UI/Game/Quest/5_Boss_Font.png"));
		pText->Add_Texture(TEXT("../../Resources/UI/Game/Quest/5_Boss_Font_False.png"));
	}
		break;
	case Client::QUEST_END:
		break;
	default:
		break;
	}

	m_pTexts.push_back(pText);

	CUI_Effect_Back* pExplain = nullptr;
	pExplain = static_cast<CUI_Effect_Back*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back")));

	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 410.f };
	UIDesc.fY = { 430.f };
	UIDesc.fZ = { 0.88f };
	UIDesc.fSizeX = { 450.f };
	UIDesc.fSizeY = { 64.f };
	switch (eType)
	{
	case Client::QUEST_FIG:
	{
		_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/Quest/Fig_Explain.png");
		lstrcpy(UIDesc.szTexturePath, szTexturePath);
		pExplain->Load(UIDesc);
		pExplain->Add_Texture(TEXT("../../Resources/UI/Game/Quest/Fig_Explain_Click.png"));

		m_pExplainBack.push_back(pExplain);
	}
	break;
	case Client::QUEST_POTION:
	{
		_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/Quest/PotionQuest_Explain.png");
		lstrcpy(UIDesc.szTexturePath, szTexturePath);
		pExplain->Load(UIDesc);
		pExplain->Add_Texture(TEXT("../../Resources/UI/Game/Quest/PotionQuest_Explain_Click.png"));

		m_pExplainBack.push_back(pExplain);
	}
	break;
	case Client::QUEST_TOWN:
	{
		_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/Quest/Town_Explain.png");
		lstrcpy(UIDesc.szTexturePath, szTexturePath);
		pExplain->Load(UIDesc);
		pExplain->Add_Texture(TEXT("../../Resources/UI/Game/Quest/Town_Explain_Click.png"));

		m_pExplainBack.push_back(pExplain);
	}
	break;
	case Client::QUEST_SECRET:
	{
		_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/Quest/Secret_Explain.png");
		lstrcpy(UIDesc.szTexturePath, szTexturePath);
		pExplain->Load(UIDesc);
		pExplain->Add_Texture(TEXT("../../Resources/UI/Game/Quest/Secret_Explain_Click.png"));

		m_pExplainBack.push_back(pExplain);
	}
	break;
	case Client::QUEST_BONE:
	{
		_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/Quest/Boss_Explain.png");
		lstrcpy(UIDesc.szTexturePath, szTexturePath);
		pExplain->Load(UIDesc);
		pExplain->Add_Texture(TEXT("../../Resources/UI/Game/Quest/Boss_Explain_Click.png"));

		m_pExplainBack.push_back(pExplain);
	}
	break;
	case Client::QUEST_END:
		break;
	default:
		break;
	}

	CUI_Image::IMAGEDESC ImageDesc;
	ImageDesc.vCombinedXY = m_pExplainBack[eType]->Get_CombinedXY();
	ImageDesc.fX = 940.f;
	ImageDesc.fY = 260.f;
	ImageDesc.fZ = 0.86f;
	ImageDesc.fSizeX = 512.f;
	ImageDesc.fSizeY = 256.f;
	m_pExplainBack[eType]->Set_ImageCom(ImageDesc);
	m_pExplainBack[eType]->Set_ImageComShader(CUI_Image::CLICK);

	switch (eType)
	{
	case Client::QUEST_FIG:
		m_pTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/Quest/1_Fig.png")));
		break;
	case Client::QUEST_POTION:
		m_pTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/Quest/2_PotionQuest.png")));
		break;
	case Client::QUEST_TOWN:
		m_pTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/Quest/3_Twon.png")));
		break;
	case Client::QUEST_SECRET:
		m_pTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/Quest/4_Secret.png")));
		break;
	case Client::QUEST_BONE:
		m_pTextures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/Quest/5_Boss.png")));
		break;
	case Client::QUEST_END:
		break;
	default:
		break;
	}
	m_pExplainBack[eType]->Set_Texture(m_pTextures[eType]);


	Safe_Release(pGameInstance);
	return S_OK;
}


HRESULT CMenu_Quest::Ready_Offset()
{
	OFFSETDEST Desc;

	Desc.vPos = _float2(350.f, 245.f);
	Desc.vSize = _float2(211.f, 45.f);
	m_fOffset.push_back(Desc);

	Desc.vPos = _float2(318.f, 275.f);
	Desc.vSize = _float2(145.f, 45.f);
	m_fOffset.push_back(Desc);

	Desc.vPos = _float2(327.f, 305.f);
	Desc.vSize = _float2(165.f, 45.f);
	m_fOffset.push_back(Desc);

	Desc.vPos = _float2(339.f, 335.f);
	Desc.vSize = _float2(191.f, 45.f);
	m_fOffset.push_back(Desc);

	Desc.vPos = _float2(306.f, 365.f);
	Desc.vSize = _float2(127.f, 45.f);
	m_fOffset.push_back(Desc);

	return S_OK;
}

CMenu_Quest* CMenu_Quest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMenu_Quest* pInstance = new CMenu_Quest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMenu_Quest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMenu_Quest::Clone(void* pArg)
{
	CMenu_Quest* pInstance = new CMenu_Quest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMenu_Quest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMenu_Quest::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);

	for (auto& pText : m_pTexts)
	{
		Safe_Release(pText);
	}

	for (auto& pUI : m_pUIs)
	{
		Safe_Release(pUI);
	}

	for (auto& pExplain : m_pExplainBack)
	{
		Safe_Release(pExplain);
	}

	for (auto& pTexture : m_pTextures)
	{
		Safe_Release(pTexture);
	}
}
