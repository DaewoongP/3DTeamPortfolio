#include "Protego_Effect.h"
#include "GameInstance.h"

CProtego_Effect::CProtego_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CProtego_Effect::CProtego_Effect(const CProtego_Effect& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
	, m_eCurState(rhs.m_eCurState)
	, m_vColor1(rhs.m_vColor1)
	, m_vColor2(rhs.m_vColor2)
	, m_fScale(rhs.m_fScale)
	, m_fTimeAcc(rhs.m_fTimeAcc)
	, m_fHitTimeAcc(rhs.m_fHitTimeAcc)
	, m_fEnterDuration(rhs.m_fEnterDuration)
	, m_fExitDuration(rhs.m_fExitDuration)
	, m_fRimPower(rhs.m_fRimPower)
	, m_isHitEffect(rhs.m_isHitEffect)
	, m_vCollisionPoint(rhs.m_vCollisionPoint)
{
}

HRESULT CProtego_Effect::Initialize_Prototype(_uint _iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iLevel = _iLevel;
	BEGININSTANCE;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_Component_Shader_Protego")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Shader_Protego"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Protego.hlsl"), VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
			return E_FAIL;
	}

	/* For.Prototype_GameObject_DefaultConeBoom_Particle */
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_DefaultConeBoom_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_DefaultConeBoom_Particle"),
			CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/DefaultConeBoom"), m_iLevel))))
			return E_FAIL;
	}

	/* For.Prototype_GameObject_DefaultConeBoom_Particle */
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireBlastFB_Loop_D_Flipbook")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireBlastFB_Loop_D_Flipbook"),
			CTexture_Flipbook::Create(m_pDevice, m_pContext, m_iLevel, TEXT("../../Resources/GameData/FlipbookData/FireBlastFB_Loop_D.flipbook")))))
			return E_FAIL;
	}

	// 원의 모양을 잡아주는 텍스처
	Find_And_Add_Texture(TEXT("../../Resources/Effects/Textures/Gradients/VFX_T_Circle_LerpMask_D.png"));

	// 5번째 일렁이는 텍스처와 섞어 쓰는 노이즈 텍스처
	Find_And_Add_Texture(TEXT("../../Resources/Effects/Textures/Noises/VFX_T_Noise04_D.png"));

	// 모름
	Find_And_Add_Texture(TEXT("../../Resources/Effects/Textures/Noises/VFX_T_RibbonOffset_N.png"));

	// 마법을 막았을 때 생기는 연기같은걸로 추정됨.
	Find_And_Add_Texture(TEXT("../../Resources/Effects/Textures/VFX_T_Inky_Smoke_D.png"));

	// 표면에 일렁이는 효과
	Find_And_Add_Texture(TEXT("../../Resources/Effects/Textures/VFX_T_Wisps_2_D.png"));

	ENDINSTANCE;
	return S_OK;
}

HRESULT CProtego_Effect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pMagicBallDesc = reinterpret_cast<CMagicBall::MAGICBALLDESC*>(pArg);

	if (FAILED(Add_Components()))
		return E_FAIL;

	switch (m_pMagicBallDesc->eMagicType)
	{
	case Client::CMagic::MT_NOTHING:
		break;
	case Client::CMagic::MT_YELLOW:
		break;
	case Client::CMagic::MT_PURPLE:
		break;
	case Client::CMagic::MT_RED:
		break;
	case Client::CMagic::MT_ALL:
		m_vColor1 = { 0.f, 0.f, 1.f, 1.f };
		m_vColor2 = { 0.5f, 0.f, 0.5f, 1.f };
		break;
	default:
		break;
	}

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));
	m_pFlameBlastFlipbook->Get_Transform()->Set_Scale(_float3(3.f, 3.f, 1.f));

	return S_OK;
}

void CProtego_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	
	//Tick_Imgui();
	m_fHitTimeAcc += fTimeDelta;
	if (m_fHitTimeAcc >= 0.5f)
	{
		m_isHitEffect = false;
	}
	BEGININSTANCE;

#ifdef _DEBUG
	
	if (pGameInstance->Get_DIKeyState(DIK_K))
	{
		Play_ForceField();
	}
	if (pGameInstance->Get_DIKeyState(DIK_L))
	{
		Exit_ForceField();
	}
	if (pGameInstance->Get_DIKeyState(DIK_J))
	{
		Hit_Effect(_float3(9.f, 9.f, 9.f));
	}
#endif // _DEBUG

	m_fTimeAcc += fTimeDelta;
	switch (m_eCurState)
	{
	case Client::CProtego_Effect::ENTER:
		Tick_Enter(fTimeDelta);
		break;
	case Client::CProtego_Effect::STAY:
		Tick_Stay(fTimeDelta);
		break;
	case Client::CProtego_Effect::EXIT:
		Tick_Exit(fTimeDelta);
		break;
	case Client::CProtego_Effect::STATE_END:
		break;
	default:
		break;
	}
	ENDINSTANCE;
}

