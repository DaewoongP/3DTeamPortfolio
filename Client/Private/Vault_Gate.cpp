#include "..\Public\Vault_Gate.h"
#include "GameInstance.h"

#include "LightStand.h"

#include "Camera_Shake.h"

CVault_Gate::CVault_Gate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CVault_Gate::CVault_Gate(const CVault_Gate& rhs)
	: CGameObject(rhs)
{
}

HRESULT CVault_Gate::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVault_Gate::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CVault_Gate Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_ObjectDesc = *reinterpret_cast<MAPOBJECTDESC*>(pArg);
	m_pTransform->Set_WorldMatrix(m_ObjectDesc.WorldMatrix);

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Vault_Gate_Portal"),
		TEXT("Com_Portal"), reinterpret_cast<CComponent**>(&m_pEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Vault_Gate_Portal)");
		__debugbreak();
		return E_FAIL;
	}

	m_pEffect->Play(m_pTransform->Get_Position() + _float3(2.f, 3.75f, -0.5f));
	m_pEffect->Get_MainModuleRef().fSimulationSpeed = Random_Generator(0.5f, 1.0f);

	return S_OK;
}

HRESULT CVault_Gate::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CVault_Gate Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	m_pModel->Set_CurrentAnimIndex(0);
	m_pModel->Get_Animation(0)->Set_Loop(false);

	// 화로 탐색
	BEGININSTANCE;
	auto pBackGroundLayer = pGameInstance->Find_Components_In_Layer(LEVEL_VAULT, TEXT("Layer_BackGround"));
	ENDINSTANCE;

	for (auto Pair : *pBackGroundLayer)
	{
		wstring wsObjTag = Pair.first;

		if (wstring::npos != wsObjTag.find(TEXT("LightStand")))
		{
			m_pLightStands.push_back(static_cast<CLightStand*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
	}

	Bind_Notifys();

	return S_OK;
}

void CVault_Gate::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// 화로에 불 다 붙으면 문열림
	Check_FireOn();

	BEGININSTANCE;
	if (true == m_isSound0 && true == m_isGateOpen)
	{
		m_iSound0 = pGameInstance->Play_Sound(TEXT("Vault_Door_Open.wav"), 1.f);
		m_isSound0 = false;
	}
		
	ENDINSTANCE;

	if (true == m_isCheckOnce && true == m_isGateOpen)
		m_pModel->Play_Animation(fTimeDelta,&m_SoundChannel, CModel::UPPERBODY, m_pTransform);
	else
		m_pModel->Play_Animation(0, &m_SoundChannel, CModel::UPPERBODY, m_pTransform);
}

void CVault_Gate::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}
}

HRESULT CVault_Gate::Render()
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

		m_pShader->Begin("AnimMesh");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CVault_Gate::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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

