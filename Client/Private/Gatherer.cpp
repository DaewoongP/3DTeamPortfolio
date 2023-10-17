#include "..\Public\Gatherer.h"
#include "GameInstance.h"

#include "Player.h"
#include "Player_Information.h"
#include "Inventory.h"

#include "UI_Interaction.h"

CGatherer::CGatherer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CGatherer::CGatherer(const CGatherer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGatherer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGatherer::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CGatherer Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_ObjectDesc = *reinterpret_cast<MAPOBJECTDESC*>(pArg);
	m_pTransform->Set_WorldMatrix(m_ObjectDesc.WorldMatrix);
	m_pTransform->Set_RigidBody(m_pRigidBody);

	if (FAILED(Add_Components()))
		return E_FAIL;

	// �÷��̾� ã��
	BEGININSTANCE;
	m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("GameObject_Player")));
	Safe_AddRef(m_pPlayer);
	m_pPlayerInformation = m_pPlayer->Get_Player_Information();
	Safe_AddRef(m_pPlayerInformation);
	ENDINSTANCE;

	return S_OK;
}

HRESULT CGatherer::Initialize_Level(_uint iCurrentLevelIndex)
{
	m_iCurrentLevel = iCurrentLevelIndex;

	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CGatherer Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	// ä���� ���� �ľ�
	wstring wsTargetName(TEXT("Prototype_Component_Model_"));
	wstring wsModelName(m_ObjectDesc.wszTag);
	
	_uint iLength = wsTargetName.size();
	wsModelName = wsModelName.substr(iLength);

	wstring wsAshwinderEggs(TEXT("Anim_AshwinderEggs"));
	wstring wsHorklump(TEXT("Anim_Horklump"));
	wstring wsLeapingToadStools(TEXT("Anim_LeapingToadStools"));
	wstring wsLeech(TEXT("Anim_Leech"));

	if (0 == lstrcmp(wsModelName.c_str(), wsAshwinderEggs.c_str()))
	{
		m_GatheringType = CGatherer::ASHWINDEREGG;
	}
	else if (0 == lstrcmp(wsModelName.c_str(), wsHorklump.c_str()))
	{
		m_GatheringType = CGatherer::HORKLUMP;

		// ��ü �߱� �߰�
		CLight::LIGHTDESC		LightDescHork;
		ZeroMemory(&LightDescHork, sizeof LightDescHork);

		LightDescHork.eType = CLight::TYPE_POINT;
		LightDescHork.vPos = m_pTransform->Get_Position().TransCoord();
		LightDescHork.fRange = 3.f;

		LightDescHork.vDiffuse = _float4(90.f / 255.f, 109.f / 255.f, 231.f / 255.f, 1.f);
		LightDescHork.vAmbient = WHITEDEFAULT;
		LightDescHork.vSpecular = LightDescHork.vDiffuse;

		BEGININSTANCE;
		pGameInstance->Add_Light(LightDescHork, &m_pLight_Horklump);
		ENDINSTANCE;
	}
	else if (0 == lstrcmp(wsModelName.c_str(), wsLeapingToadStools.c_str()))
	{
		m_GatheringType = CGatherer::LEAPINGTOADSTOOLS;
	}
	else if (0 == lstrcmp(wsModelName.c_str(), wsLeech.c_str()))
	{
		m_GatheringType = CGatherer::LEECH;
	}

	// ä���� �ִϸ��̼��� ���������� 0���� ������ �����
	// 1���� IDLE�̶�� ������ �� ����
	m_pModel->Set_CurrentAnimIndex(1);
	m_pModel->Get_Animation(1)->Set_Loop(true);

	// ������ �ٵ� �ʱ�ȭ
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.eConstraintFlag = CRigidBody::All;
	RigidBodyDesc.fStaticFriction = 1.f;
	RigidBodyDesc.fDynamicFriction = 1.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry MyGeometry = PxSphereGeometry(2.f);
	RigidBodyDesc.pGeometry = &MyGeometry;
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.vDebugColor = _float4(0.f, 1.f, 1.f, 1.f);
	RigidBodyDesc.eThisCollsion = COL_STATIC;
	RigidBodyDesc.eCollisionFlag = COL_PLAYER;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Gatherer");
	
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CDoor Add_Component : (Com_RigidBody)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(Make_Notifies()))
		return E_FAIL;

	return S_OK;
}

