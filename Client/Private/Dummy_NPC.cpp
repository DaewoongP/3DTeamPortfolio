#include "Dummy_NPC.h"
#include "GameInstance.h"
#include "..\Public\Dummy_NPC.h"
#include "UI_Interaction.h"
#include "Script.h"

CDummy_NPC::CDummy_NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummy_NPC::CDummy_NPC(const CDummy_NPC& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummy_NPC::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummy_NPC::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	NPCINITDESC* pDesc = reinterpret_cast<NPCINITDESC*>(pArg);

	m_isInteraction = pDesc->isInteraction;
	if (m_isInteraction)
		lstrcpy(wszScriptTag, pDesc->wszScriptTag);

	if (FAILED(Add_Components(*pDesc)))
		return E_FAIL;

	if (FAILED(Ready_MeshParts(*pDesc)))
		return E_FAIL;

	m_pTransform->Set_WorldMatrix(pDesc->WorldMatrix);
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	m_pCustomModel->Change_Animation(pDesc->wstrAnimationTag);

#ifdef _DEBUG
	m_isCheckPosition = pDesc->isCheckPosition;
#endif // _DEBUG

	return S_OK;
}

void CDummy_NPC::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (nullptr != m_pCustomModel)
		m_pCustomModel->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);

	if (true == m_isColPlayer && m_isInteraction)
	{
		m_pUI_Interaction->Tick(fTimeDelta);

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (pGameInstance->Get_DIKeyState(DIK_F, CInput_Device::KEY_DOWN))
		{
			m_isPlayScript = true;
			m_pScripts->Reset_Script();
			m_pScripts->Set_isRender(true);
		}
		Safe_Release(pGameInstance);
	}

	if (m_isPlayScript && m_isInteraction)
		m_pScripts->Tick(fTimeDelta);

#ifdef _DEBUG
	ADD_IMGUI([&] { this->Tick_TestShake(); });
#endif // _DEBUG
}

void CDummy_NPC::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (true == m_isColPlayer && m_isInteraction)
	{
		m_pUI_Interaction->Late_Tick(fTimeDelta);

		if (m_isPlayScript)
			m_pScripts->Late_Tick(fTimeDelta);
	}

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

void CDummy_NPC::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	// 플레이어가 range콜라이더 안에 진입한경우 "한번 불림"
	wstring wsCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wsPlayer(TEXT("Player_Default"));

	if (0 == lstrcmp(wsCollisionTag.c_str(), wsPlayer.c_str()))
		m_isColPlayer = true;

}

void CDummy_NPC::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	// 플레이어가 range콜라이더 밖으로 나간경우 "한번 불림"
	wstring wsCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wsPlayer(TEXT("Player_Default"));

	if (0 == lstrcmp(wsCollisionTag.c_str(), wsPlayer.c_str()))
	{
		m_isColPlayer = false;
		m_isPlayScript = false;
		m_pScripts->Reset_Script();
	}
}


#ifdef _DEBUG

void CDummy_NPC::Tick_TestShake()
{
	if (false == m_isCheckPosition)
		return;

	ImGui::Begin("TestShake");
	
	wstring wstrTag = m_pTag;
	string strTag = wstrToStr(wstrTag);

	_float3 vPosition = m_pTransform->Get_Position();

	ImGui::DragFloat3(strTag.c_str(), (_float*)&vPosition, 0.05f, -100.f, 10000.f);
	strTag += "Degree";

	_float3 vRadians;
	ImGui::DragFloat3(strTag.c_str(), (_float*)&m_vAngle, 0.5f, -360.f, 360.f);
	vRadians.x = XMConvertToRadians(m_vAngle.x);
	vRadians.y = XMConvertToRadians(m_vAngle.y);
	vRadians.z = XMConvertToRadians(m_vAngle.z);

	m_pTransform->Set_Position(vPosition);
	m_pTransform->Rotation(vRadians);

	ImGui::End();
}

#endif

