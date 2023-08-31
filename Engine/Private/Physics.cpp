#include "..\Public\Physics.h"
#include "PhysX_Manager.h"

CPhysics::CPhysics(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CPhysics::CPhysics(const CPhysics& rhs)
	: CComponent(rhs)
{
}

HRESULT CPhysics::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPhysics::Initialize(void* pArg)
{
	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	pPhysX_Manager->Get_Physics();
	//m_pPhysxScene = pPhysX_Manager->Create_Scene();
	if (nullptr == m_pPhysxScene)
	{
		MSG_BOX("Failed Create Scene");
		return E_FAIL;
	}
#ifdef _DEBUG
	// Debug Rendering을 위한 처리
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 2.0f);
#endif // _DEBUG

	m_pControllerManager = PxCreateControllerManager(*m_pPhysxScene);
	if (nullptr == m_pControllerManager)
	{
		MSG_BOX("Failed Create ControllerManager");
		return E_FAIL;
	}

	Safe_Release(pPhysX_Manager);

	return S_OK;
}

void CPhysics::Tick(_float fTimeDelta)
{
	// fixed time 처리 필요할수도 있음.
	m_pPhysxScene->simulate(fTimeDelta);
	m_pPhysxScene->fetchResults(true);
}

#ifdef _DEBUG
HRESULT CPhysics::Render()
{
	return S_OK;
}
#endif // _DEBUG

CPhysics* CPhysics::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPhysics* pInstance = new CPhysics(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPhysics");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPhysics* CPhysics::Clone(void* pArg)
{
	CPhysics* pInstance = new CPhysics(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPhysics");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics::Free()
{
	__super::Free();

	if (nullptr != m_pPhysxScene)
	{
		m_pPhysxScene->release();
	}

	if (nullptr != m_pControllerManager)
	{
		m_pControllerManager->release();
	}
}
