#include "BaseAttack.h"
#include "GameInstance.h"

CBaseAttack::CBaseAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMagic(pDevice, pContext)
{
}

CBaseAttack::CBaseAttack(const CBaseAttack& rhs)
	: CMagic(rhs)
{
}

HRESULT CBaseAttack::Initialize_Prototype()
{
	// ������Ÿ�Զ� ����� �����ϴ�.
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CBaseAttack::Initialize(void* pArg)
{
	// �纻 ������ pArg�� ���� ��������մϴ�.
	// ��Ÿ�� �⺻ ���� �������ݴϴ�.
	MAGICDESC*  InitDesc =static_cast<MAGICDESC*>(pArg);
	InitDesc->eBuffType = BUFF_NONE;
	InitDesc->eMagicGroup = MG_ESSENTIAL;
	InitDesc->eMagicType = MT_NOTHING;

	if (FAILED(__super::Initialize(InitDesc)))
		return E_FAIL;
	return S_OK;
}

void CBaseAttack::Tick(_float fTimeDelta)
{
	// ����ƽ���� ���� ���ϴ�.
	__super::Tick(fTimeDelta);
}

void CBaseAttack::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

_bool CBaseAttack::Action_Magic(CTransform* pTarget)
{
	// �θ� ȣ��(���� �ߵ��� true : false)
	if (__super::Action_Magic(pTarget))
	{
		// ���� ��Ÿ ������ ����
		// ���� ��Ÿ ���������� ���� ��������߰���? (type, target)
		return true;
	}
	return false;
}

CBaseAttack* CBaseAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBaseAttack* pInstance = new CBaseAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBaseAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBaseAttack* CBaseAttack::Clone(void* pArg)
{
	CBaseAttack* pInstance = new CBaseAttack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBaseAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBaseAttack::Free()
{
	__super::Free();
}
