#include "Magic.h"
#include "MagicBallPool.h"
#include "GameInstance.h"

CMagic::CMagic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CMagic::CMagic(const CMagic& rhs)
	: CComposite(rhs)
	, m_eMagicGroup(rhs.m_eMagicGroup)
	, m_eMagicType(rhs.m_eMagicType)
	, m_eBuffType(rhs.m_eBuffType)
	, m_fInitCoolTime(rhs.m_fInitCoolTime)
	, m_fDamage(rhs.m_fDamage)
	, m_fDistance(rhs.m_fDistance)
	, m_eMagicTag(rhs.m_eMagicTag)
{
	for (auto func : rhs.m_ActionVec)
	{
		m_ActionVec.push_back(func);
	}
}

HRESULT CMagic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMagic::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	MAGICDESC* InitDesc = static_cast<MAGICDESC*>(pArg);
	m_eMagicGroup	= InitDesc->eMagicGroup;
	m_eMagicType	= InitDesc->eMagicType;
	m_eBuffType		= InitDesc->eBuffType;
	m_fInitCoolTime = InitDesc->fCoolTime;
	m_fDamage		= InitDesc->fDamage;
	m_fDistance		= InitDesc->fDistance;
	m_eMagicTag		= InitDesc->eMagicTag;

	if (FAILED(Add_Component()))
	{
		MSG_BOX("Failed to Add Component Magic");
	}

	return S_OK;
}

void CMagic::Tick(_float fTimeDelta)
{
	if (m_fCurrentCoolTime > 0)
	{
		m_fCurrentCoolTime -= fTimeDelta;
	}

	__super::Tick(fTimeDelta);
}

void CMagic::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

_bool CMagic::Magic_Cast(CTransform* pTarget)
{
	if (m_fCurrentCoolTime <= 0)
	{
		//마법을 생성 합니다.
		
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BaseAttack"), TEXT("Layer_Debug"), TEXT("GameObject_BaseAttack"))))
		{
			MSG_BOX("Failed Add_GameObject : (GameObject_BaseAttack)");
			return E_FAIL;
		}

		Safe_Release(pGameInstance);
		//마법 발사시 같이 호출해줬으면 하는 함수들 행동 개시
		for (_uint i = 0; i < m_ActionVec.size(); i++)
		{
			
			m_ActionVec[i]();
		}
		m_fCurrentCoolTime = m_fInitCoolTime;
		return true;
	}
	return false;
}

HRESULT CMagic::Add_ActionFunc(function<void()> func)
{
	m_ActionVec.push_back(func);
	return S_OK;
}

HRESULT CMagic::Add_Component()
{
	/*if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MagicBallPool"),
		TEXT("Com_Pool"), reinterpret_cast<CComponent**>(&m_pMagicBallPool))))
	{
		MSG_BOX("Failed CMagic Add_Component : (Com_Pool)");
		return E_FAIL;
	}*/

	return S_OK;
}

CMagic* CMagic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMagic* pInstance = new CMagic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMagic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMagic* CMagic::Clone(void* pArg)
{
	CMagic* pInstance = new CMagic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMagic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMagic::Free()
{
	//Safe_Release(m_pMagicBallPool);
	__super::Free();
}
