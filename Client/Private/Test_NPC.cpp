#include "..\Public\Test_NPC.h"
#include "GameInstance.h"
#include "PhysXConverter.h"

CTest_NPC::CTest_NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTest_NPC::CTest_NPC(const CTest_NPC& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTest_NPC::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTest_NPC::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));
	m_pRigidBody->Set_TransformComponent(m_pTransform);
	//m_pRigidBody->Set_ControllerComponent(m_pController);

	return S_OK;
}

void CTest_NPC::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	//m_pTransform->Set_Position(m_pController->Get_Position());

	Key_Input(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CTest_NPC::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CTest_NPC::Render()
{
#ifdef _DEBUG
	Tick_ImGui();
#endif // _DEBUG

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE);

		m_pShaderCom->Begin("AnimMesh");

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CTest_NPC::Render_Depth()
{
	return S_OK;
}

HRESULT CTest_NPC::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CTest_NPC Add_Component : (Com_Renderer)");
		return E_FAIL;
	}
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	PxCapsuleControllerDesc CapsuleControllerDesc;
	CapsuleControllerDesc.setToDefault();
	CapsuleControllerDesc.radius = 1.f;
	CapsuleControllerDesc.height = 1.f;
	CapsuleControllerDesc.material = pGameInstance->Get_Physics()->createMaterial(0.f, 0.f, 0.f);
	CapsuleControllerDesc.density = 30.f;
	CapsuleControllerDesc.isValid();

	Safe_Release(pGameInstance);

	/* Com_Controller */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"),
		TEXT("Com_Controller"), reinterpret_cast<CComponent**>(&m_pController), &CapsuleControllerDesc)))
	{
		MSG_BOX("Failed CTest_NPC Add_Component : (Com_Controller)");
		return E_FAIL;
	}

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody))))
	{
		MSG_BOX("Failed CTest_NPC Add_Component : (Com_RigidBody)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_TestModel"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("Failed CTest_NPC Add_Component : (Com_Model)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CTest_NPC Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTest_NPC::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CTest_NPC::Key_Input(_float fTimeDelta)
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_UP))
	{
		//m_pController->Move(_float3(0.f, 0.f, 1.f), fTimeDelta * m_pTransform->Get_Speed());
		m_pRigidBody->Add_Force(m_pTransform->Get_Look() * m_pTransform->Get_Speed(), PxForceMode::eACCELERATION);
	}

	if (pGameInstance->Get_DIKeyState(DIK_DOWN))
	{
		//m_pController->Move(_float3(0.f, 0.f, 1.f), -1.f * fTimeDelta * m_pTransform->Get_Speed());
		m_pRigidBody->Add_Force(m_pTransform->Get_Look() * -m_pTransform->Get_Speed(), PxForceMode::eACCELERATION);
	}

	if (pGameInstance->Get_DIKeyState(DIK_LEFT))
	{
		m_pRigidBody->Add_Force(m_pTransform->Get_Right() * -m_pTransform->Get_Speed(), PxForceMode::eACCELERATION);
	}

	if (pGameInstance->Get_DIKeyState(DIK_RIGHT))
	{
		m_pRigidBody->Add_Force(m_pTransform->Get_Right() * m_pTransform->Get_Speed(), PxForceMode::eACCELERATION);
	}

	if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		m_pRigidBody->Add_Force(m_pTransform->Get_Up() * 30.f, PxForceMode::eIMPULSE);
	}

	ENDINSTANCE;

}

#ifdef _DEBUG
void CTest_NPC::Tick_ImGui()
{
}
#endif // _DEBUG

CTest_NPC* CTest_NPC::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTest_NPC* pInstance = New CTest_NPC(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTest_NPC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTest_NPC::Clone(void* pArg)
{
	CTest_NPC* pInstance = New CTest_NPC(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTest_NPC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTest_NPC::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pController);
	Safe_Release(m_pRigidBody);
}
