#include "UI_Group_Finisher_Icon.h"
#include "GameInstance.h"


#include "UI_Back.h"
#include "UI_HP.h"

CUI_Group_Finisher_Icon::CUI_Group_Finisher_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Group(pDevice, pContext)
{
}

CUI_Group_Finisher_Icon::CUI_Group_Finisher_Icon(const CUI_Group_Finisher_Icon& rhs)
	: CUI_Group(rhs)
	, m_ProtoTypeTags(rhs.m_ProtoTypeTags)
{
}

HRESULT CUI_Group_Finisher_Icon::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_ProtoType()))
	{
		MSG_BOX("Failed CUI_Group_Finisher_Icon Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Group_Finisher_Icon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	BEGININSTANCE

	_tchar wszGroupName[MAX_PATH] = TEXT("");
	DWORD dwStrByte;
	DWORD dwByte = 0;
	ReadFile(pArg, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(pArg, wszGroupName, dwStrByte, &dwByte, nullptr);

	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[0], TEXT("Layer_UI"),
		TEXT("GameObject_UI_Finisher_Icon"), pArg);

	CUI* pUI = dynamic_cast<CUI*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Finisher_Icon")));
	m_pParent = pUI;
	Safe_AddRef(m_pParent);
	pUI = nullptr;

	ENDINSTANCE;

	return S_OK;
}

void CUI_Group_Finisher_Icon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Group_Finisher_Icon::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CUI_Group_Finisher_Icon::Add_ProtoType()
{
	BEGININSTANCE;

	_tchar pName[MAX_PATH] = TEXT("");
	lstrcpy(pName, TEXT("Prototype_GameObject_UI_Back"));

	CComponent* pComponent = pGameInstance->Find_Prototype(LEVEL_MAINGAME, pName);

	if (nullptr == pComponent)
	{
		pGameInstance->Add_Prototype(LEVEL_MAINGAME, pName, CUI_Back::Create(m_pDevice, m_pContext));
	}

	m_ProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));

	ENDINSTANCE;

	return S_OK;
}

CUI_Group_Finisher_Icon* CUI_Group_Finisher_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_Finisher_Icon* pInstance = new CUI_Group_Finisher_Icon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_Finisher_Icon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Group_Finisher_Icon::Clone(void* pArg)
{
	CUI_Group_Finisher_Icon* pInstance = new CUI_Group_Finisher_Icon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Group_Finisher_Icon");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CUI_Group_Finisher_Icon::Free()
{
	__super::Free();
}