HRESULT CVault_Gate::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CCliff_Gate Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CCliff_Gate Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShader))))
	{
		MSG_BOX("Failed CCliff_Gate Add_Component : (Com_ShadowShader)");
		__debugbreak();
		return E_FAIL;
	}


	CCamera_Shake::CAMERA_SHAKE_DESC Camera_Shake_Desc = { CCamera_Shake::CAMERA_SHAKE_DESC() };

	_float fMaxDistance = { 30.0f };
	_float fMinDistance = { 2.0f };

	Camera_Shake_Desc.eShake_Priority = CCamera_Manager::SHAKE_PRIORITY_1;
	Camera_Shake_Desc.isDistanceOption = true;
	Camera_Shake_Desc.pTransform = m_pTransform;
	Camera_Shake_Desc.Shake_Info_Desc.eEase = CEase::IN_EXPO;
	Camera_Shake_Desc.Shake_Info_Desc.eShake_Axis = CCamera_Manager::SHAKE_AXIS_UP;
	Camera_Shake_Desc.Shake_Info_Desc.eShake_Power = CCamera_Manager::SHAKE_POWER_DECRECENDO;
	Camera_Shake_Desc.Shake_Info_Desc.eShake_Type = CCamera_Manager::SHAKE_TYPE_TRANSLATION;
	Camera_Shake_Desc.Shake_Info_Desc.fShakeDuration = 0.1f;
	Camera_Shake_Desc.Shake_Info_Desc.fShakePower = 0.08f;
	Camera_Shake_Desc.Shake_Info_Desc.fShakeSpeed = 10.0f;
	Camera_Shake_Desc.Shake_Info_Desc.vShake_Axis_Set = _float3();

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Enemy_Camera_Shake"),
		TEXT("Com_Camera_Shake_Up"), reinterpret_cast<CComponent**>(&m_pCamera_Shake_Up), &Camera_Shake_Desc)))
		throw TEXT("Com_Camera_Shake_Up");

	m_pCamera_Shake_Up->Ready_Shake(fMaxDistance, fMinDistance, Camera_Shake_Desc.Shake_Info_Desc.fShakePower);

	Camera_Shake_Desc.eShake_Priority = CCamera_Manager::SHAKE_PRIORITY_1;
	Camera_Shake_Desc.isDistanceOption = true;
	Camera_Shake_Desc.pTransform = m_pTransform;
	Camera_Shake_Desc.Shake_Info_Desc.eEase = CEase::IN_EXPO;
	Camera_Shake_Desc.Shake_Info_Desc.eShake_Axis = CCamera_Manager::SHAKE_AXIS_UP;
	Camera_Shake_Desc.Shake_Info_Desc.eShake_Power = CCamera_Manager::SHAKE_POWER_DECRECENDO;
	Camera_Shake_Desc.Shake_Info_Desc.eShake_Type = CCamera_Manager::SHAKE_TYPE_TRANSLATION;
	Camera_Shake_Desc.Shake_Info_Desc.fShakeDuration = 0.1f;
	Camera_Shake_Desc.Shake_Info_Desc.fShakePower = -0.08f;
	Camera_Shake_Desc.Shake_Info_Desc.fShakeSpeed = 10.0f;
	Camera_Shake_Desc.Shake_Info_Desc.vShake_Axis_Set = _float3();

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Enemy_Camera_Shake"),
		TEXT("Com_Camera_Shake_Down"), reinterpret_cast<CComponent**>(&m_pCamera_Shake_Down), &Camera_Shake_Desc)))
		throw TEXT("Com_Camera_Shake_Down");

	m_pCamera_Shake_Down->Ready_Shake(fMaxDistance, fMinDistance, Camera_Shake_Desc.Shake_Info_Desc.fShakePower);

	Camera_Shake_Desc.eShake_Priority = CCamera_Manager::SHAKE_PRIORITY_1;
	Camera_Shake_Desc.isDistanceOption = true;
	Camera_Shake_Desc.pTransform = m_pTransform;
	Camera_Shake_Desc.Shake_Info_Desc.eEase = CEase::IN_EXPO;
	Camera_Shake_Desc.Shake_Info_Desc.eShake_Axis = CCamera_Manager::SHAKE_AXIS_UP;
	Camera_Shake_Desc.Shake_Info_Desc.eShake_Power = CCamera_Manager::SHAKE_POWER_DECRECENDO;
	Camera_Shake_Desc.Shake_Info_Desc.eShake_Type = CCamera_Manager::SHAKE_TYPE_TRANSLATION;
	Camera_Shake_Desc.Shake_Info_Desc.fShakeDuration = 2.0f;
	Camera_Shake_Desc.Shake_Info_Desc.fShakePower = 0.05f;
	Camera_Shake_Desc.Shake_Info_Desc.fShakeSpeed = 6.0f;
	Camera_Shake_Desc.Shake_Info_Desc.vShake_Axis_Set = _float3();

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Enemy_Camera_Shake"),
		TEXT("Com_Camera_Shake_Loop"), reinterpret_cast<CComponent**>(&m_pCamera_Shake_Loop), &Camera_Shake_Desc)))
		throw TEXT("Com_Camera_Shake_Loop");

	m_pCamera_Shake_Loop->Ready_Shake(fMaxDistance, fMinDistance, Camera_Shake_Desc.Shake_Info_Desc.fShakePower);

	Camera_Shake_Desc.eShake_Priority = CCamera_Manager::SHAKE_PRIORITY_1;
	Camera_Shake_Desc.isDistanceOption = true;
	Camera_Shake_Desc.pTransform = m_pTransform;
	Camera_Shake_Desc.Shake_Info_Desc.eEase = CEase::IN_EXPO;
	Camera_Shake_Desc.Shake_Info_Desc.eShake_Axis = CCamera_Manager::SHAKE_AXIS_UP;
	Camera_Shake_Desc.Shake_Info_Desc.eShake_Power = CCamera_Manager::SHAKE_POWER_DECRECENDO;
	Camera_Shake_Desc.Shake_Info_Desc.eShake_Type = CCamera_Manager::SHAKE_TYPE_TRANSLATION;
	Camera_Shake_Desc.Shake_Info_Desc.fShakeDuration = 0.8f;
	Camera_Shake_Desc.Shake_Info_Desc.fShakePower = 0.1f;
	Camera_Shake_Desc.Shake_Info_Desc.fShakeSpeed = 8.0f;
	Camera_Shake_Desc.Shake_Info_Desc.vShake_Axis_Set = _float3();

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Enemy_Camera_Shake"),
		TEXT("Com_Camera_Shake_End"), reinterpret_cast<CComponent**>(&m_pCamera_Shake_End), &Camera_Shake_Desc)))
		throw TEXT("Com_Camera_Shake_End");

	m_pCamera_Shake_End->Ready_Shake(fMaxDistance, fMinDistance, Camera_Shake_Desc.Shake_Info_Desc.fShakePower);

	return S_OK;
}

