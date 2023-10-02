//#include "EndurusPotion.h"
//#include"GameInstance.h"
//#include"Player.h"
//#include"Player_Information.h"  
//#include"CoolTime.h"
//#include"Defence.h"
//
//CEndurusPotion::CEndurusPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :CItem(pDevice, pContext)
//{
//}
//
//CEndurusPotion::CEndurusPotion(const CEndurusPotion& rhs) : CItem(rhs)
//{
//}
//
//HRESULT CEndurusPotion::Initialize_Prototype(_uint iLevel, const _tchar* pUIImagePath)
//{
//	if (FAILED(__super::Initialize_Prototype(iLevel, pUIImagePath)))
//		return E_FAIL;
//
//	m_iLevel = iLevel;
//	m_eItemType = ITEMTYPE::POTION;
//
//	return S_OK;
//}
//
//HRESULT CEndurusPotion::Initialize(void* pArg)
//{
//	if (FAILED(__super::Initialize(pArg)))
//		return E_FAIL;
//	m_iDefValue = *static_cast<_int*>(pArg);
//
//
//
//	if (FAILED(Add_Components()))
//		return E_FAIL;
//
//	return S_OK;
//}
//
////void CEndurusPotion::Tick(_float fTimeDelta)
////{
////}
//
//void CEndurusPotion::Use(_float3 vPlayPos,void* ExtraValue)
//{
//	
//	cout << "물약사용" << endl;
//
//	cout << m_iDefValue << endl;
//
//	
//
//	//m_pDefence->Set_PotionDefence(15);
//
//}
//
//HRESULT CEndurusPotion::Add_Components()
//{
//
//	CCoolTime::COOLTIME_DESC MaxCool;
//	MaxCool.fMaxCoolTime = 20.f;
//
//	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CoolTime"),
//		TEXT("Com_CoolTime"), reinterpret_cast<CComponent**>(&m_pCooltime), &MaxCool)))
//	{
//		__debugbreak();
//		return E_FAIL;
//	}
//
//	CDefence::DEFFENCEDESC DefValue;
//	DefValue.iDeffence = 0;
//
//	//if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Defence"),
//	//	TEXT("Com_Defence"), reinterpret_cast<CComponent**>(&m_pDefence),&DefValue)))
//	//{
//	//	__debugbreak();
//	//	return E_FAIL;
//	//}
//
//
//	return S_OK;
//}
//
//HRESULT CEndurusPotion::SetUp_ShaderResources()
//{
//	return S_OK;
//}
//
//CEndurusPotion* CEndurusPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, const _tchar* pUIImagePath)
//{
//	CEndurusPotion* pInstance = New CEndurusPotion(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype(iLevel, pUIImagePath)))
//	{
//		MSG_BOX("Failed to Created CAccPotion");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//CGameObject* CEndurusPotion::Clone(void* pArg)
//{
//	CEndurusPotion* pInstance = New CEndurusPotion(*this);
//
//	if (FAILED(pInstance->Initialize(pArg)))
//	{
//		MSG_BOX("Failed to Cloned CAccPotion");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//void CEndurusPotion::Free(void)
//{
//	__super::Free();
//
//	Safe_Release(m_pCooltime);
//	//Safe_Release(m_pDefence);
//}