HRESULT CDummy_NPC::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	for (_uint iPartsIndex = 0; iPartsIndex < CCustomModel::MESH_END; ++iPartsIndex)
	{
		_uint		iNumMeshes = m_pCustomModel->Get_NumMeshes(iPartsIndex);

		if (CCustomModel::HAIR == iPartsIndex)
		{
			for (_uint i = 0; i < iNumMeshes; ++i)
			{
				m_pCustomModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", iPartsIndex, i);
				m_pCustomModel->Bind_Color(m_pShaderCom, "g_vHairColor", iPartsIndex);
				m_pCustomModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", iPartsIndex, i, DIFFUSE);
				m_pCustomModel->Bind_Material(m_pShaderCom, "g_NormalTexture", iPartsIndex, i, NORMALS);

				m_pShaderCom->Begin("HairMesh");

				m_pCustomModel->Render(iPartsIndex, i);
			}
		}
		else if (CCustomModel::HEAD == iPartsIndex ||
			CCustomModel::ARM == iPartsIndex)
		{
			for (_uint i = 0; i < iNumMeshes; ++i)
			{
				m_pCustomModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", iPartsIndex, i);

				m_pCustomModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", iPartsIndex, i, DIFFUSE);
				m_pCustomModel->Bind_Material(m_pShaderCom, "g_NormalTexture", iPartsIndex, i, NORMALS);

				m_pShaderCom->Begin("AnimMesh");

				m_pCustomModel->Render(iPartsIndex, i);
			}
		}
		else if (CCustomModel::ROBE == iPartsIndex)
		{
			for (_uint i = 0; i < iNumMeshes; ++i)
			{
				m_pCustomModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", iPartsIndex, i);

				m_pCustomModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", iPartsIndex, i, DIFFUSE);
				m_pCustomModel->Bind_Material(m_pShaderCom, "g_NormalTexture", iPartsIndex, i, NORMALS);

				m_pShaderCom->Begin("AnimMeshNonCull");

				m_pCustomModel->Render(iPartsIndex, i);
			}
		}
		else
		{
			for (_uint i = 0; i < iNumMeshes; ++i)
			{
				m_pCustomModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", iPartsIndex, i);
				m_pCustomModel->Bind_Color(m_pShaderCom, "g_vColor", iPartsIndex);
				m_pCustomModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", iPartsIndex, i, DIFFUSE);
				m_pCustomModel->Bind_Material(m_pShaderCom, "g_NormalTexture", iPartsIndex, i, NORMALS);

				m_pShaderCom->Begin("AnimMeshColor");

				m_pCustomModel->Render(iPartsIndex, i);
			}
		}
	}

	return S_OK;
}

HRESULT CDummy_NPC::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	if (FAILED(SetUp_ShadowShaderResources(LightViewMatrix, LightProjMatrix)))
		return E_FAIL;

	for (_uint iPartsIndex = 0; iPartsIndex < CCustomModel::MESH_END; ++iPartsIndex)
	{
		_uint		iNumMeshes = m_pCustomModel->Get_NumMeshes(iPartsIndex);

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pCustomModel->Bind_BoneMatrices(m_pShadowShaderCom, "g_BoneMatrices", iPartsIndex, i);

			m_pShadowShaderCom->Begin("Shadow");

			m_pCustomModel->Render(iPartsIndex, i);
		}
	}

	return S_OK;
}

