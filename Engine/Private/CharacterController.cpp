#include "..\Public\CharacterController.h"
#include "PhysX_Manager.h"
#include "PhysXConverter.h"
#include <characterkinematic/PxController.h>

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

	m_pController->move(PhysXConverter::ToPxVec3(_vVelocity * _fTimeDelta), _fMinDist, 0, nullptr, nullptr);
}

CCharacterController::CCharacterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CCharacterController::CCharacterController(const CCharacterController& rhs)
	: CComposite(rhs)
{
}

HRESULT CCharacterController::Initialize(void* pArg)
{
	// 사용 예시입니다.
	/*PxCapsuleControllerDesc CapsuleControllerDesc;
	CapsuleControllerDesc.setToDefault();
	CapsuleControllerDesc.radius = 1.f;
	CapsuleControllerDesc.height = 1.f;
	CapsuleControllerDesc.material = pPhysX_Manager->Get_Physics()->createMaterial(0.f, 0.f, 0.f);
	CapsuleControllerDesc.density = 30.f;
	CapsuleControllerDesc.isValid();*/

	if (nullptr == pArg)
	{
		MSG_BOX("PxController not valid");
		return E_FAIL;
	}

	PxControllerDesc* ControllerDesc = reinterpret_cast<PxControllerDesc*>(pArg);

	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	PxScene* pScene = pPhysX_Manager->Get_PhysxScene();

	pScene->simulate(1 / 60.f);
	pScene->fetchResults(true);

	// 컨트롤러 매니저를 통해 컨트롤러를 생성합니다.
	PxControllerManager* pControllerManager = pPhysX_Manager->Get_ControllerManager();

	const PxRenderBuffer* pBuffer = pPhysX_Manager->Get_RenderBuffer();
	m_pController = pControllerManager->createController(*ControllerDesc);
	
	pScene->simulate(1 / 60.f);
	pScene->fetchResults(true);
	pBuffer = pPhysX_Manager->Get_RenderBuffer();

	Safe_Release(pPhysX_Manager);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CCharacterController::Render()
{
	return E_NOTIMPL;
}

HRESULT CCharacterController::Add_Components()
{
	return E_NOTIMPL;
}

HRESULT CCharacterController::SetUp_ShaderResources()
{
	return E_NOTIMPL;
}

void CCharacterController::Make_Buffers()
{
}
#endif // _DEBUG

CCharacterController* CCharacterController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCharacterController* pInstance = new CCharacterController(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCharacterController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCharacterController::Clone(void* pArg)
{
	CCharacterController* pInstance = new CCharacterController(*this);

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
}
