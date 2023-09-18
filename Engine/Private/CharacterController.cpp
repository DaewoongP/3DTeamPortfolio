#include "..\Public\CharacterController.h"
#include "PipeLine.h"
#include "PhysX_Manager.h"
#include "PhysXConverter.h"
#include <characterkinematic/PxController.h>
#include "ReportCallBack.h"
#include "BehaviorCallBack.h"

#ifdef _DEBUG
#include "Shader.h"
#include "Debug_Render_Box.h"
#include "Debug_Render_Capsule.h"
#include "Component_Manager.h"
#endif // _DEBUG

_float3 CCharacterController::Get_Position()
{
	if (nullptr == m_pController)
		return _float3();

	return PhysXConverter::ToXMFLOAT3(m_pController->getPosition());
}

void CCharacterController::Set_Position(_float3 _vPosition)
{
	if (nullptr == m_pController)
		return;
	
	m_pController->setPosition(PhysXConverter::ToPxExtendedVec3(_vPosition));
}

void CCharacterController::Move(_float3 _vVelocity, _float _fTimeDelta, _float _fMinDist)
{
	if (nullptr == m_pController)
		return;

	PxControllerCollisionFlags collisionFlags = 
		m_pController->move(PhysXConverter::ToPxVec3(_vVelocity * _fTimeDelta), _fMinDist, 0, nullptr, nullptr);

}

CCharacterController::CCharacterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CCharacterController::CCharacterController(const CCharacterController& rhs)
	: CComposite(rhs)
#ifdef _DEBUG
	, m_vColor(_float4(0.f, 1.f, 0.f, 1.f))
#endif // _DEBUG
{
}

HRESULT CCharacterController::Initialize_Prototype()
{
#ifdef _DEBUG
	CComponent_Manager* pComponent_Manager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponent_Manager);

	if (FAILED(pComponent_Manager->Add_Prototype(0, TEXT("Prototype_Component_RigidBody_Debug_Render_Box"),
		CDebug_Render_Box::Create(m_pDevice, m_pContext), true)))
	{
		MSG_BOX("Failed Create Prototype : RigidBody DebugRender Box");
		Safe_Release(pComponent_Manager);
		return E_FAIL;
	}

	if (FAILED(pComponent_Manager->Add_Prototype(0, TEXT("Prototype_Component_RigidBody_Debug_Render_Capsule"),
		CDebug_Render_Capsule::Create(m_pDevice, m_pContext), true)))
	{
		MSG_BOX("Failed Create Prototype : RigidBody DebugRender Capsule");
		Safe_Release(pComponent_Manager);
		return E_FAIL;
	}

	Safe_Release(pComponent_Manager);
#endif // _DEBUG
	return S_OK;
}

