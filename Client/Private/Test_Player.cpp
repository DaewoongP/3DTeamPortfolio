#include "..\Public\Test_Player.h"
#include "GameInstance.h"
#include "PhysXConverter.h"
#include "ReportCallBack.h"
#include "BehaviorCallBack.h"

CTest_Player::CTest_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTest_Player::CTest_Player(const CTest_Player& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTest_Player::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTest_Player::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Speed(30.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));
	m_pTransform->Set_RigidBody(m_pRigidBody);
	//m_pTransform->Set_CharacterController(m_pCharacterController);
	
	m_pCharacterController->Set_Position(_float3(10.f, 0.f, 2.f));

	m_pModelCom->Play_Animation(0.f);

	m_fWindPower = 5.f;

	return S_OK;
}

void CTest_Player::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pCharacterController->Move(_float3(0.f, -9.81f, 0.f), fTimeDelta);

	Key_Input(fTimeDelta);

	if (nullptr != m_pModelCom &&
		nullptr != m_pRigidBody)
	{
		m_pModelCom->Set_WindVelocity(PhysXConverter::ToXMFLOAT3(m_pRigidBody->Get_RigidBodyActor()->getLinearVelocity()) * m_fWindPower);

		m_pModelCom->Tick(CCustomModel::ROBE, 2, fTimeDelta);
	}
}

void CTest_Player::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	
	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
		m_pRenderer->Add_DebugGroup(m_pCharacterController);
#endif // _DEBUG
	}

#ifdef _DEBUG
	//Tick_ImGui();
#endif // _DEBUG
}

void CTest_Player::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
}

void CTest_Player::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
}

void CTest_Player::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
}

HRESULT CTest_Player::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	for (_uint iParts = 0; iParts < CCustomModel::MESH_END; ++iParts)
	{
		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes(iParts);

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			try /* Failed Render */
			{
				if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", iParts, i)))
					throw TEXT("Bind_BoneMatrices");

				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", iParts, i, DIFFUSE)))
					throw TEXT("Bind_Material Diffuse");
				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", iParts, i, NORMALS)))
					throw TEXT("Bind_Material Diffuse");

				if (FAILED(m_pShaderCom->Begin("AnimMeshNonCull")))
					throw TEXT("Shader Begin AnimMesh");

				if (FAILED(m_pModelCom->Render(iParts, i)))
					throw TEXT("Model Render");
			}
			catch (const _tchar* pErrorTag)
			{
				wstring wstrErrorMSG = TEXT("[CTest_Player] Failed Render : ");
				wstrErrorMSG += pErrorTag;
				MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

				return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CTest_Player::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false; // static - 고정된 물체 (true -> 고정) (false -> 움직임)
	RigidBodyDesc.isTrigger = false; // 트리거임 원래 콜라이더 생각하시면됩니다.
	RigidBodyDesc.vInitPosition = _float3(_float(rand() % 10), _float(rand() % 10), _float(rand() % 10)); // -> 트랜스폼에다가 초기 포지션 줘도 적용 안됩니다 !! / 요기다 주셔야 합니다 (리지드 바디가 있는 경우만 해당)
	RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
	RigidBodyDesc.fStaticFriction = 0.5f; // 가만히 있을때 움직이기 위한 최소 힘의 수치 0~1
	RigidBodyDesc.fDynamicFriction = 0.5f; // 움직일때 멈추기위한 마찰력? 0~1
	RigidBodyDesc.fRestitution = 0.f; // 탄성값이 얼마나 들어갈 것인가 0~1 -> 1로주면 존나튑니다 보통 0으로줍니다.
	PxCapsuleGeometry GeoMetry = PxCapsuleGeometry(0.5f, 1.f); // Px~Geometry
	//PxSphereGeometry
	//PxBoxGeometry
	RigidBodyDesc.pGeometry = &GeoMetry; // 위에서 만든거 넣어주시면됩니다.
	RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot; // 움직임을 제한할 값을 넣어주면 됩니다. (ex allrot의 경우 로테이션을 하지않습니다.)
	RigidBodyDesc.vDebugColor = _float4(1.f, 1.f, 0.f, 1.f); // 디버그 컬러
	RigidBodyDesc.pOwnerObject = this; // 디스포인터 넣ㄹ어주셔야 안터집니다 !!
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Test_Player_Default");

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_RigidBody)");
		return E_FAIL;
	}

	/* Com_CustomModel */
	if (FAILED(CComposite::Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_Component_Model_CustomModel_Player"),
		TEXT("Com_CustomModel"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_CustomModel)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	m_pModelCom->Add_MeshParts(LEVEL_CLIFFSIDE, TEXT("Prototype_Component_MeshPart_Robe01"), CCustomModel::ROBE, _float4(), TEXT("../../Resources/GameData/ClothData/Test.cloth"));
	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	PxCapsuleControllerDesc CapsuleControllerDesc;
	CapsuleControllerDesc.setToDefault();
	CapsuleControllerDesc.height = 1.f;
	CapsuleControllerDesc.radius = 0.5f;
	CapsuleControllerDesc.material = pGameInstance->Get_Physics()->createMaterial(0.5f, 0.5f, 0.5f);
	CapsuleControllerDesc.density = 10.f;
	CapsuleControllerDesc.stepOffset = 0.5f;
	CapsuleControllerDesc.contactOffset = 0.1f;
	CapsuleControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f);
	CapsuleControllerDesc.userData = this;
	
	Safe_Release(pGameInstance);

	if (false == CapsuleControllerDesc.isValid())
	{
		MSG_BOX("Failed Create Character Controller");
		return E_FAIL;
	}
	
	/* For.Com_Controller */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"),
		TEXT("Com_CharacterController"), reinterpret_cast<CComponent**>(&m_pCharacterController), &CapsuleControllerDesc)))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_CharacterController)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTest_Player::SetUp_ShaderResources()
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

