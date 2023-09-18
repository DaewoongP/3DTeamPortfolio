#include "..\Public\Debug_Render_Capsule.h"
#include "Component_Manager.h"

CDebug_Render_Capsule::CDebug_Render_Capsule(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CDebug_Render_Capsule::CDebug_Render_Capsule(const CDebug_Render_Capsule& rhs)
	: CComposite(rhs)
{
}

HRESULT CDebug_Render_Capsule::Initialize_Prototype()
{
	CComponent_Manager* pComponent_Manager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponent_Manager);

	if (FAILED(pComponent_Manager->Add_Prototype(0, TEXT("Prototype_Component_Debug_Render_Sphere_For_Capsule"),
		CDebug_Render_Sphere::Create(m_pDevice, m_pContext), true)))
	{
		MSG_BOX("Failed Create Prototype : RigidBody DebugRender Ring");
		Safe_Release(pComponent_Manager);
		return E_FAIL;
	}

	if (FAILED(pComponent_Manager->Add_Prototype(0, TEXT("Prototype_Component_Debug_Line_For_Capsule"),
		CVIBuffer_Line::Create(m_pDevice, m_pContext), true)))
	{
		MSG_BOX("Failed Create Prototype : RigidBody DebugRender Ring");
		Safe_Release(pComponent_Manager);
		return E_FAIL;
	}

	Safe_Release(pComponent_Manager);

	return S_OK;
}

HRESULT CDebug_Render_Capsule::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Capsule arg null");
		return E_FAIL;
	}

	CAPSULEDESC CapsuleDesc = *reinterpret_cast<CAPSULEDESC*>(pArg);

	CDebug_Render_Sphere::SPHEREDESC SphereDesc;
	SphereDesc.fRadius = CapsuleDesc.fRadius;
	_float3 vHighCenter = _float3(CapsuleDesc.fHalfHeight, 0.f, 0.f);
	vHighCenter = XMVector3TransformCoord(vHighCenter, XMMatrixRotationQuaternion(CapsuleDesc.vOffsetRotation)) + XMLoadFloat3(&CapsuleDesc.vOffsetPosition);
	SphereDesc.vOffsetPosition = vHighCenter;
	SphereDesc.vOffsetRotation = CapsuleDesc.vOffsetRotation;

	/* For.Com_HighSphere */
	if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_Debug_Render_Sphere_For_Capsule"),
		TEXT("Com_HighSphere"), reinterpret_cast<CComponent**>(&m_pHighSphere), &SphereDesc)))
	{
		MSG_BOX("Failed CDebug_Render_Capsule Add_Component : (Com_HighSphere)");
		return E_FAIL;
	}

	_float3 vLowCenter = _float3(-1.f * CapsuleDesc.fHalfHeight, 0.f, 0.f);
	vLowCenter = XMVector3TransformCoord(vLowCenter, XMMatrixRotationQuaternion(CapsuleDesc.vOffsetRotation)) + XMLoadFloat3(&CapsuleDesc.vOffsetPosition);
	SphereDesc.vOffsetPosition = vLowCenter;
	SphereDesc.vOffsetRotation = CapsuleDesc.vOffsetRotation;

	/* For.Com_LowSphere */
	if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_Debug_Render_Sphere_For_Capsule"),
		TEXT("Com_LowSphere"), reinterpret_cast<CComponent**>(&m_pLowSphere), &SphereDesc)))
	{
		MSG_BOX("Failed CDebug_Render_Capsule Add_Component : (Com_LowSphere)");
		return E_FAIL;
	}
	
	_float fRadius = CapsuleDesc.fRadius;

	CVIBuffer_Line::LINEDESC LineDesc;
	LineDesc.iNum = 6;
	vector<_float3> Lines;
	Lines.resize(LineDesc.iNum * 2);

	// 오른쪽 라인
	_float3 vRight = _float3(1.f, 0.f, 0.f) * fRadius;
	vRight = XMVector3TransformCoord(vRight, XMMatrixRotationQuaternion(CapsuleDesc.vOffsetRotation));
	Lines[0] = vHighCenter + vRight;
	Lines[1] = vLowCenter + vRight;

	// 왼쪽 라인
	_float3 vLeft = _float3(-1.f, 0.f, 0.f) * fRadius;
	vLeft = XMVector3TransformCoord(vLeft, XMMatrixRotationQuaternion(CapsuleDesc.vOffsetRotation));
	Lines[2] = vHighCenter + vLeft;
	Lines[3] = vLowCenter + vLeft;

	// 앞쪽 라인
	_float3 vFront = _float3(0.f, 0.f, 1.f) * fRadius;
	vFront = XMVector3TransformCoord(vFront, XMMatrixRotationQuaternion(CapsuleDesc.vOffsetRotation));
	Lines[4] = vHighCenter + vFront;
	Lines[5] = vLowCenter + vFront;

	// 뒤쪽 라인
	_float3 vBack = _float3(0.f, 0.f, -1.f) * fRadius;
	vBack = XMVector3TransformCoord(vBack, XMMatrixRotationQuaternion(CapsuleDesc.vOffsetRotation));
	Lines[6] = vHighCenter + vBack;
	Lines[7] = vLowCenter + vBack;

	// 위쪽 라인
	_float3 vUp = _float3(0.f, 1.f, 0.f) * fRadius;
	vUp = XMVector3TransformCoord(vUp, XMMatrixRotationQuaternion(CapsuleDesc.vOffsetRotation));
	Lines[8] = vHighCenter + vUp;
	Lines[9] = vLowCenter + vUp;

	// 아래쪽 라인
	_float3 vDown = _float3(0.f, -1.f, 0.f) * fRadius;
	vDown = XMVector3TransformCoord(vDown, XMMatrixRotationQuaternion(CapsuleDesc.vOffsetRotation));
	Lines[10] = vHighCenter + vDown;
	Lines[11] = vLowCenter + vDown;

	LineDesc.pLines = Lines.data();

	/* For.Com_Line */
	if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_Debug_Line_For_Capsule"),
		TEXT("Com_Line"), reinterpret_cast<CComponent**>(&m_pLine), &LineDesc)))
	{
		MSG_BOX("Failed CDebug_Render_Capsule Add_Component : (Com_Line)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDebug_Render_Capsule::Render()
{
	if (FAILED(m_pHighSphere->Render()))
		return E_FAIL;

	if (FAILED(m_pLowSphere->Render()))
		return E_FAIL;

	if (FAILED(m_pLine->Render()))
		return E_FAIL;

	return S_OK;
}

CDebug_Render_Capsule* CDebug_Render_Capsule::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebug_Render_Capsule* pInstance = New CDebug_Render_Capsule(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDebug_Render_Capsule");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CDebug_Render_Capsule::Clone(void* pArg)
{
	CDebug_Render_Capsule* pInstance = New CDebug_Render_Capsule(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDebug_Render_Capsule");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDebug_Render_Capsule::Free()
{
	__super::Free();

	Safe_Release(m_pHighSphere);
	Safe_Release(m_pLowSphere);
	Safe_Release(m_pLine);
}