void CProtego_Effect::Late_Tick(_float fTimeDelta)
{
	m_pDefaultConeBoom_Particle->Late_Tick(fTimeDelta);

	//switch (m_eCurState)
	//{
	//case Client::CProtego_Effect::ENTER:
	//	Late_Tick_Enter(fTimeDelta);
	//	break;
	//case Client::CProtego_Effect::STAY:
	//	Late_Tick_Stay(fTimeDelta);
	//	break;
	//case Client::CProtego_Effect::EXIT:
	//	Late_Tick_Exit(fTimeDelta);
	//	break;
	//case Client::CProtego_Effect::STATE_END:
	//	break;
	//default:
	//	break;
	//}

	if (nullptr != m_pRenderer && STATE_END != m_eCurState)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
#ifdef _DEBUG

#endif // _DEBUG
	}
}

HRESULT CProtego_Effect::Render()
{
#ifdef _DEBUG
	// Tick_ImGui();
#endif // _DEBUG
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pShader->Begin("Protego"), E_FAIL);
	FAILED_CHECK_RETURN(m_pBuffer->Render(), E_FAIL);
	return S_OK;
}

void CProtego_Effect::Play_ForceField(_float fScale)
{
	m_fScale = fScale;
	m_eCurState = ENTER;
	m_fTimeAcc = 0.f;
}

void CProtego_Effect::Exit_ForceField()
{
	m_fTimeAcc = 0.f;
	m_eCurState = EXIT;
}

void CProtego_Effect::Hit_Effect(_float3 vPosition)
{
	// 파티클 위치 설정 후 터트리기
	_float3 vDirection = vPosition - m_pTransform->Get_Position();
	vDirection.Normalize();
	m_vCollisionPoint = vDirection * m_pTransform->Get_Scale().x;
	SHAPE_MODULE& shapeModule = m_pDefaultConeBoom_Particle->Get_ShapeModuleRef();
	shapeModule.Set_ShapeLook(m_pTransform->Get_Position(), vPosition);
	m_pDefaultConeBoom_Particle->Get_Transform()->Set_Position(m_vCollisionPoint);
	m_pDefaultConeBoom_Particle->Play();

	m_pFlameBlastFlipbook->Play(m_vCollisionPoint);
	m_isHitEffect = true;
	m_fHitTimeAcc = 0.f;
}

void CProtego_Effect::Tick_Enter(const _float& fTimeDelta)
{
	_float fRatio = m_fTimeAcc / m_fEnterDuration;
	m_vColor1.w = fRatio;
	m_vColor2.w = fRatio;

	m_pTransform->Set_Scale(_float3(m_fScale * fRatio, m_fScale * fRatio, m_fScale * fRatio));

	if (fRatio > 1.f)
		m_eCurState = STAY;
}

void CProtego_Effect::Tick_Stay(const _float& fTimeDelta)
{
	if (m_pMagicBallDesc->fLifeTime - m_fTimeAcc <= m_fExitDuration)
	{
		//Exit_ForceField();
	}
}

void CProtego_Effect::Tick_Exit(const _float& fTimeDelta)
{
	_float fRatio = m_fTimeAcc / m_fExitDuration;
	m_vColor1.w = fRatio;
	m_vColor2.w = fRatio;
	m_fScale = 3.f - fRatio * 3.f;
	m_pTransform->Set_Scale(_float3(m_fScale, m_fScale, m_fScale));

	if (fRatio >= 1.f)
		m_eCurState = STATE_END;
}

void CProtego_Effect::Late_Tick_Enter(const _float& fTimeDelta)
{
}

void CProtego_Effect::Late_Tick_Stay(const _float& fTimeDelta)
{
}

void CProtego_Effect::Late_Tick_Exit(const _float& fTimeDelta)
{
}

void CProtego_Effect::Find_And_Add_Texture(const _tchar* pPath)
{
	BEGININSTANCE;

	wstring wstrTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), pPath);
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, pGameInstance->Make_WChar(wstrTag.data())))
	{
		pGameInstance->Add_Prototype(m_iLevel, wstrTag.data()
			, CTexture::Create(m_pDevice, m_pContext, pPath));
	}
	ENDINSTANCE;
}

#ifdef _DEBUG
void CProtego_Effect::Tick_Imgui()
{
	RECT clientRect;
	GetClientRect(g_hWnd, &clientRect);
	POINT leftTop = { clientRect.left, clientRect.top };
	POINT rightBottom = { clientRect.right, clientRect.bottom };
	ClientToScreen(g_hWnd, &leftTop);
	ClientToScreen(g_hWnd, &rightBottom);
	int Left = leftTop.x;
	int Top = rightBottom.y;
	ImVec2 vWinpos = { _float(Left + 0.f), _float(Top) };
	ImGui::SetNextWindowPos(vWinpos);

	ImGui::Begin("RimPower");
	
	ImGui::DragFloat("RimPower", &m_fRimPower, 0.1f, 0.f, FLT_MAX);

	ImGui::End();
}
#endif // _DEBUG

