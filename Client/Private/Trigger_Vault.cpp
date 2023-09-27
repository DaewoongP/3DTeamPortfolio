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
	if (FAILED(Add_Components()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	dynamic_cast<CMeshEffect*>(pGameInstance->Find_Component_In_Layer(
		LEVEL_VAULT, TEXT("Layer_MeshEffect"), TEXT("GameObject_MeshEffect")));
	Safe_Release(pGameInstance);

	return S_OK;
}

void CTrigger_Vault::Tick(_float fTimeDelta)
{
	if (true == m_isStartEffect)
	{
		m_fAccTime += fTimeDelta;
		_float3 vPos = m_pMeshEffect->Get_Transform()->Get_Position();
		m_pMeshEffect->Get_Transform()->Set_Position(_float3(vPos.x, -12.f + m_fAccTime, vPos.z));
	}
}

void CTrigger_Vault::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
	}
#endif // _DEBUG
}

void CTrigger_Vault::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	if (wcswcs(CollisionEventDesc.pOtherObjectTag, TEXT("Player")))
	{
		m_isStartEffect = true;
		
		if (nullptr == m_pMeshEffect)
			return;

		_float3 vPos = m_pMeshEffect->Get_Transform()->Get_Position();

		if (nullptr != m_pMeshEffect)
			m_pMeshEffect->Play(_float3(vPos.x, -12.f, vPos.z));
	}
}

HRESULT CTrigger_Vault::Add_Components()
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
}
