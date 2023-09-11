#include "..\Public\Player.h"
#include "GameInstance.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("Failed Player Initialize");

		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Player Add_Components");

		return E_FAIL;
	}

	if (FAILED(Ready_MeshParts()))
	{
		MSG_BOX("Failed Ready Player Mesh Parts");

		return E_FAIL;
	}


#ifdef _DEBUG
	
	if (FAILED(Ready_Test_Collider()))
	{
		MSG_BOX("Failed Ready Player Test_Collider");

		return E_FAIL;
	}

#endif // _DEBUG


	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Key_Input(fTimeDelta);


	m_pCustomModel->Play_Animation(fTimeDelta);
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
		
		m_pRenderer->Add_DebugGroup(m_pTestCollider);

#endif // _DEBUG

	}

#ifdef _DEBUG
	//Tick_ImGui();
#endif // _DEBUG
}

void CPlayer::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
	cout << "Player Enter" << endl;
}

void CPlayer::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
	cout << "stay" << endl;
}

void CPlayer::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
	cout << "Exit" << endl;
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	for (_uint iPartsIndex = 0; iPartsIndex < CCustomModel::MESH_END; ++iPartsIndex)
	{
		_uint		iNumMeshes = m_pCustomModel->Get_NumMeshes(iPartsIndex);

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pCustomModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", iPartsIndex, i);

			m_pCustomModel->Bind_Material(m_pShader, "g_DiffuseTexture", iPartsIndex, i, DIFFUSE);

			m_pShader->Begin("AnimMesh");

			m_pCustomModel->Render(iPartsIndex, i);
		}
	}

	return S_OK;
}

HRESULT CPlayer::Render_Depth()
{
	return S_OK;
}

HRESULT CPlayer::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* For.Com_Model_CustomModel_Player */
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_CustomModel_Player"),
		TEXT("Com_Model_CustomModel_Player"), reinterpret_cast<CComponent**>(&m_pCustomModel))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Model_CustomModel_Player)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_UP))
	{
		//m_pRigidBody->Add_Force(m_pTransform->Get_Look() * m_pTransform->Get_Speed(), PxForceMode::eACCELERATION);
	}
	
	if (pGameInstance->Get_DIKeyState(DIK_DOWN))
	{
		//m_pRigidBody->Add_Force(m_pTransform->Get_Look() * -m_pTransform->Get_Speed(), PxForceMode::eACCELERATION);
	}

	if (pGameInstance->Get_DIKeyState(DIK_LEFT))
	{
		m_pTransform->Turn(_float3(0.f, -1.f, 0.f), fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_RIGHT))
	{
		m_pTransform->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		//m_pRigidBody->Add_Force(m_pTransform->Get_Up() * 30.f, PxForceMode::eIMPULSE);
	}

	ENDINSTANCE;
}

HRESULT CPlayer::Ready_MeshParts()
{
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_MAINGAME, 
		TEXT("Prototype_Component_MeshParts_Up"),
		CCustomModel::TOP, 
		nullptr)))
	{
		MSG_BOX("Failed Add MeshPart Up");

		return E_FAIL;
	}

	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_MAINGAME,
		TEXT("Prototype_Component_MeshParts_Low"),
		CCustomModel::PANTS,
		nullptr)))
	{
		MSG_BOX("Failed Add MeshPart Low");

		return E_FAIL;
	}

	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_MAINGAME,
		TEXT("Prototype_Component_MeshParts_Robe"),
		CCustomModel::ROBE,
		nullptr)))
	{
		MSG_BOX("Failed Add MeshPart Robe");

		return E_FAIL;
	}

	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_MAINGAME,
		TEXT("Prototype_Component_MeshParts_Socks"),
		CCustomModel::SOCKS,
		nullptr)))
	{
		MSG_BOX("Failed Add MeshPart Socks");

		return E_FAIL;
	}

	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_MAINGAME,
		TEXT("Prototype_Component_MeshParts_Socks1"),
		CCustomModel::SOCKS,
		nullptr)))
	{
		MSG_BOX("Failed Add MeshPart Socks1");

		return E_FAIL;
	}

	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_MAINGAME,
		TEXT("Prototype_Component_MeshParts_Shoes"),
		CCustomModel::SHOES,
		nullptr)))
	{
		MSG_BOX("Failed Add MeshPart Shoes");

		return E_FAIL;
	}

	return S_OK;
}

#ifdef _DEBUG

HRESULT CPlayer::Ready_Test_Collider()
{
	CBounding_Sphere::BOUNDINGSPHEREDESC BoundingSphereDesc{};

	BoundingSphereDesc.fRadius = 0.5f;
	BoundingSphereDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);

	/* For.Com_Collider */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sphere_Collider"),
		TEXT("Com_TestCollider"), reinterpret_cast<CComponent**>(&m_pTestCollider), &BoundingSphereDesc)))
	{
		MSG_BOX("Failed CDebug_Point Add_Component : (Com_TestCollider)");
		return E_FAIL;
	}

	return S_OK;
}

void CPlayer::Tick_ImGui()
{
	//ImGui::Begin("Player");
	
	//ImGui::End();
}

#endif // _DEBUG

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = New CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = New CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	CGameObject::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pShader);
		Safe_Release(m_pRenderer);
		Safe_Release(m_pCustomModel);

#ifdef _DEBUG

		Safe_Release(m_pTestCollider);

#endif // _DEBUG
	}
}
