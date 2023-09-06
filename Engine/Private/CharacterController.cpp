#include "..\Public\CharacterController.h"
#include "PipeLine.h"
#include "PhysX_Manager.h"
#include "PhysXConverter.h"
#include <characterkinematic/PxController.h>

#ifdef _DEBUG
#include "Shader.h"
#include "VIBuffer_Line.h"
#include "VIBuffer_Triangle.h"
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

#ifdef _DEBUG
	pScene->simulate(1 / 60.f);
	pScene->fetchResults(true);
	_uint iPrevLines = pScene->getRenderBuffer().getNbLines();
	_uint iPrevTriangles = pScene->getRenderBuffer().getNbTriangles();

	m_iStartLineBufferIndex = pPhysX_Manager->Get_LastLineBufferIndex();
	m_iStartTriangleBufferIndex = pPhysX_Manager->Get_LastTriangleBufferIndex();
#endif // _DEBUG

	// 컨트롤러 매니저를 통해 컨트롤러를 생성합니다.
	PxControllerManager* pControllerManager = pPhysX_Manager->Get_ControllerManager();
	m_pController = pControllerManager->createController(*ControllerDesc);
	m_pController->setUserData(this);
#ifdef _DEBUG
	// 다음 렌더링을 위한 갱신 처리
	pScene->simulate(1 / 60.f);
	pScene->fetchResults(true);
	_uint iNewLines = pScene->getRenderBuffer().getNbLines();
	_uint iNewTriangles = pScene->getRenderBuffer().getNbTriangles();

	m_iNumLineBuffer = iNewLines - iPrevLines;
	m_iNumTriangleBuffer = iNewTriangles - iPrevTriangles;
	pPhysX_Manager->Add_LastLineBufferIndex(iNewLines - iPrevLines);
	pPhysX_Manager->Add_LastTriangleBufferIndex(iNewTriangles - iPrevTriangles);
#endif // _DEBUG

	Safe_Release(pPhysX_Manager);

#ifdef _DEBUG
	if (FAILED(Add_Components()))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

void CCharacterController::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	Make_Buffers();
#endif // _DEBUG
}

#ifdef _DEBUG
HRESULT CCharacterController::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Debug")))
		return E_FAIL;

	if (FAILED(m_pLine->Render()))
		return E_FAIL;

	if (FAILED(m_pTriangle->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacterController::Add_Components()
{
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Debug.hlsl"), VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
	m_Components.emplace(TEXT("Com_Shader"), m_pShader);
	Safe_AddRef(m_pShader);

	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	const PxRenderBuffer* pBuffer = pPhysX_Manager->Get_RenderBuffer();

	CVIBuffer_Line::LINEDESC LineDesc;
	ZEROMEM(&LineDesc);

	LineDesc.iNum = m_iNumLineBuffer;
	const PxDebugLine* pLines = pBuffer->getLines();

	vector<_float3> Lines;
	for (_uint i = m_iStartLineBufferIndex;
		i < m_iStartLineBufferIndex + m_iNumLineBuffer; ++i)
	{
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pLines[i].pos0));
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pLines[i].pos1));
	}
	LineDesc.pLines = Lines.data();

	if (0 < LineDesc.iNum &&
		nullptr != LineDesc.pLines)
	{
		/* For.Com_Line */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_VIBuffer_Line"),
			TEXT("Com_Line"), reinterpret_cast<CComponent**>(&m_pLine), &LineDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Line)");
			return E_FAIL;
		}
	}

	CVIBuffer_Triangle::TRIANGLEDESC TriangleDesc;
	ZEROMEM(&TriangleDesc);

	TriangleDesc.iNum = m_iNumTriangleBuffer;
	const PxDebugTriangle* pDebugTriangles = pBuffer->getTriangles();

	vector<_float3> Triangles;
	for (_uint i = m_iStartTriangleBufferIndex;
		i < m_iStartTriangleBufferIndex + m_iNumTriangleBuffer; ++i)
	{
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos0));
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos1));
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos2));
	}
	TriangleDesc.pTriangles = Triangles.data();

	if (0 < TriangleDesc.iNum &&
		nullptr != TriangleDesc.pTriangles)
	{
		/* For.Com_Triangle */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_VIBuffer_Triangle"),
			TEXT("Com_Triangle"), reinterpret_cast<CComponent**>(&m_pTriangle), &TriangleDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Triangle)");
			return E_FAIL;
		}
	}

	Safe_Release(pPhysX_Manager);

	return S_OK;
}

HRESULT CCharacterController::SetUp_ShaderResources()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float4x4 WorldMatrix = XMMatrixIdentity();

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof(m_vColor))))
		return E_FAIL;

	Safe_Release(pPipeLine);
	return S_OK;
}

void CCharacterController::Make_Buffers()
{
	CVIBuffer_Line::LINEDESC LineDesc;
	ZEROMEM(&LineDesc);

	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	const PxRenderBuffer* pBuffer = pPhysX_Manager->Get_RenderBuffer();

	Safe_Release(pPhysX_Manager);

	const PxDebugLine* pDebugLines = pBuffer->getLines();

	vector<_float3> Lines;
	for (_uint i = m_iStartLineBufferIndex;
		i < m_iStartLineBufferIndex + m_iNumLineBuffer; ++i)
	{
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pDebugLines[i].pos0));
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pDebugLines[i].pos1));
	}

	LineDesc.iNum = m_iNumLineBuffer;
	LineDesc.pLines = Lines.data();

	m_pLine->Tick(LineDesc);

	CVIBuffer_Triangle::TRIANGLEDESC TriangleDesc;
	ZEROMEM(&TriangleDesc);

	const PxDebugTriangle* pDebugTriangles = pBuffer->getTriangles();

	vector<_float3> Triangles;
	for (_uint i = m_iStartTriangleBufferIndex;
		i < m_iStartTriangleBufferIndex + m_iNumTriangleBuffer; ++i)
	{
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos0));
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos1));
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos2));
	}
	TriangleDesc.iNum = m_iNumTriangleBuffer;
	TriangleDesc.pTriangles = Triangles.data();

	m_pTriangle->Tick(TriangleDesc);
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

#ifdef _DEBUG
	Safe_Release(m_pShader);
	Safe_Release(m_pLine);
	Safe_Release(m_pTriangle);
#endif // _DEBUG
}