void CGatherer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if(true == m_isDissolveStart)
		m_fDissolveAmount += fTimeDelta / 1.5f; // ������ �� ����

	// �÷��̾�� �浹���� ��
	if (true == m_isCol_with_Player && nullptr != m_pModel)
	{
		// ���⼭ ��ư UI�� ��Ÿ���� �ɵ�
		m_pUI_Interaction->Tick(fTimeDelta);

		BEGININSTANCE;  // ��ư�� ������ ����(�ѹ���)
		if (pGameInstance->Get_DIKeyState(DIK_F, CInput_Device::KEY_DOWN) && true == m_isGetItem)
		{
			m_isGetItem = false;

			// ä�����ϴ� �ִϸ��̼����� ����
			m_pModel->Set_CurrentAnimIndex(0);

			// �κ��丮 ȹ�� ó��
			switch (m_GatheringType)
			{
			case CGatherer::ASHWINDEREGG:
				m_pPlayerInformation->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_ASHWINDER_EGGS);
				break;
			case CGatherer::HORKLUMP:
				m_pPlayerInformation->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_HORKLUMP_JUICE);
				break;
			case CGatherer::LEAPINGTOADSTOOLS:
				m_pPlayerInformation->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_LEAPING_TOADSTOOL_CAPS);
				break;
			case CGatherer::LEECH:
				m_pPlayerInformation->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_LEECH_JUICE);
				break;
			}
		}

		ENDINSTANCE;
	}

	if (nullptr != m_pModel)
		m_pModel->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);

	// ä���� ������ �ִϸ��̼��� ������ ���� ���ó��
	if (nullptr != m_pModel) 
	{
		// ������ ������ ������ ��� ó��
		if (0 == m_pModel->Get_CurrentAnimIndex() && m_fDissolveAmount >= 1.f)
		{
			Set_ObjEvent(OBJ_DEAD);

			BEGININSTANCE;
			if(nullptr != m_pLight_Horklump)
				pGameInstance->Return_Light(m_pLight_Horklump);
			ENDINSTANCE;
#ifdef _DEBUG
			cout << "ä���� ����" << '\n';
#endif // _DEBUG
		}
	}
}

void CGatherer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	BEGININSTANCE;

	if (true == m_isCol_with_Player && nullptr != m_pModel)
		m_pUI_Interaction->Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		// ��Ŭ����(�߱� ����) ���� ȸ������ �Ҹ� ���, �ٸ� Renderer�� �־��ش�.
		if (m_GatheringType == CGatherer::HORKLUMP && (_uint)m_iCurrentLevel == LEVEL_VAULT)
			m_pRenderer->Add_RenderGroup(CRenderer::RENDER_GLOW, this);
		else
			m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);

#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}

	ENDINSTANCE;
}

void CGatherer::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wsCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wsPlayer(TEXT("Player_Default"));

	// �÷��̾�� �浹�ߴٸ� ä�� Ȱ��ȭ
	if(0 == lstrcmp(wsCollisionTag.c_str(), wsPlayer.c_str()))
		m_isCol_with_Player = true;	
}

void CGatherer::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	wstring wsCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wsPlayer(TEXT("Player_Default"));

	// �÷��̾�� �浹�� �����ٸ� ä�� ��Ȱ��ȭ
	if (0 == lstrcmp(wsCollisionTag.c_str(), wsPlayer.c_str()))
		m_isCol_with_Player = false;
}

