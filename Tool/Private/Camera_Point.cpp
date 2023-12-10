#include "Camera_Point.h"
#include "GameInstance.h"

CCamera_Point::CCamera_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice,pContext)
{
}

_float4 CCamera_Point::Get_Position()
{
	return m_pTransform->Get_Position().TransNorm();
}

void CCamera_Point::Set_Position(_float4 _vPosition)
{
	m_pTransform->Set_Position(_vPosition.xyz());
}

HRESULT CCamera_Point::Initialize_Prototype()
{
	CGameObject::Initialize_Prototype();
	return S_OK;
}

HRESULT CCamera_Point::Initialize(void* pArg)
{
	CGameObject::Initialize(pArg);

	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Ready Components");
		return E_FAIL;
	}

	CAMERAPOINTDESC CameraPointDesc = *(CAMERAPOINTDESC*)pArg;

	m_pTransform->Set_Position(CameraPointDesc.vPosition.xyz());

	return S_OK;
}

void CCamera_Point::Tick(_float _TimeDelta)
{
	m_pCollider->Tick(m_pTransform->Get_WorldMatrix());
#ifdef _DEBUG
	//렌더러에 던지기만 할듯
	m_pRenderer->Add_DebugGroup(m_pCollider);
#endif // _DEBUG
}

HRESULT CCamera_Point::Render()
{
	return S_OK;
}

_bool CCamera_Point::RayIntersects(_float4 vOrigin, _float4 vDirection, _float& fDist)
{
	return m_pCollider->RayIntersects(vOrigin, vDirection, fDist);
}

void CCamera_Point::Set_BoundingDesc(void* pBoundingDesc)
{
	m_pCollider->Set_BoundingDesc(pBoundingDesc);
}


HRESULT CCamera_Point::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CCamera_Point Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	CBounding_Sphere::BOUNDINGSPHEREDESC BoundingSphereDesc{};

	BoundingSphereDesc.fRadius = 0.5f;
	BoundingSphereDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);

	/* For.Com_Collider */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Sphere_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider),&BoundingSphereDesc)))
	{
		MSG_BOX("Failed CCamera_Point Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	return S_OK;
}

#ifdef _DEBUG
void CCamera_Point::Set_Collider_Color(_float4 _vColor)
{
	NULL_CHECK(m_pCollider);

	m_pCollider->Set_Color(_vColor);
}
#endif // _DEBUG

CCamera_Point* CCamera_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Point* pInstance = New CCamera_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCamera_Point");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_Point::Clone(void* pArg)
{
	CCamera_Point* pInstance = New CCamera_Point(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCamera_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Point::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider);
}
