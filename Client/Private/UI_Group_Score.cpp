#include "UI_Group_Score.h"
#include "GameInstance.h"
#include "UI_Back.h"
#include "UI_Font.h"

bool ZComp(CUI_Group_Score::UISCORE& a, CUI_Group_Score::UISCORE& b) {
	return (*a.fScore > *b.fScore);
}

CUI_Group_Score::CUI_Group_Score(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Group_Score::CUI_Group_Score(const CUI_Group_Score& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Group_Score::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CUI_Group_Score Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Group_Score::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Offset()))
		return E_FAIL;

	m_UIScores.clear();

	if (nullptr != pArg)
	{
		UISCOREDESC* pDesc = (UISCOREDESC*)pArg;

		Create_UI_Score(pDesc->wszFirstName, &Text1, FIRST);
		Create_UI_Score(pDesc->wszSecondName, &Text2, SECOND);
		Create_UI_Score(pDesc->wszThirdName, &Text3, THIRD);
		Create_UI_Score(pDesc->wszFourthName, &Text4, FOURTH);
		Create_UI_Score(pDesc->wszFifthName, &Text5, FIFTH);
		Create_UI_Score(pDesc->wszSixthName, &Text6, SIXTH);
		Create_UI_Score(pDesc->wszSeventhName, &Text7, SEVENTH);
		Create_UI_Score(pDesc->wszEighthName, &Text8, EIGHTH);
		Create_UI_Score(pDesc->wszEighthName, &Text9, NINTH);
	}

	return S_OK;
}

void CUI_Group_Score::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (pGameInstance->Get_DIKeyState(DIK_L, CInput_Device::KEY_DOWN))
	{
		Text1 = rand() % 10;
		Text2 = rand() % 10;
		Text3 = rand() % 10;
		Text4 = rand() % 10;
		Text5 = rand() % 10;
		Text6 = rand() % 10;
		Text7 = rand() % 10;
		Text8 = rand() % 10;
		Text9 = rand() % 10;
	}
	Safe_Release(pGameInstance);

	_uint iIndex = 0;
	for (auto& pScore : m_UIScores)
	{
		m_UIScores[iIndex].pScore->Set_Text(to_wstring(*pScore.fScore));
		++iIndex;
	}

	Update_Score(fTimeDelta);


}

void CUI_Group_Score::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}


HRESULT CUI_Group_Score::Create_UI_Score(_tchar* pFontFont, _uint* fScore, UISCOREENUM eType)
{
	UISCORE score;
	Add_Components(pFontFont, &score, eType);
	Add_Font(pFontFont, &score);
	score.fScore = fScore;
	m_UIScores.push_back(score);

	return S_OK;
}

HRESULT CUI_Group_Score::Add_Prototype()
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

