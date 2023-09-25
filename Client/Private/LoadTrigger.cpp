#include "..\Public\LoadTrigger.h"
#include "GameInstance.h"
#include "Seamless_Loader.h"

CLoadTrigger::CLoadTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLoadTrigger::CLoadTrigger(const CLoadTrigger& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLoadTrigger::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CLoadTrigger::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRenderer)
	{
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

void CLoadTrigger::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	if (wcswcs(CollisionEventDesc.pOtherObjectTag, TEXT("Player")))
	{
		//积己 鞍篮芭 or 构 贸府窍绊
		//Set_ObjEvent(OBJ_DEAD);
	}
}

HRESULT CLoadTrigger::Add_Components()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = _float3(20.f, 10.f, 20.f);
	RigidBodyDesc.vDebugColor = _float4(0.f, 0.f, 0.f, 1.f);
	RigidBodyDesc.eConstraintFlag = CRigidBody::All;
	RigidBodyDesc.pOwnerObject = this;
	PxSphereGeometry SphereGeometry = PxSphereGeometry(10.f);
	RigidBodyDesc.pGeometry = &SphereGeometry;
	strcpy_s(RigidBodyDesc.szCollisionTag, "LoadTrigger");
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

CLoadTrigger* CLoadTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoadTrigger* pInstance = New CLoadTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CLoadTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoadTrigger::Clone(void* pArg)
{
	CLoadTrigger* pInstance = New CLoadTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CLoadTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoadTrigger::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pRenderer);
#endif // _DEBUG

	Safe_Release(m_pRigidBody);
}
