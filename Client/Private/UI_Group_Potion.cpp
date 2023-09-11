#include "UI_Group_Potion.h"
#include "GameInstance.h"


#include "UI_Back.h"
#include "UI_HP.h"

CUI_Group_Potion::CUI_Group_Potion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Group(pDevice, pContext)
{
}

CUI_Group_Potion::CUI_Group_Potion(const CUI_Group_Potion& rhs)
	: CUI_Group(rhs)
	, m_ProtoTypeTags(rhs.m_ProtoTypeTags)
{
}

HRESULT CUI_Group_Potion::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_ProtoType()))
	{
		MSG_BOX("Failed CUI_Group_Potion Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Group_Potion::Initialize(void* pArg)
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
		TEXT("GameObject_UI_Potion"), pArg);

	CUI* pUI = dynamic_cast<CUI*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_HP_Potion")));
	m_pParent = pUI;
	Safe_AddRef(m_pParent);
	pUI = nullptr;


	ENDINSTANCE;

	return S_OK;
}

void CUI_Group_Potion::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Group_Potion::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CUI_Group_Potion::Add_ProtoType()
{
	BEGININSTANCE

		_tchar pName[MAX_PATH] = TEXT("");
	lstrcpy(pName, TEXT("Prototype_GameObject_UI_Back"));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, pName,
		CUI_Back::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Create Prototype_GameObject_UI_Back");
	}
	m_ProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));

	ENDINSTANCE

		return S_OK;
}

CUI_Group_Potion* CUI_Group_Potion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_Potion* pInstance = new CUI_Group_Potion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_Potion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Group_Potion::Clone(void* pArg)
{
	CUI_Group_Potion* pInstance = new CUI_Group_Potion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Group_Potion");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CUI_Group_Potion::Free()
{
	__super::Free();
}