HRESULT CDummy_NPC::Add_Components(const NPCINITDESC& Desc)
{
	try /* Check Add_Components */
	{
		/* Com_Renderer */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
			TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
			throw TEXT("Com_Renderer");

		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(LEVEL_SMITH, Desc.wstrCustomModelTag.c_str(),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pCustomModel))))
			throw TEXT("Com_Model");

		/* For.Com_Shader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			throw TEXT("Com_Shader");

		/* For.Com_ShadowShader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
			TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShaderCom))))
			throw TEXT("Com_ShadowShader");

		if (m_isInteraction)
		{
			CRigidBody::RIGIDBODYDESC RigidBodyDesc;
			RigidBodyDesc.isStatic = true;
			RigidBodyDesc.isTrigger = true;
			RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
			RigidBodyDesc.fDynamicFriction = 1.f;
			RigidBodyDesc.fRestitution = 0.f;
			RigidBodyDesc.fStaticFriction = 0.f;
			RigidBodyDesc.pOwnerObject = this;
			RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
			RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
			RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.85f, 0.f);
			RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
			PxSphereGeometry pSphereGeomatry = PxSphereGeometry(1.f); // 범위 설정 
			RigidBodyDesc.pGeometry = &pSphereGeomatry;
			strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "NPC_Range");
			RigidBodyDesc.eThisCollsion = COL_NPC_RANGE;
			RigidBodyDesc.eCollisionFlag = COL_PLAYER;

			if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
				TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
				throw TEXT("Com_RigidBody");

			/* Com_UI_Interaction */
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			CUI_Interaction::INTERACTIONDESC pDesc;
			lstrcpy(pDesc.m_wszName, TEXT("마을 주민"));
			lstrcpy(pDesc.m_wszFunc, TEXT("대화하기"));
			pDesc.m_WorldMatrix = m_pTransform->Get_WorldMatrixPtr();

			m_pUI_Interaction = static_cast<CUI_Interaction*>(pGameInstance->Clone_Component(LEVEL_STATIC,
				TEXT("Prototype_GameObject_UI_Interaction"), &pDesc));
			if (m_pUI_Interaction == nullptr)
			{
				Safe_Release(pGameInstance);
				throw TEXT("Com_UI_Interaction");
			}

			_uint iResult = lstrcmp(TEXT(""), wszScriptTag);
			if (iResult != 0)
			{
				m_pScripts = static_cast<CScript*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Script")));
				m_pScripts->Add_Script(wszScriptTag);
			}
			Safe_Release(pGameInstance);
		}
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDummy_NPC] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDummy_NPC::Ready_MeshParts(const NPCINITDESC& Desc)
{
	_float4 vColor = _float4(1.f, 1.f, 1.f, 1.f);

	_uint iIndex = { 0 };
	for (auto wstrPartTag : Desc.MeshPartsTags)
	{
		if (0 == wstrPartTag.size())
		{
			++iIndex;
			continue;
		}
		
		if (CCustomModel::HAIR == iIndex)
		{
			vColor = _float4(GetRandomFloat(0.1f, 0.4f), GetRandomFloat(0.1f, 0.4f), GetRandomFloat(0.1f, 0.4f), 1.f);
		}
		else
			vColor = _float4(1.f, 1.f, 1.f, 1.f);

		if (FAILED(m_pCustomModel->Add_MeshParts(LEVEL_STATIC, wstrPartTag, CCustomModel::MESHTYPE(iIndex++), vColor)))
		{
			wstring wstrErrorTag = TEXT("Failed Add MeshPart : ");
			wstrErrorTag += wstrPartTag;
			MSG_BOX(wstrErrorTag.c_str());
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CDummy_NPC::SetUp_ShaderResources()
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

		_float3 vHairColor = { 0.5f, 0.7f, 0.6f };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vHairColor", &vHairColor, sizeof(_float3))))
			throw TEXT("Failed Bind_RawValue : g_vHairColor");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDummy_NPC] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDummy_NPC::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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
		wstring wstrErrorMSG = TEXT("[CDummy_NPC] Failed SetUp_ShadowShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

CDummy_NPC* CDummy_NPC::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_NPC* pInstance = New CDummy_NPC(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDummy_NPC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_NPC::Clone(void* pArg)
{
	CDummy_NPC* pInstance = New CDummy_NPC(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDummy_NPC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy_NPC::Free()
{
	__super::Free();

	Safe_Release(m_pCustomModel);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShadowShaderCom);
	Safe_Release(m_pRigidBody);
	Safe_Release(m_pRenderer);

	Safe_Release(m_pUI_Interaction);
	Safe_Release(m_pScripts);
}