#include "..\Public\Trigger.h"
#include "GameInstance.h"

CTrigger::CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTrigger::CTrigger(const CTrigger& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTrigger::Initialize(void* pArg)
{
	NULL_CHECK_RETURN(pArg, E_FAIL);

	if (FAILED(Add_Components(static_cast<TRIGGERDESC*>(pArg))))
		return E_FAIL;

	return S_OK;
}

void CTrigger::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CTrigger::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
	}
#endif // _DEBUG
}

void CTrigger::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	if (wcswcs(CollisionEventDesc.pOtherCollisionTag, m_szOtherTag))
	{
		m_isCollision = true;
		Set_ObjEvent(OBJ_DEAD);
	}
}

HRESULT CTrigger::Add_Components(TRIGGERDESC* pTriggerDesc)
{
	lstrcpy(m_szOtherTag, pTriggerDesc->szOtherTag);

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = pTriggerDesc->vTriggerWorldPos;
	RigidBodyDesc.eConstraintFlag = CRigidBody::All;
	RigidBodyDesc.pOwnerObject = this;
	PxBoxGeometry BoxGeometry = PxBoxGeometry(pTriggerDesc->vTriggerSize.x, pTriggerDesc->vTriggerSize.y, pTriggerDesc->vTriggerSize.z);
	RigidBodyDesc.pGeometry = &BoxGeometry;
	RigidBodyDesc.eThisCollsion = COL_TRIGGER;
	RigidBodyDesc.eCollisionFlag = COL_PLAYER;
	strcpy_s(RigidBodyDesc.szCollisionTag, sizeof(_char) * MAX_PATH, pTriggerDesc->szCollisionTag);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("CLoadTrigger Failed Clone Component : Com_RigidBody");
		return E_FAIL;
	}

#ifdef _DEBUG
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("CLoadTrigger Failed Clone Component : Com_Renderer");
		return E_FAIL;
	}
#endif // _DEBUG

	return S_OK;
}

CTrigger* CTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrigger* pInstance = New CTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrigger::Clone(void* pArg)
{
	CTrigger* pInstance = New CTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrigger::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pRenderer);
#endif // _DEBUG

	Safe_Release(m_pRigidBody);
}