HRESULT CUI_Group_Score::Add_Components(_tchar* pFontFont, UISCORE* pUI, UISCOREENUM eType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Back* pBack = nullptr;
	wstring back = TEXT("Com_UI_Back_");
	back += pFontFont;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		back.c_str(), reinterpret_cast<CComponent**>(&pBack))))
	{
		MSG_BOX("Com_CUI_Score : Failed Clone Component (Com_UI_Back)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	CUI::UIDESC UIDesc;
	switch (eType)
	{
	case Client::CUI_Group_Score::FIRST:
		lstrcpy(UIDesc.szTexturePath, TEXT("../../Resources/UI/Game/UI/Icons/Activities/UI_T_HUD_AccioScore_BG_Black.png"));
		UIDesc.fX = { 70.f };
		UIDesc.fY = { 160.f };
		break;
	case Client::CUI_Group_Score::SECOND:
		lstrcpy(UIDesc.szTexturePath, TEXT("../../Resources/UI/Game/UI/Icons/Activities/UI_T_HUD_AccioScore_BG_Blue.png"));
		UIDesc.fX = { 70.f };
		UIDesc.fY = { 230.f };
		break;
	case Client::CUI_Group_Score::THIRD:
		lstrcpy(UIDesc.szTexturePath, TEXT("../../Resources/UI/Game/UI/Icons/Activities/UI_T_HUD_AccioScore_BG_Green.png"));
		UIDesc.fX = { 70.f };
		UIDesc.fY = { 300.f };
		break;
	case Client::CUI_Group_Score::FOURTH:
		lstrcpy(UIDesc.szTexturePath, TEXT("../../Resources/UI/Game/UI/Icons/Activities/UI_T_HUD_AccioScore_BG_Red.png"));
		UIDesc.fX = { 70.f };
		UIDesc.fY = { 370.f };
		break;
	case Client::CUI_Group_Score::FIFTH:
		lstrcpy(UIDesc.szTexturePath, TEXT("../../Resources/UI/Game/UI/Icons/Activities/UI_T_HUD_AccioScore_BG_Yellow.png"));
		UIDesc.fX = { 70.f };
		UIDesc.fY = { 440.f };
		break;
	case Client::CUI_Group_Score::SIXTH:
		lstrcpy(UIDesc.szTexturePath, TEXT("../../Resources/UI/Game/UI/Icons/Activities/UI_T_HUD_AccioScore_BG_Gray.png"));
		UIDesc.fX = { 70.f };
		UIDesc.fY = { 440.f };
		break;
	case Client::CUI_Group_Score::SEVENTH:
		lstrcpy(UIDesc.szTexturePath, TEXT("../../Resources/UI/Game/UI/Icons/Activities/UI_T_HUD_AccioScore_BG_HotPink.png"));
		UIDesc.fX = { 70.f };
		UIDesc.fY = { 440.f };
		break;
	case Client::CUI_Group_Score::EIGHTH:
		lstrcpy(UIDesc.szTexturePath, TEXT("../../Resources/UI/Game/UI/Icons/Activities/UI_T_HUD_AccioScore_BG_Laim.png"));
		UIDesc.fX = { 70.f };
		UIDesc.fY = { 440.f };
		break;
	case Client::CUI_Group_Score::NINTH:
		lstrcpy(UIDesc.szTexturePath, TEXT("../../Resources/UI/Game/UI/Icons/Activities/UI_T_HUD_AccioScore_BG_TerQuaiz.png"));
		UIDesc.fX = { 70.f };
		UIDesc.fY = { 440.f };
		break;
	case Client::CUI_Group_Score::UISCORE_END:
		break;
	default:
		break;
	}

	UIDesc.fZ = { 0.8f };
	UIDesc.fSizeX = { 128.f };
	UIDesc.fSizeY = { 64.f };

	pBack->Load(UIDesc);
	pUI->pBack = pBack;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CUI_Group_Score::Add_Font(const _tchar* pFontFont, UISCORE* pUI)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Font* pName = nullptr;

	CUI_Font::FONTDESC Desc;
	ZeroMemory(&Desc, sizeof(CUI_Font::FONTDESC));
	lstrcpy(Desc.m_pText, pFontFont);
	Desc.m_vPos = { -53.f, -20.f };
	Desc.m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	Desc.m_fRotation = { 0.f };
	Desc.m_vOrigin = { 0.f, 0.f };
	Desc.m_vScale = { 0.4f, 0.4f };
	Desc.m_pParent = pUI->pBack;
	wstring Name = TEXT("Com_UI_Font_Name_");
	Name += pFontFont;;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"),
		Name.c_str(), reinterpret_cast<CComponent**>(&pUI->pName), &Desc)))
	{
		MSG_BOX("CUI_Group_Score : Failed Clone Component (Com_UI_Font_Name)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	CUI_Font* pScore = nullptr;
	ZeroMemory(&Desc, sizeof(CUI_Font::FONTDESC));
	lstrcpy(Desc.m_pText, TEXT("0"));
	Desc.m_vPos = { -53.f,  3.f };
	Desc.m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	Desc.m_fRotation = { 0.f };
	Desc.m_vOrigin = { 0.f, 0.f };
	Desc.m_vScale = { 0.4f, 0.4f };
	Desc.m_pParent = pUI->pBack;
	wstring Score = TEXT("Com_UI_Font_Score_");
	Score += pFontFont;;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"),
		Score.c_str(), reinterpret_cast<CComponent**>(&pUI->pScore), &Desc)))
	{
		MSG_BOX("CUI_Group_Score : Failed Clone Component (Com_UI_Font_Score)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CUI_Group_Score::Ready_Offset()
{
	m_vOffset.clear();

	_float2 vPos = _float2(1180.f, 90.f);
	m_vOffset.push_back(vPos);

	vPos = _float2(1180.f, 150.f);
	m_vOffset.push_back(vPos);

	vPos = _float2(1180.f, 210.f);
	m_vOffset.push_back(vPos);

	vPos = _float2(1180.f, 270.f);
	m_vOffset.push_back(vPos);

	vPos = _float2(1180.f, 330.f);
	m_vOffset.push_back(vPos);

	vPos = _float2(1180.f, 390.f);
	m_vOffset.push_back(vPos);

	vPos = _float2(1180.f, 450.f);
	m_vOffset.push_back(vPos);

	vPos = _float2(1180.f, 510.f);
	m_vOffset.push_back(vPos);

	vPos = _float2(1180.f, 570.f);
	m_vOffset.push_back(vPos);

	return S_OK;
}

void CUI_Group_Score::Update_Score(_float fTimeDelta)
{
	std::sort(m_UIScores.begin(), m_UIScores.end(), ZComp);

	_uint iIndex = 0;
	for (auto& pScore : m_UIScores)
	{
		pScore.pBack->Set_XY(XMVectorLerp(pScore.pBack->Get_XY(), m_vOffset[iIndex], fTimeDelta));
		++iIndex;
	}
}

CUI_Group_Score* CUI_Group_Score::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_Score* pInstance = New CUI_Group_Score(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_Score");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Group_Score::Clone(void* pArg)
{
	CUI_Group_Score* pInstance = New CUI_Group_Score(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Group_Score");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Group_Score::Free()
{
	__super::Free();
	for (auto& pScore : m_UIScores)
	{
		Safe_Release(pScore.pBack);
		Safe_Release(pScore.pName);
		Safe_Release(pScore.pScore);
	}

	//Safe_Release(m_pUI_Alarm);
	//Safe_Release(m_pUI_Time);
}