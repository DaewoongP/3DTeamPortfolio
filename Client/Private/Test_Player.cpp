#include "..\Public\Test_Player.h"
#include "GameInstance.h"
#include "PhysXConverter.h"
#include "Magic.h"

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

	m_pTransform->Set_Speed(50.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));
	m_pTransform->Set_RigidBody(m_pRigidBody);

	m_pModelCom->Play_Animation(0.f);

	return S_OK;
}

void CTest_Player::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Key_Input(fTimeDelta);

	m_pModelCom->Set_WindVelocity(PhysXConverter::ToXMFLOAT3(m_pRigidBody->Get_RigidBodyActor()->getLinearVelocity()) * m_fWindPower * -1.f);
	m_pModelCom->Tick(CCustomModel::ROBE, 2, fTimeDelta);

}

void CTest_Player::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	
	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
		m_pRenderer->Add_DebugGroup(m_pController);
#endif // _DEBUG
	}

#ifdef _DEBUG
	Tick_ImGui();
#endif // _DEBUG
}

void CTest_Player::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
	cout << "Player Enter" << endl;
}

void CTest_Player::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
	cout << "stay" << endl;
}

void CTest_Player::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
	cout << "Exit" << endl;
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

HRESULT CTest_Player::Render_Depth()
{
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
	RigidBodyDesc.isStatic = false; // static - ������ ��ü (true -> ����) (false -> ������)
	RigidBodyDesc.isTrigger = false; // Ʈ������ ���� �ݶ��̴� �����Ͻø�˴ϴ�.
	RigidBodyDesc.vInitPosition = _float3(5.f, 5.f, 5.f); // -> Ʈ���������ٰ� �ʱ� ������ �൵ ���� �ȵ˴ϴ� !! / ���� �ּž� �մϴ� (������ �ٵ� �ִ� ��츸 �ش�)
	RigidBodyDesc.fStaticFriction = 0.5f; // ������ ������ �����̱� ���� �ּ� ���� ��ġ 0~1
	RigidBodyDesc.fDynamicFriction = 0.5f; // �����϶� ���߱����� ������? 0~1
	RigidBodyDesc.fRestitution = 0.f; // ź������ �󸶳� �� ���ΰ� 0~1 -> 1���ָ� ����Ƨ�ϴ� ���� 0�����ݴϴ�.
	PxCapsuleGeometry GeoMetry = PxCapsuleGeometry(1.f, 2.f); // Px~Geometry
	//PxSphereGeometry
	//PxBoxGeometry
	RigidBodyDesc.pGeometry = &GeoMetry; // ������ ����� �־��ֽø�˴ϴ�.
	RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot; // �������� ������ ���� �־��ָ� �˴ϴ�. (ex allrot�� ��� �����̼��� �����ʽ��ϴ�.)
	RigidBodyDesc.vDebugColor = _float4(1.f, 1.f, 0.f, 1.f); // ����� �÷�
	RigidBodyDesc.pOwnerObject = this; // �������� �֤����ּž� �������ϴ� !!

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_RigidBody)");
		return E_FAIL;
	}
	// ������ٵ� ���� �߰�
	PxRigidBody* Rigid = m_pRigidBody->Get_RigidBodyActor();
	Rigid->setAngularDamping(10.f);
	Rigid->setMaxLinearVelocity(1000.f);
	Rigid->setMass(10.f);
	
	/* Com_CustomModel */
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_CustomModel_Player"),
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

	/* For.Com_Magic*/
	//���� Ŭ���Ҷ� ����ü ���������.
	CMagic::MAGICDESC magicInitDesc;
	magicInitDesc.eBuffType = CMagic::BUFF_NONE;
	magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
	magicInitDesc.eMagicType = CMagic::MT_NOTHING;
	magicInitDesc.eMagicTag = BASICCAST;
	magicInitDesc.fCoolTime = 1.f;
	magicInitDesc.fDamage = 10.f;
	magicInitDesc.fDistance = 0;
	magicInitDesc.fLifeTime = 0.5f;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Magic"),
		TEXT("Com_Magic"), reinterpret_cast<CComponent**>(&m_pMagic), &magicInitDesc)))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_Magic)");
		return E_FAIL;
	}
	
	m_pModelCom->Add_MeshParts(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshPart_Robe01"), CCustomModel::ROBE, TEXT("../../Resources/GameData/ClothData/Test.cloth"));

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
		m_pRigidBody->Add_Force(m_pTransform->Get_Look() * m_pTransform->Get_Speed(), PxForceMode::eFORCE);
	}

	if (pGameInstance->Get_DIKeyState(DIK_DOWN))
	{
		m_pRigidBody->Add_Force(m_pTransform->Get_Look() * -m_pTransform->Get_Speed(), PxForceMode::eFORCE);
	}

	if (pGameInstance->Get_DIKeyState(DIK_LEFT))
	{
		m_pRigidBody->Add_Force(m_pTransform->Get_Right() * -m_pTransform->Get_Speed(), PxForceMode::eFORCE);
	}

	if (pGameInstance->Get_DIKeyState(DIK_RIGHT))
	{
		m_pRigidBody->Add_Force(m_pTransform->Get_Right() * m_pTransform->Get_Speed(), PxForceMode::eFORCE);
	}

	if (pGameInstance->Get_DIKeyState(DIK_C, CInput_Device::KEY_DOWN))
	{
		if (m_pMagic != nullptr)
		{
			m_pMagic->Magic_Cast(m_pTransform);
		}
	}

	if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		m_pRigidBody->Add_Force(m_pTransform->Get_Up() * 30.f, PxForceMode::eIMPULSE);
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
		Safe_Release(m_pController);
		Safe_Release(m_pRigidBody);
		Safe_Release(m_pMagic);
	}
}