HRESULT CCharacterController::Initialize(void* pArg)
{
	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	if (nullptr == pArg)
	{
		MSG_BOX("PxController not valid");
		Safe_Release(pPhysX_Manager);
		return E_FAIL;
	}

	PxControllerDesc* pControllerDesc = reinterpret_cast<PxControllerDesc*>(pArg);
	pControllerDesc->behaviorCallback = m_pBehaviorCallBack= CBehaviorCallBack::Create();
	pControllerDesc->reportCallback = m_pReportCallBack = CReportCallBack::Create();
	// 컨트롤러 매니저를 통해 컨트롤러를 생성합니다.
	PxControllerManager* pControllerManager = pPhysX_Manager->Get_ControllerManager();
	m_pController = pControllerManager->createController(*pControllerDesc);
	m_pController->setUserData(pControllerDesc->userData);
	Safe_Release(pPhysX_Manager);

	PxRigidDynamic* m_pActor = m_pController->getActor();
	m_pActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
	_uint iNumShapes = m_pActor->getNbShapes();

	vector<PxShape*> Shapes(iNumShapes);
	_uint iNumPtrs = m_pActor->getShapes(Shapes.data(), iNumShapes);

	PxFilterData FilterData;
	FilterData.word0 = 0x1111;
	
	for (_uint i = 0; i < iNumPtrs; ++i)
	{
		PxShape* pShape = Shapes[i];
		pShape->setSimulationFilterData(FilterData);
		pShape->setQueryFilterData(FilterData);
	}

#ifdef _DEBUG
	if (FAILED(Add_Components(pControllerDesc)))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

#ifdef _DEBUG
HRESULT CCharacterController::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Debug")))
		return E_FAIL;

	if (FAILED(m_pDebug_Render->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacterController::Add_Components(PxControllerDesc* pControllerDesc)
{
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Debug.hlsl"), VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
	m_Components.emplace(TEXT("Com_Shader"), m_pShader);
	Safe_AddRef(m_pShader);

	if (PxControllerShapeType::eBOX == m_pController->getType())
	{
		PxBoxControllerDesc* pBoxControllerDesc = dynamic_cast<PxBoxControllerDesc*>(pControllerDesc);
		// Debug Components
		CDebug_Render_Box::BOXDESC BoxDesc;
		BoxDesc.vExtents.x = pBoxControllerDesc->halfSideExtent;
		BoxDesc.vExtents.y = pBoxControllerDesc->halfHeight;
		BoxDesc.vExtents.z = pBoxControllerDesc->halfForwardExtent;
		BoxDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
		BoxDesc.vOffsetRotation = _float4(0.f, 0.f, 0.f, 1.f);

		/* For.Com_Debug_Render_Box */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_RigidBody_Debug_Render_Box"),
			TEXT("Com_Debug_Render_Box"), reinterpret_cast<CComponent**>(&m_pDebug_Render), &BoxDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Debug_Render_Box)");
			return E_FAIL;
		}
	}
	else if (PxControllerShapeType::eCAPSULE == m_pController->getType())
	{
		PxCapsuleControllerDesc* pCapsuleControllerDesc = dynamic_cast<PxCapsuleControllerDesc*>(pControllerDesc);

		CDebug_Render_Capsule::CAPSULEDESC CapsuleDesc;
		CapsuleDesc.fRadius = pCapsuleControllerDesc->radius;
		CapsuleDesc.fHalfHeight = pCapsuleControllerDesc->height;
		CapsuleDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
		CapsuleDesc.vOffsetRotation = _float4(0.f, 0.f, 0.f, 1.f);
		/* For.Com_Debug_Render_Capsule */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_RigidBody_Debug_Render_Capsule"),
			TEXT("Com_Debug_Render_Capsule"), reinterpret_cast<CComponent**>(&m_pDebug_Render), &CapsuleDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Debug_Render_Capsule)");
			return E_FAIL;
		}
	}
	else
	{
		MSG_BOX("Character Controller : Invalid Shape Type");
	}

	if (nullptr == m_pDebug_Render)
	{
		MSG_BOX("Character Controller : Failed Create Debug Render");
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CCharacterController::SetUp_ShaderResources()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float4x4 WorldMatrix = XMMatrixTranslation(_float(m_pController->getPosition().x), _float(m_pController->getPosition().y), _float(m_pController->getPosition().z));

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pPipeLine);
	return S_OK;
}

#endif // _DEBUG

CCharacterController* CCharacterController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCharacterController* pInstance = New CCharacterController(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCharacterController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCharacterController::Clone(void* pArg)
{
	CCharacterController* pInstance = New CCharacterController(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCharacterController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCharacterController::Free()
{
	__super::Free();

	if (nullptr != m_pController)
	{
		m_pController->release();
	}

	Safe_Release(m_pBehaviorCallBack);
	Safe_Release(m_pReportCallBack);

#ifdef _DEBUG
	Safe_Release(m_pShader);
	Safe_Release(m_pDebug_Render);
#endif // _DEBUG
}