HRESULT CGatherer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", iMeshCount);

		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModel->Bind_Material(m_pShader, "g_NormalTexture", iMeshCount, NORMALS);

		// ��Ŭ����(�߱� ����) ���� ȸ������ �Ҹ� ���, Emissive �ؽ�ó�� �߰��� �־��ش�.
		if (m_GatheringType == CGatherer::HORKLUMP && (_uint)m_iCurrentLevel == LEVEL_VAULT)
			m_pModel->Bind_Material(m_pShader, "g_EmissiveTexture", iMeshCount, EMISSIVE);

		if (true == m_isDissolveStart)
		{
			if (FAILED(m_pShader->Begin("AnimMesh_Dissolve")))
				return E_FAIL;
		}

		else
		{
			// ��Ŭ����(�߱� ����) ���� ȸ������ �Ҹ� ���, �ٸ� �н��� �׷��ش�.
			if (m_GatheringType == CGatherer::HORKLUMP && (_uint)m_iCurrentLevel == LEVEL_VAULT)
				m_pShader->Begin("AnimMesh_E");
			else
				m_pShader->Begin("AnimMesh");
		}	

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGatherer::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	if (FAILED(SetUp_ShadowShaderResources(LightViewMatrix, LightProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_BoneMatrices(m_pShadowShader, "g_BoneMatrices", iMeshCount);

		if (FAILED(m_pShadowShader->Begin("Shadow")))
			return E_FAIL;

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGatherer::Make_Notifies()
{
	function<void()> Func = [&] { this->Gatherer_Dead(); };
	if (FAILED(m_pModel->Bind_Notifies(TEXT("Gatherer_Dead"), Func)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGatherer::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CGatherer Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CGatherer Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShader))))
	{
		MSG_BOX("Failed CGatherer Add_Component : (Com_ShadowShader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_UI_Interaction */
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	CUI_Interaction::INTERACTIONDESC pDesc;
	lstrcpy(pDesc.m_wszName, TEXT("�ȳ��ϼ��������ȯ�Դϴ�"));
	lstrcpy(pDesc.m_wszFunc, TEXT("���Ѹ��°������ϴ���ȯ���Ѹ��°������ϴ���ȯ���Ѹ��°������ϴ���ȯ���Ѹ��°������ϴ���ȯ���Ѹ��°������ϴ���ȯ���Ѹ��°������ϴ���ȯ"));
	pDesc.m_WorldMatrix = m_pTransform->Get_WorldMatrixPtr();

	m_pUI_Interaction = static_cast<CUI_Interaction*>(pGameInstance->Clone_Component(LEVEL_STATIC,
		TEXT("Prototype_GameObject_UI_Interaction"), &pDesc));

	Safe_Release(pGameInstance);

	m_pDissolveTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/VFX/Textures/Noises/VFX_T_NoiseGreypack02_D.png"));
	if (nullptr == m_pDissolveTexture)
	{
		MSG_BOX("Failed CGatherer Add_Texture : (m_pDissolveTexture)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGatherer::SetUp_ShaderResources()
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

	if (nullptr != m_pDissolveTexture)
	{
		if (FAILED(m_pDissolveTexture->Bind_ShaderResources(m_pShader, "g_DissolveTexture")))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
			return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGatherer::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	BEGININSTANCE;

	if (FAILED(m_pShadowShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ViewMatrix", &LightViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ProjMatrix", &LightProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

void CGatherer::Check_MinMaxPoint(_float3 vPoint)
{
	if (m_vMinPoint.x > vPoint.x)
		m_vMinPoint.x = vPoint.x;
	if (m_vMinPoint.y > vPoint.y)
		m_vMinPoint.y = vPoint.y;
	if (m_vMinPoint.z > vPoint.z)
		m_vMinPoint.z = vPoint.z;

	if (m_vMaxPoint.x < vPoint.x)
		m_vMaxPoint.x = vPoint.x;
	if (m_vMaxPoint.y < vPoint.y)
		m_vMaxPoint.y = vPoint.y;
	if (m_vMaxPoint.z < vPoint.z)
		m_vMaxPoint.z = vPoint.z;
}

void CGatherer::Gatherer_Dead()
{
	m_isDissolveStart = true;
}

CGatherer* CGatherer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGatherer* pInstance = New CGatherer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CGatherer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGatherer::Clone(void* pArg)
{
	CGatherer* pInstance = New CGatherer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CGameObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGatherer::Free()
{
	__super::Free();

	if (m_GatheringType == CGatherer::HORKLUMP)
		Safe_Release(m_pLight_Horklump);

	Safe_Release(m_pPlayer);
	Safe_Release(m_pPlayerInformation);

	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pRigidBody);
	Safe_Release(m_pShadowShader);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pUI_Interaction);
}
