#include "Smith_Fig.h"
#include "GameInstance.h"
#include "UI_Interaction.h"
#include "Script.h"
#include "Quest_Manager.h"

CSmith_Fig::CSmith_Fig(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSmith_Fig::CSmith_Fig(const CSmith_Fig& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSmith_Fig::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSmith_Fig::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_float4x4* pMatrix = reinterpret_cast<_float4x4*>(pArg);
		m_pTransform->Set_WorldMatrix(*pMatrix);
	}
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	m_pModelCom->Change_Animation(TEXT("Stand_Listen"));

	return S_OK;
}

void CSmith_Fig::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);


	if (true == m_isColPlayer && nullptr != m_pModelCom)
	{
		m_pUI_Interaction->Tick(fTimeDelta);

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (pGameInstance->Get_DIKeyState(DIK_F, CInput_Device::KEY_DOWN))
		{
			m_isPlayScript = true;
			m_pScripts[m_iScriptIndex]->Reset_Script();
			m_pScripts[m_iScriptIndex]->Set_isRender(true);
		}
		Safe_Release(pGameInstance);
	}


	if (m_isPlayScript)
	{
		m_pScripts[m_iScriptIndex]->Tick(fTimeDelta);

		if (true == m_pScripts[SMITHFIGSCRIPT_POTION]->Is_Finished())
		{
			CQuest_Manager* pQuest_Manager = CQuest_Manager::GetInstance();
			Safe_AddRef(pQuest_Manager);
			pQuest_Manager->Unlock_Quest(TEXT("Quest_Potion"));
			Safe_Release(pQuest_Manager);
		}
	}
}

void CSmith_Fig::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (true == m_isColPlayer && nullptr != m_pModelCom)
	{
		m_pUI_Interaction->Late_Tick(fTimeDelta);

		if (m_isPlayScript)
			m_pScripts[m_iScriptIndex]->Late_Tick(fTimeDelta);
	}

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}
}

void CSmith_Fig::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{	
	wstring wsCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wsPlayer(TEXT("Player_Default"));

	if (0 == lstrcmp(wsCollisionTag.c_str(), wsPlayer.c_str()))
		m_isColPlayer = true;
}

void CSmith_Fig::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	wstring wsCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wsPlayer(TEXT("Player_Default"));

	if (0 == lstrcmp(wsCollisionTag.c_str(), wsPlayer.c_str()))
	{
		m_isColPlayer = false;
		m_isPlayScript = false;
		m_pScripts[m_iScriptIndex]->Reset_Script();
	}
}

