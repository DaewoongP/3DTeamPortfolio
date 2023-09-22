#include "..\Public\Inventory.h"
#include "GameInstance.h"
#include "UI_Inventory.h"
#include "GameObject.h"

CInventory::CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CInventory::CInventory(const CInventory& rhs)
	: CGameObject(rhs)
{
}

HRESULT CInventory::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CInventory::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CInventory::Late_Tick(_float fTimeDelta)
{
	//button 클릭딱했어
	// 그러면 여기 인벤토리 클래스를 버튼눌렀다는걸 알려주는거임
	// 그러면 이친구가 UI를 렌더링하면서
	// 기능을 수행하는거임



}

HRESULT CInventory::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Inventory* pInventory = nullptr;

	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Inventory"),
		TEXT("Com_UI_Inventory"), reinterpret_cast<CComponent**>(&pInventory))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Text)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


	Safe_Release(pGameInstance);
	return S_OK;
}

void CInventory::Add_Item()
{
}

void CInventory::Delete_Item()
{
}

CInventory* CInventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CInventory::Clone(void* pArg)
{
	return nullptr;
}

void CInventory::Free()
{
}