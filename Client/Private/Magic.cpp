#include "Magic.h"

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
{
	lstrcpy(m_wszMagicName, rhs.m_wszMagicName);
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
	lstrcpy(m_wszMagicName, InitDesc->wszMagicName);
	m_eMagicGroup	= InitDesc->eMagicGroup;
	m_eMagicType	= InitDesc->eMagicType;
	m_eBuffType		= InitDesc->eBuffType;
	m_fInitCoolTime = InitDesc->fCoolTime;
	m_fDamage		= InitDesc->fDamage;
	m_fDistance		= InitDesc->fDistance;
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

_bool CMagic::Action_Magic(CTransform* pTarget)
{
	if (m_fCurrentCoolTime <= 0)
	{
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

void CMagic::Free()
{
	__super::Free();
}
