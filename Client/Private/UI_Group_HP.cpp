#include "UI_Group_HP.h"
#include "GameInstance.h"


#include "UI_Back.h"
#include "UI_Progress.h"

CUI_Group_HP::CUI_Group_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Group(pDevice, pContext)
{
}

CUI_Group_HP::CUI_Group_HP(const CUI_Group_HP& rhs)
	: CUI_Group(rhs)
{
}

HRESULT CUI_Group_HP::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_ProtoType()))
	{
		MSG_BOX("Failed CUI_Group_HP Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

// UI_Group_HP 를 클론할때 hFile을 던진다.
// 
// 
// h파일 pArg로 받아오면됨.
HRESULT CUI_Group_HP::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/*BEGININSTANCE

	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[0], TEXT("Layer_UI"), TEXT(""), pArg);

	CUI* pUI = dynamic_cast<CUI*>(pGameInstance->Clone_Component(LEVEL_MAINGAME, m_ProtoTypeTags[0], pArg));
	m_pParent = pUI;
	Safe_AddRef(m_pParent);

	pUI = nullptr;
	pUI = dynamic_cast<CUI*>(pGameInstance->Clone_Component(LEVEL_MAINGAME, m_ProtoTypeTags[1], pArg));
	pUI->Set_Parent(m_pParent);
	Safe_AddRef(m_pParent);

	m_Childs.push_back(pUI);
	Safe_AddRef(pUI);

	ENDINSTANCE;*/
	// parent -> Prototypetag / level
	//__super::Load(TEXT("../../Resources/UIDATA/"), vector<pair<Prototypetag / level>>, Prototypetag / level);

	return S_OK;
}

void CUI_Group_HP::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Group_HP::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CUI_Group_HP::Add_ProtoType()
{
	BEGININSTANCE

	_tchar* pName = new _tchar[MAX_PATH];
	lstrcpy(pName, TEXT("Prototype_GameObject_UI_Back"));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, pName,
		CUI_Back::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Create Prototype_GameObject_UI_Back");
	}
	m_ProtoTypeTags.push_back(pName);
	pName = nullptr;

	pName = new _tchar[MAX_PATH];
	lstrcpy(pName, TEXT("Prototype_GameObject_UI_Progress"));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, pName,
		CUI_Progress::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Create Prototype_GameObject_UI_Progress");
	}

	m_ProtoTypeTags.push_back(pName);

	ENDINSTANCE

	return S_OK;
}

CUI_Group_HP* CUI_Group_HP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_HP* pInstance = new CUI_Group_HP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_HP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Group_HP::Clone(void* pArg)
{
	CUI_Group_HP* pInstance = new CUI_Group_HP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Group_HP");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CUI_Group_HP::Free()
{
	__super::Free();
	
	for (auto& iter : m_ProtoTypeTags)
	{
		Safe_Delete_Array(iter);
	}
}

