#include "Magic.h"
#include "MagicBallPool.h"
#include "GameInstance.h"

CMagic::CMagic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

// 재생성될때 값 가지고 올 필요 없음.
// 클론할때 데이터 입력해줄거임.
CMagic::CMagic(const CMagic& rhs)
	: CComposite(rhs)
{
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
	m_eMagicGroup = InitDesc->eMagicGroup;
	m_eMagicType = InitDesc->eMagicType;
	m_eBuffType = InitDesc->eBuffType;
	m_fInitCoolTime = InitDesc->fCoolTime;
	m_fDamage = InitDesc->fDamage;
	m_fCastDistance = InitDesc->fCastDistance;
	m_fBallDistance = InitDesc->fBallDistance;
	m_eMagicTag = InitDesc->eMagicTag;
	m_fLifeTime = InitDesc->fLifeTime;

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

_bool CMagic::Magic_Cast(CTransform* pTarget, class CWeapon_Player_Wand* pWeapon)
{
	if (m_fCurrentCoolTime <= 0)
	{
		//마법을 생성 합니다.
		CMagicBall::MAGICBALLINITDESC ballInit;
		ballInit.eBuffType = m_eBuffType;
		ballInit.eMagicGroup = m_eMagicGroup;
		ballInit.eMagicTag = m_eMagicTag;
		ballInit.eMagicType = m_eMagicType;
		ballInit.fDamage = m_fDamage;
		ballInit.fDistance = m_fBallDistance;
		ballInit.pTarget = pTarget;
		ballInit.fLifeTime = m_fLifeTime;
		ballInit.pWeapon = pWeapon;

		BEGININSTANCE;
		if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Protego"), TEXT("Layer_Magic"), Generate_HashtagW().c_str(), &ballInit)))
		{
			MSG_BOX("Failed Add_GameObject : (GameObject_BaseAttack)");
			return false;
		}
		ENDINSTANCE;

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
	__super::Free();
}
