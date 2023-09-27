#include "..\Public\Trigger_Vault.h"
#include "GameInstance.h"

CTrigger_Vault::CTrigger_Vault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTrigger_Vault::CTrigger_Vault(const CTrigger_Vault& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTrigger_Vault::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	TRIGGERPOS vPos = *reinterpret_cast<TRIGGERPOS*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/*m_pMeshEffect->Get_Transform()->Set_Position(vPos.vEffectPos);
	m_pTransform->Set_Position(vPos.vTriggerPos);*/
	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));
	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CTrigger_Vault::Tick(_float fTimeDelta)
{
	if (true == m_isStartEffect)
	{
		_float3 vPos = m_pMeshEffect->Get_Transform()->Get_Position();
		if (8.25f < vPos.y)
			return;
		vPos.y += fTimeDelta;
		m_pMeshEffect->Get_Transform()->Set_Position(vPos);
	}

	__super::Tick(fTimeDelta);
}

void CTrigger_Vault::Late_Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (pGameInstance->Get_DIKeyState(DIK_N, CInput_Device::KEY_DOWN))//트리거 발동시로수정해야함.
		m_pMeshEffect->Stop();
	Safe_Release(pGameInstance);

	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
	}
#endif // _DEBUG
}

void CTrigger_Vault::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	// 트리거 충돌 - 플레이어
	if (wcswcs(CollisionEventDesc.pOtherObjectTag, TEXT("Player")))
	{
		// null확인
		if (nullptr == m_pMeshEffect)
			return;

		// 이펙트 시작 bool
		m_isStartEffect = true;

		// 포지션 긁어옴
		_float3 vPos = m_pMeshEffect->Get_Transform()->Get_Position();

		// 이펙트  시작
		m_pMeshEffect->Play(_float3(vPos.x, -12.f, vPos.z));
	}
}

HRESULT CTrigger_Vault::Add_Components()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry MyGeometry = PxSphereGeometry(10.f);
	RigidBodyDesc.pGeometry = &MyGeometry;
	RigidBodyDesc.eConstraintFlag = CRigidBody::All;
	RigidBodyDesc.eThisCollsion = COL_TRIGGER;
	RigidBodyDesc.eCollisionFlag = COL_PLAYER;
	RigidBodyDesc.pOwnerObject = this;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Trigger_Vault");

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("CTrigger_Vault Failed Clone Component : Com_RigidBody");
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_VAULT, TEXT("Prototype_GameObject_Cloister_MeshEffect"),
		TEXT("Com_MeshEffect"), reinterpret_cast<CComponent**>(&m_pMeshEffect))))
	{
		MSG_BOX("CTrigger_Vault Failed Clone Component : Com_MeshEffect");
		return E_FAIL;
	}

#ifdef _DEBUG
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("CTrigger_Vault Failed Clone Component : Com_Renderer");
		return E_FAIL;
	}
#endif // _DEBUG

	return S_OK;
}

CTrigger_Vault* CTrigger_Vault::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrigger_Vault* pInstance = New CTrigger_Vault(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTrigger_Vault");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrigger_Vault::Clone(void* pArg)
{
	CTrigger_Vault* pInstance = New CTrigger_Vault(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTrigger_Vault");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrigger_Vault::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pRenderer);
#endif // _DEBUG

	Safe_Release(m_pRigidBody);
	Safe_Release(m_pMeshEffect);
}