HRESULT CVault_Gate::SetUp_ShaderResources()
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

HRESULT CVault_Gate::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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

void CVault_Gate::Check_MinMaxPoint(_float3 vPoint)
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

void CVault_Gate::Check_FireOn()
{
	if (true == m_isCheckOnce)
		return;

	_uint iFireOn = m_pLightStands.size();
	_uint iCheckFireOn = 0;

	// 화로가 전부 켜졌는지 확인
	for (auto& iter : m_pLightStands)
	{
		if (true == iter->Get_FireOn())
			++iCheckFireOn;
	}

	if (iFireOn == iCheckFireOn)
	{
		m_isCheckOnce = true;
		m_isAllLightStandsOn = true;
	}		
}

HRESULT CVault_Gate::Bind_Notifys()
{
	if (nullptr == m_pCamera_Shake_Up || nullptr == m_pCamera_Shake_Down || nullptr == m_pCamera_Shake_Loop || nullptr == m_pCamera_Shake_End)
	{
		MSG_BOX("Failed Make_Notifies_for_Shake");
		return E_FAIL;
	}

	function<void()> func = [&] {m_pCamera_Shake_Up->RandomUpAxisShake(); };

	m_pModel->Bind_Notifies(TEXT("Camera_Shake_Up"), func);

	func = [&] {m_pCamera_Shake_Down->RandomUpAxisShake(); };

	m_pModel->Bind_Notifies(TEXT("Camera_Shake_Down"), func);

	func = [&] {m_pCamera_Shake_Loop->RandomUpAxisShake(); };

	m_pModel->Bind_Notifies(TEXT("Camera_Shake_Loop"), func);

	func = [&] {m_pCamera_Shake_End->RandomUpAxisShake(); };

	m_pModel->Bind_Notifies(TEXT("Camera_Shake_End"), func);

	return S_OK;
}

CVault_Gate* CVault_Gate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVault_Gate* pInstance = New CVault_Gate(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CVault_Gate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVault_Gate::Clone(void* pArg)
{
	CVault_Gate* pInstance = New CVault_Gate(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVault_Gate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVault_Gate::Free()
{
	__super::Free();

	for (auto& iter : m_pLightStands)
		Safe_Release(iter);

	Safe_Release(m_pShadowShader);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pEffect);
	Safe_Release(m_pCamera_Shake_Up);
	Safe_Release(m_pCamera_Shake_Down);
	Safe_Release(m_pCamera_Shake_Loop);
	Safe_Release(m_pCamera_Shake_End);
}
