#include "UI_Group_T.h"
#include "GameInstance.h"


#include "UI_Back.h"
#include "UI_HP.h"

CUI_Group_T::CUI_Group_T(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Group_T::CUI_Group_T(const CUI_Group_T& rhs)
	: CGameObject(rhs)
	, m_ProtoTypeTags(rhs.m_ProtoTypeTags)
{
}

HRESULT CUI_Group_T::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_ProtoType()))
	{
		MSG_BOX("Failed CUI_Group_T Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Group_T::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	BEGININSTANCE

		_tchar wszGroupName[MAX_PATH] = TEXT("");
	DWORD dwStrByte;
	DWORD dwByte = 0;
	ReadFile(pArg, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(pArg, wszGroupName, dwStrByte, &dwByte, nullptr);

	pGameInstance->Add_Component(LEVEL_CLIFFSIDE, m_ProtoTypeTags[0], TEXT("Layer_UI"),
		TEXT("GameObject_UI_Potion"), pArg);

	CUI* pUI = dynamic_cast<CUI*>(pGameInstance->Find_Component_In_Layer(LEVEL_CLIFFSIDE, TEXT("Layer_UI"), TEXT("GameObject_UI_HP_Potion")));
	//m_pParent = pUI;
	//Safe_AddRef(m_pParent);
	pUI = nullptr;


	ENDINSTANCE;

	return S_OK;
}

void CUI_Group_T::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Group_T::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CUI_Group_T::Add_ProtoType()
{
	BEGININSTANCE

		_tchar pName[MAX_PATH] = TEXT("");
	lstrcpy(pName, TEXT("Prototype_GameObject_UI_Back"));

	CComponent* pComponent = pGameInstance->Find_Prototype(LEVEL_CLIFFSIDE, pName);

	if (nullptr == pComponent)
	{
		pGameInstance->Add_Prototype(LEVEL_CLIFFSIDE, pName, CUI_Back::Create(m_pDevice, m_pContext));
	}

	m_ProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));

	ENDINSTANCE

		return S_OK;
}

CUI_Group_T* CUI_Group_T::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_T* pInstance = New CUI_Group_T(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_T");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Group_T::Clone(void* pArg)
{
	CUI_Group_T* pInstance = New CUI_Group_T(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Group_T");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CUI_Group_T::Free()
{
	__super::Free();
}