HRESULT CSmith_Fig::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try /* Failed Render */
		{

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				throw TEXT("Bind_BoneMatrices");

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE)))
				throw TEXT("Bind_Material Diffuse");

			if (0 == i)
			{
				if (FAILED(m_pShaderCom->Begin("HairMesh")))
					throw TEXT("Shader Begin HairMesh");
			}
			else
			{
				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, NORMALS)))
					throw TEXT("Bind_Material Normal");

				if (FAILED(m_pShaderCom->Begin("AnimMesh")))
					throw TEXT("Shader Begin AnimMesh");
			}

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Model Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CSmith_Fig] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CSmith_Fig::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	if (FAILED(SetUp_ShadowShaderResources(LightViewMatrix, LightProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try /* Failed Render */
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShadowShaderCom, "g_BoneMatrices", i)))
				throw TEXT("Bind_BoneMatrices");

			if (FAILED(m_pShadowShaderCom->Begin("Shadow")))
				throw TEXT("Shader Begin AnimMesh");

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Model Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CSmith_Fig] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CSmith_Fig::Add_Components()
{
	try /* Check Add_Components */
	{
		/* Com_Renderer */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
			TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
			throw TEXT("Com_Renderer");

		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Professor_Fig"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			throw TEXT("Com_Model");

		/* For.Com_Shader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			throw TEXT("Com_Shader");

		/* For.Com_ShadowShader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
			TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShaderCom))))
			throw TEXT("Com_ShadowShader");

		/* Com_RigidBody */
		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.eConstraintFlag = CRigidBody::RotX | CRigidBody::RotY | CRigidBody::RotZ;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.85f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
		PxCapsuleGeometry pCapsuleGeomatry = PxCapsuleGeometry(0.25f, 0.6f);
		RigidBodyDesc.pGeometry = &pCapsuleGeomatry;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Body");
		RigidBodyDesc.eThisCollsion = COL_NPC;
		RigidBodyDesc.eCollisionFlag = COL_STATIC;

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = true;
		RigidBodyDesc.eConstraintFlag = CRigidBody::RotX | CRigidBody::RotY | CRigidBody::RotZ;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.85f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
		PxSphereGeometry pSphereGeomatry = PxSphereGeometry(2.f); // 범위 설정 
		RigidBodyDesc.pGeometry = &pSphereGeomatry;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "NPC_Range");
		RigidBodyDesc.eThisCollsion = COL_NPC_RANGE;
		RigidBodyDesc.eCollisionFlag = COL_STATIC | COL_PLAYER;

		if (FAILED(m_pRigidBody->Create_Collider(&RigidBodyDesc)))
			return E_FAIL;

		/* Com_UI_Interaction */
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CUI_Interaction::INTERACTIONDESC pDesc;
		lstrcpy(pDesc.m_wszName, TEXT("피그 교수"));
		lstrcpy(pDesc.m_wszFunc, TEXT("대화하기"));
		pDesc.m_WorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		m_pUI_Interaction = static_cast<CUI_Interaction*>(pGameInstance->Clone_Component(LEVEL_STATIC,
			TEXT("Prototype_GameObject_UI_Interaction"), &pDesc));

		if (m_pUI_Interaction == nullptr)
		{
			Safe_Release(pGameInstance);
			throw TEXT("Com_UI_Interaction");
		}

		CScript* pScript = static_cast<CScript*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Script")));
		pScript->Add_Script(TEXT("../../Resources/UI/Game/Script/Fig/Fig_2_1.png"));
		pScript->Add_Script(TEXT("../../Resources/UI/Game/Script/Fig/Fig_2_2.png"));
		pScript->Add_Script(TEXT("../../Resources/UI/Game/Script/Fig/Fig_2_3.png"));
		m_pScripts.push_back(pScript);

		pScript = static_cast<CScript*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Script")));
		pScript->Add_Script(TEXT("../../Resources/UI/Game/Script/Fig/Fig_3_1.png"));
		pScript->Add_Script(TEXT("../../Resources/UI/Game/Script/Fig/Fig_3_2.png"));
		m_pScripts.push_back(pScript);

		Safe_Release(pGameInstance);
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CSmith_Fig] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSmith_Fig::SetUp_ShaderResources()
{
	BEGININSTANCE;

	try /* Check SetUp_ShaderResources */
	{
		if (nullptr == m_pShaderCom)
			throw TEXT("m_pShaderCom is nullptr");

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
			throw TEXT("Failed Bind_Matrix : g_WorldMatrix");

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
			throw TEXT("Failed Bind_Matrix : g_ViewMatrix");

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
			throw TEXT("Failed Bind_Matrix : g_ProjMatrix");

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
			throw TEXT("Failed Bind_RawValue : g_fCamFar");

		_float3 vHairColor = { 0.7f, 0.7f, 0.7f };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vHairColor", &vHairColor, sizeof(_float3))))
			throw TEXT("Failed Bind_RawValue : g_vHairColor");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CSmith_Fig] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CSmith_Fig::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	BEGININSTANCE;

	try /* Check SetUp_ShaderResources */
	{
		if (nullptr == m_pShadowShaderCom)
			throw TEXT("m_pShaderCom is nullptr");

		if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
			throw TEXT("Failed Bind_Matrix : g_WorldMatrix");

		if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_ViewMatrix", &LightViewMatrix)))
			throw TEXT("Failed Bind_Matrix : g_ViewMatrix");

		if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_ProjMatrix", &LightProjMatrix)))
			throw TEXT("Failed Bind_Matrix : g_ProjMatrix");

		if (FAILED(m_pShadowShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
			throw TEXT("Failed Bind_RawValue : g_fCamFar");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CSmith_Fig] Failed SetUp_ShadowShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

CSmith_Fig* CSmith_Fig::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSmith_Fig* pInstance = New CSmith_Fig(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSmith_Fig");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSmith_Fig::Clone(void* pArg)
{
	CSmith_Fig* pInstance = New CSmith_Fig(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSmith_Fig");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSmith_Fig::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShadowShaderCom);
	Safe_Release(m_pRigidBody);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pUI_Interaction);

	for (auto& pScript : m_pScripts)
	{
		Safe_Release(pScript);
	}
}