void CTest_Player::Key_Input(_float fTimeDelta)
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_UP))
	{
		m_pCharacterController->Move(m_pTransform->Get_Look() * m_pTransform->Get_Speed(), fTimeDelta, 0.1f);
		m_pRigidBody->Add_Force(m_pTransform->Get_Look() * m_pTransform->Get_Speed(), PxForceMode::eFORCE);
	}

	if (pGameInstance->Get_DIKeyState(DIK_DOWN))
	{
		m_pCharacterController->Move(-m_pTransform->Get_Look() * m_pTransform->Get_Speed(), fTimeDelta, 0.1f);
		m_pRigidBody->Add_Force(m_pTransform->Get_Look() * -m_pTransform->Get_Speed(), PxForceMode::eFORCE);
	}

	if (pGameInstance->Get_DIKeyState(DIK_LEFT))
	{
		m_pCharacterController->Move(-m_pTransform->Get_Right() * m_pTransform->Get_Speed(), fTimeDelta, 0.1f);
		m_pRigidBody->Add_Force(m_pTransform->Get_Right() * -m_pTransform->Get_Speed(), PxForceMode::eFORCE);
	}

	if (pGameInstance->Get_DIKeyState(DIK_RIGHT))
	{
		m_pCharacterController->Move(m_pTransform->Get_Right() * m_pTransform->Get_Speed(), fTimeDelta, 0.1f);
		m_pRigidBody->Add_Force(m_pTransform->Get_Right() * m_pTransform->Get_Speed(), PxForceMode::eFORCE);
	}

	if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		m_pRigidBody->Add_Force(m_pTransform->Get_Up() * 10.f, PxForceMode::eIMPULSE);
	}

	ENDINSTANCE;
}

#ifdef _DEBUG
void CTest_Player::Tick_ImGui()
{
	ImGui::Begin("Test Player");
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	const PxMaterial* pMaterial = m_pRigidBody->Get_Material();
	_float3 vMaterial;
	vMaterial.x = pMaterial->getStaticFriction();
	vMaterial.y = pMaterial->getDynamicFriction();
	vMaterial.z = pMaterial->getRestitution();
	if (ImGui::InputFloat3("static, dynamic, restitution", (_float*)(&vMaterial)))
	{
		m_pRigidBody->Set_Material(vMaterial);
	}
	ImGui::SetNextItemWidth(100.f);
	_float fSpeed = m_pTransform->Get_Speed();
	if (ImGui::InputFloat("Speed", &fSpeed))
	{
		m_pTransform->Set_Speed(fSpeed);
	}

	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.f);
	_float fMaxLinearVelocity = m_pRigidBody->Get_RigidBodyActor()->getMaxLinearVelocity();
	if (ImGui::InputFloat("MaxLinearVelocity", &fMaxLinearVelocity))
	{
		m_pRigidBody->Get_RigidBodyActor()->setMaxLinearVelocity(fMaxLinearVelocity);
	}

	_float3 vPlayerPos;
	ImGui::SetNextItemWidth(20.f);
	ImGui::Text("%.1f /", vPlayerPos.x); ImGui::SameLine();
	ImGui::SetNextItemWidth(20.f);
	ImGui::Text("%.1f /", vPlayerPos.y); ImGui::SameLine();
	ImGui::SetNextItemWidth(20.f);
	ImGui::Text("%.1f /", vPlayerPos.z);
	ImGui::SetNextItemWidth(100.f);
	if (ImGui::InputFloat3("Set Position", (_float*)(&vPlayerPos)))
	{
		m_pTransform->Set_Position(vPlayerPos);
	}

	if (ImGui::Button("Set Position to Cam"))
	{
		_float4x4 ViewMatrixInv = *pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
		_float3 vCamPos = ViewMatrixInv.Translation();
		_float3 vCamDir = ViewMatrixInv.Look();
		m_pRigidBody->Set_Position(vCamPos + vCamDir * 5.f);
	}

	Safe_Release(pGameInstance);
	ImGui::End();
}
#endif // _DEBUG
CTest_Player* CTest_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTest_Player* pInstance = New CTest_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTest_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTest_Player::Clone(void* pArg)
{
	CTest_Player* pInstance = New CTest_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTest_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTest_Player::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pModelCom);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRenderer);
		Safe_Release(m_pCharacterController);
		Safe_Release(m_pRigidBody);
	}
}
