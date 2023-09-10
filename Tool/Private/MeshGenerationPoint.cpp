#include "MeshGenerationPoint.h"
#include "GameInstance.h"

CMeshGenerationPoint::CMeshGenerationPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice,pContext)
{
}

CMeshGenerationPoint::CMeshGenerationPoint(const CMeshGenerationPoint& rhs)
	: CGameObject(rhs)
{
}

_float4 CMeshGenerationPoint::Get_Position()
{
	return m_pTransform->Get_Position().TransNorm();
}

void CMeshGenerationPoint::Set_Position(_float4 _vPosition)
{
	m_pTransform->Set_Position(_vPosition.xyz());
}

HRESULT CMeshGenerationPoint::Initialize_Prototype()
{
	CGameObject::Initialize_Prototype();
	return S_OK;
}

HRESULT CMeshGenerationPoint::Initialize(void* pArg)
{
	CGameObject::Initialize(pArg);

	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Ready Components");
		return E_FAIL;
	}

	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	return S_OK;
}

void CMeshGenerationPoint::Tick(_float _TimeDelta)
{
	m_pCollider->Tick(m_pTransform->Get_WorldMatrix());
#ifdef _DEBUG
	//렌더러에 던지기만 할듯
	m_pRenderer->Add_DebugGroup(m_pCollider);
#endif // _DEBUG
}

void CMeshGenerationPoint::Late_Tick(_float fTimeDelta)
{
}

void CMeshGenerationPoint::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
	m_isLand = true;
}

HRESULT CMeshGenerationPoint::Render()
{
	return S_OK;
}

_bool CMeshGenerationPoint::RayIntersects(_float4 vOrigin, _float4 vDirection, _float& fDist)
{
	return m_pCollider->RayIntersects(vOrigin, vDirection, fDist);
}

void CMeshGenerationPoint::Set_BoundingDesc(void* pBoundingDesc)
{
	m_pCollider->Set_BoundingDesc(pBoundingDesc);
}


HRESULT CMeshGenerationPoint::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CMeshGenerationPoint Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	CBounding_Sphere::BOUNDINGSPHEREDESC BoundingSphereDesc{};

	BoundingSphereDesc.fRadius = 0.5f;
	BoundingSphereDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);

	/* For.Com_Collider */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Sphere_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider),&BoundingSphereDesc)))
	{
		MSG_BOX("Failed CMeshGenerationPoint Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody))))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_RigidBody)");
		return E_FAIL;
	}
	// 리지드바디 액터 설정
	PxRigidBody* Rigid = m_pRigidBody->Get_RigidBodyActor();
	Rigid->setMaxLinearVelocity(1000.f);
	Rigid->setMass(10.f);

	return S_OK;
}

#ifdef _DEBUG
void CMeshGenerationPoint::Set_Collider_Color(_float4 _vColor)
{
	NULL_CHECK(m_pCollider);

	m_pCollider->Set_Color(_vColor);
}
#endif // _DEBUG

CMeshGenerationPoint* CMeshGenerationPoint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMeshGenerationPoint* pInstance = New CMeshGenerationPoint(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMeshGenerationPoint");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMeshGenerationPoint::Clone(void* pArg)
{
	CMeshGenerationPoint* pInstance = New CMeshGenerationPoint(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMeshGenerationPoint");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshGenerationPoint::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRigidBody);
}


