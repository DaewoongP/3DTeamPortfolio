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
	// 프로토타입때 만들거 없습니다.
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CBaseAttack::Initialize(void* pArg)
{
	// 사본 생성시 pArg로 값을 던져줘야합니다.
	// 평타의 기본 값을 설정해줍니다.
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
	// 슈퍼틱에서 쿨이 돕니다.
	__super::Tick(fTimeDelta);
}

void CBaseAttack::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

_bool CBaseAttack::Action_Magic(CTransform* pTarget)
{
	// 부모를 호출(마법 발동시 true : false)
	if (__super::Action_Magic(pTarget))
	{
		// 마법 평타 오브제 생성
		// 마법 평타 오브제에게 값을 전달해줘야겠죠? (type, target)
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
