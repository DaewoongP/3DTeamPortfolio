#include "..\Public\Debug_Render_Sphere.h"
#include "Debug_Render_Ring.h"
#include "Component_Manager.h"

CDebug_Render_Sphere::CDebug_Render_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CDebug_Render_Sphere::CDebug_Render_Sphere(const CDebug_Render_Sphere& rhs)
	: CComposite(rhs)
{
}

HRESULT CDebug_Render_Sphere::Initialize_Prototype()
{
	CComponent_Manager* pComponent_Manager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponent_Manager);

	if (FAILED(pComponent_Manager->Add_Prototype(0, TEXT("Prototype_Component_Debug_Render_Ring_For_Sphere"),
		CDebug_Render_Ring::Create(m_pDevice, m_pContext), true)))
	{
		MSG_BOX("Failed Create Prototype : RigidBody DebugRender Ring");
		Safe_Release(pComponent_Manager);
		return E_FAIL;
	}

	Safe_Release(pComponent_Manager);

	return S_OK;
}

HRESULT CDebug_Render_Sphere::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Sphere Desc arg null");
		return E_FAIL;
	}
	
	SPHEREDESC SphereDesc = *reinterpret_cast<SPHEREDESC*>(pArg);

	CDebug_Render_Ring::RINGDESC RingDesc;
	RingDesc.fRadius = SphereDesc.fRadius;
	RingDesc.vMajorAxis = _float3(1.f, 0.f, 0.f);
	RingDesc.vMinorAxis = _float3(0.f, 0.f, 1.f);
	RingDesc.vOrigin = SphereDesc.vOrigin;

	/* For.Com_XZRing */
	if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_Debug_Render_Ring_For_Sphere"),
		TEXT("Com_XZRing"), reinterpret_cast<CComponent**>(&m_pXZRing), &RingDesc)))
	{
		MSG_BOX("Failed CRigidBody Add_Component : (Com_XZRing)");
		return E_FAIL;
	}
	
	RingDesc.vMajorAxis = _float3(1.f, 0.f, 0.f);
	RingDesc.vMinorAxis = _float3(0.f, 1.f, 0.f);

	/* For.Com_XYRing */
	if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_Debug_Render_Ring_For_Sphere"),
		TEXT("Com_XYRing"), reinterpret_cast<CComponent**>(&m_pXYRing), &RingDesc)))
	{
		MSG_BOX("Failed CRigidBody Add_Component : (Com_XYRing)");
		return E_FAIL;
	}
	
	RingDesc.vMajorAxis = _float3(0.f, 1.f, 0.f);
	RingDesc.vMinorAxis = _float3(0.f, 0.f, 1.f);

	/* For.Com_YZRing */
	if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_Debug_Render_Ring_For_Sphere"),
		TEXT("Com_YZRing"), reinterpret_cast<CComponent**>(&m_pYZRing), &RingDesc)))
	{
		MSG_BOX("Failed CRigidBody Add_Component : (Com_YZRing)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDebug_Render_Sphere::Render()
{
	if (FAILED(m_pXZRing->Render()))
		return E_FAIL;

	if (FAILED(m_pXYRing->Render()))
		return E_FAIL;

	if (FAILED(m_pYZRing->Render()))
		return E_FAIL;

	return S_OK;
}

CDebug_Render_Sphere* CDebug_Render_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebug_Render_Sphere* pInstance = new CDebug_Render_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDebug_Render_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CDebug_Render_Sphere::Clone(void* pArg)
{
	CDebug_Render_Sphere* pInstance = new CDebug_Render_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDebug_Render_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDebug_Render_Sphere::Free()
{
	__super::Free();

	Safe_Release(m_pXZRing);
	Safe_Release(m_pXYRing);
	Safe_Release(m_pYZRing);
}