HRESULT CProtego_Effect::Add_Components()
{
	try
	{
		/* Com_Renderer */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
			TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
			throw "Com_Renderer";

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_UVSphere"),
			TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer))))
			throw "Com_Buffer";

		/* For.Com_Shader */
		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_Shader_Protego"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
			throw "Com_Shader";

		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_Texture_VFX_T_Circle_LerpMask_D"),
			TEXT("Com_VFX_T_Circle_LerpMask_D"), reinterpret_cast<CComponent**>(&m_pTexture[0]))))
			throw "Com_VFX_T_Circle_LerpMask_D";

		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_Texture_VFX_T_Noise04_D"),
			TEXT("Com_VFX_T_Noise04_D.VFX_T_Noise04_D"), reinterpret_cast<CComponent**>(&m_pTexture[1]))))
			throw "Com_VFX_T_Noise04_D";

		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_Texture_VFX_T_RibbonOffset_N"),
			TEXT("Com_VFX_T_RibbonOffset_N"), reinterpret_cast<CComponent**>(&m_pTexture[2]))))
			throw "Com_VFX_T_RibbonOffset_N";

		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_Texture_VFX_T_Inky_Smoke_D"),
			TEXT("Com_VFX_T_Inky_Smoke_D"), reinterpret_cast<CComponent**>(&m_pTexture[3]))))
			throw "Com_VFX_T_Inky_Smoke_D";

		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_Texture_VFX_T_Wisps_2_D"),
			TEXT("Com_VFX_VFX_T_Wisps_2_D"), reinterpret_cast<CComponent**>(&m_pTexture[4]))))
			throw "Com_VFX_VFX_T_Wisps_2_D";

		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_DefaultConeBoom_Particle"),
			TEXT("Com_DefaultConeBoom_Particle"), reinterpret_cast<CComponent**>(&m_pDefaultConeBoom_Particle))))
			throw "Com_DefaultConeBoom_Particle";

		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_FireBlastFB_Loop_D_Flipbook"),
			TEXT("Prototype_GameObject_FireBlastFB_Loop_D_Flipbook"), reinterpret_cast<CComponent**>(&m_pFlameBlastFlipbook))))
			throw "Com_FireBlastFB_Loop_D_Flipbook";
	}
	catch (const char* pErrorMessage)
	{
		string strErrorMessage = "Failed to Cloned in CProtego_Effect : ";
		strErrorMessage += pErrorMessage;
		MSG_BOX(strErrorMessage.data());
	}

	return S_OK;
}

HRESULT CProtego_Effect::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_float3 vSphereWorldPos = m_pTransform->Get_Position();

	// Matrices
	FAILED_CHECK_RETURN(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr()), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ)), E_FAIL);

	// Textures
	FAILED_CHECK_RETURN(m_pTexture[0]->Bind_ShaderResource(m_pShader, "g_Circle_LayerMask_Texture"), E_FAIL);
	FAILED_CHECK_RETURN(m_pTexture[1]->Bind_ShaderResource(m_pShader, "g_Noise04_Texture"), E_FAIL);
	FAILED_CHECK_RETURN(m_pTexture[2]->Bind_ShaderResource(m_pShader, "g_RibbonOffset_Texture"), E_FAIL);
	FAILED_CHECK_RETURN(m_pTexture[3]->Bind_ShaderResource(m_pShader, "g_Inky_Smoke_Texture"), E_FAIL);
	FAILED_CHECK_RETURN(m_pTexture[4]->Bind_ShaderResource(m_pShader, "g_Wisps_2_Texture"), E_FAIL);
	//FAILED_CHECK_RETURN(m_pTexture[3]->Bind_ShaderResource(m_pShader, "g_Collision_Texture"), E_FAIL);
	FAILED_CHECK_RETURN(m_pFlameBlastFlipbook->Bind_DiffuseTexture(m_pShader, "g_Collision_Texture"), E_FAIL);
	
	// RawValues
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_vCamPos", pGameInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_vColor1", &m_vColor1, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_vColor2", &m_vColor2, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_fTime", &m_fTimeAcc, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_fRimPower", &m_fRimPower, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_vCollisionPoint", &m_vCollisionPoint, sizeof(_float3)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_iWidthLength", m_pFlameBlastFlipbook->Get_WidthLengthPtr(), sizeof(_uint)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_iHeightLength", m_pFlameBlastFlipbook->Get_HeightLengthPtr(), sizeof(_uint)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_iCurIndex", m_pFlameBlastFlipbook->Get_CurIndexPtr(), sizeof(_uint)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_fHitTimeAcc", &m_fHitTimeAcc, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_vSphereWorldPos", &vSphereWorldPos, sizeof(_float3)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_isHitEffect", &m_isHitEffect, sizeof(_bool)), E_FAIL);
	Safe_Release(pGameInstance);

	return S_OK;
}

CProtego_Effect* CProtego_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CProtego_Effect* pInstance = New CProtego_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CProtego_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProtego_Effect::Clone(void* pArg)
{
	CProtego_Effect* pInstance = New CProtego_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CProtego_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProtego_Effect::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pRenderer);
		Safe_Release(m_pShader);
		for (auto pTexture : m_pTexture)
			Safe_Release(pTexture);
		Safe_Release(m_pBuffer);
		Safe_Release(m_pDefaultConeBoom_Particle);
		Safe_Release(m_pFlameBlastFlipbook);
	}
}
