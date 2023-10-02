#include "AccPotion.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"
#include"CoolTime.h"

CAccPotion::CAccPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CAccPotion::CAccPotion(const CAccPotion& rhs)
	: CItem(rhs)
{
}

HRESULT CAccPotion::Initialize_Prototype(_uint iLevel, const _tchar* pUIImagePath)
{
	if (FAILED(__super::Initialize_Prototype(iLevel, pUIImagePath)))
		return E_FAIL;

	m_iLevel = iLevel;
	m_eItemType = ITEMTYPE::POTION;

	return S_OK;
}

HRESULT CAccPotion::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CAccPotion::Use(_float3 vPlayPos,void* ExtraValue)
{
	if (false == m_pCooltime->IsEnable())
		m_pCooltime->Set_Enable(true);

	if (true == m_pCooltime->IsEnable())
	{
		cout << "아이템 썼어요" << endl;
		
		m_pCooltime->Set_Time_Accel(true);

	}
}

HRESULT CAccPotion::Add_Components()
{
	CCoolTime::COOLTIME_DESC MaxCool;
	MaxCool.fMaxCoolTime = 15.f;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CoolTime"),
		TEXT("Com_CoolTime"), reinterpret_cast<CComponent**>(&m_pCooltime),&MaxCool)))
	{
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CAccPotion::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);



	Safe_Release(pGameInstance);
	return S_OK;
}

CAccPotion* CAccPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, const _tchar* pUIImagePath)
{
	CAccPotion* pInstance = New CAccPotion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel, pUIImagePath)))
	{
		MSG_BOX("Failed to Created CAccPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAccPotion::Clone(void* pArg)
{
	CAccPotion* pInstance = New CAccPotion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAccPotion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAccPotion::Free()
{
	__super::Free();

	Safe_Release(m_pCooltime);
}