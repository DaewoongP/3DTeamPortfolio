#include "..\Public\ImpulseSphere_Effect.h"
#include "GameInstance.h"

// 두번째 지형은 -60.5이다.
CImpulseSphere_Effect::CImpulseSphere_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CImpulseSphere_Effect::CImpulseSphere_Effect(const CImpulseSphere_Effect& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
{
	m_pShader = static_cast<CShader*>(rhs.m_pShader->Clone(nullptr));
}

HRESULT CImpulseSphere_Effect::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		__debugbreak();
		return E_FAIL;
	}

	m_iLevel = iLevel;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ImpulseSphere.hlsl")
		, VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements);
	
	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Pulse_Rock")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Pulse_Rock")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Pulse/Rock/"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Pulse_SplashWater")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Pulse_SplashWater")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Pulse/SplashWater/"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Pulse_SplashWater2")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Pulse_SplashWater2")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Pulse/SplashWater2/"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Pulse_GlowSphere")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Pulse_GlowSphere")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Pulse/GlowSphere/"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Pulse_GlowSphere")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Pulse_GlowSphere")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Pulse/GlowSphere/"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}



	ENDINSTANCE;

	return S_OK;
}

HRESULT CImpulseSphere_Effect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pNoiseTexture = CTexture::Create(m_pDevice, m_pContext
		, TEXT("../../Resources/Effects/Textures/Noises/T_Noise_Perlin_01_N.png"));

	m_pTransform->Set_Position(_float3());
	m_pCoolTime = CCoolTime::Create(m_pDevice, m_pContext);
	m_pCoolTime->Set_MaxCoolTime(3.5f);
	m_pCoolTime->Set_Enable(false);

	return S_OK;
}

void CImpulseSphere_Effect::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	//ADD_IMGUI([&] {Tick_Imgui(fTimeDelta); })
#endif // _DEBUG

	if (false == m_isEnable)
		return;
	
	__super::Tick(fTimeDelta);
	m_pCoolTime->Tick(fTimeDelta);
	m_fScale += fTimeDelta * m_fRatio;
	m_pTransform->Set_Scale(_float3(m_fScale, m_fScale, m_fScale));

	m_fCircleRadius = sqrtf(m_fScale * m_fScale - m_fHeight * m_fHeight) + m_fCircleOffset;
	if (m_fScale > m_fHeight)
		m_isTouchGround = true;

	if (false == m_isStart && true == m_isTouchGround)
	{
		m_isStart = true;
		_float3 vPos = m_pTransform->Get_Position();
		_float3 vStartPos = _float3(vPos.x, m_fTerrainYPos, vPos.z);
		m_pPulse_Rock_Particle->Play(vStartPos);
		m_pPulse_SplashWater_Particle->Play(vStartPos);
		m_pPulse_SplashWater2_Particle->Play(vStartPos);
		m_pPulse_GlowSphere_Particle->Play(vStartPos);
	}

	// 셰이프매트릭스 가져와서 x,z 스케일 갱신하기.
	if (true == m_isStart)
	{
		_float4x4 ScaleMatrix = _float4x4::MatrixScale(m_fCircleRadius);
		m_pPulse_Rock_Particle->Get_ShapeModuleRef().ShapeMatrix = ScaleMatrix;
		m_pPulse_SplashWater_Particle->Get_ShapeModuleRef().ShapeMatrix = ScaleMatrix;
		m_pPulse_SplashWater2_Particle->Get_ShapeModuleRef().ShapeMatrix = ScaleMatrix;
		m_pPulse_GlowSphere_Particle->Get_ShapeModuleRef().ShapeMatrix = ScaleMatrix;
	}

	if (false == m_pCoolTime->IsEnable())
		Stop();
}

void CImpulseSphere_Effect::Late_Tick(_float fTimeDelta)
{
	if (false == m_isEnable)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	}
}

HRESULT CImpulseSphere_Effect::Render()
{
	Set_ShaderResources();
	FAILED_CHECK(m_pShader->Begin("ImpulseSphere"));
	FAILED_CHECK(m_pVIBuffer->Render());

	return S_OK;
}

void CImpulseSphere_Effect::Play(_float3 vPos, _float fTerrainYPos)
{
	m_pTransform->Set_Position(vPos); 
	m_fScale = 0.f;
	m_isEnable = true;
	m_isStart = false;
	m_isTouchGround = false;
	m_pCoolTime->Play_CoolTime();
	m_fTerrainYPos = fTerrainYPos;
	m_fHeight = fabs(m_pTransform->Get_Position().y - fTerrainYPos);
}

void CImpulseSphere_Effect::Stop()
{
	
	m_pPulse_Rock_Particle->Stop();
	m_pPulse_SplashWater_Particle->Stop();
	m_pPulse_SplashWater2_Particle->Stop();
	m_pPulse_GlowSphere_Particle->Stop();
}

void CImpulseSphere_Effect::Reset()
{
	m_pTransform->Set_Scale(_float3());
}
#ifdef _DEBUG
void CImpulseSphere_Effect::Tick_Imgui(_float fTimeDelta)
{
	CImGui_Manager::NextWindow_LeftBottom();
	ImGui::Begin("ImpulseSphere");
	CImGui_Manager::MatrixImgui(m_pTransform->Get_WorldMatrixRef(), "Impulse");
	ImGui::ColorEdit3("Color", (_float*)(&m_vColor));
	ImGui::DragFloat("FresnelPower", &m_fFresnelPower, 0.01f);
	ImGui::DragFloat("EmissivePower", &m_fEmissivePower, 0.001f);
	ImGui::DragFloat("Ratio", &m_fRatio, 0.001f);
	ImGui::DragFloat("CircleOffset", &m_fCircleOffset, 0.001f);
	if (ImGui::Button("Play"))
		Play(m_pTransform->Get_Position());

	if (ImGui::Button("Stop"))
		Stop();

	ImGui::End();
}
#endif // _DEBUG
HRESULT CImpulseSphere_Effect::Add_Components()
{
	FAILED_CHECK(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer")
		, TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_GeoSphere")
		, TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer)));
														   
	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Pulse_Rock")
		, TEXT("Com_Pulse_Rock_Particle"), reinterpret_cast<CComponent**>(&m_pPulse_Rock_Particle)));

	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Pulse_SplashWater")
		, TEXT("Com_Pulse_SplashWater_Particle"), reinterpret_cast<CComponent**>(&m_pPulse_SplashWater_Particle)));

	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Pulse_SplashWater2")
		, TEXT("Com_Pulse_SplashWater2_Particle"), reinterpret_cast<CComponent**>(&m_pPulse_SplashWater2_Particle)));

	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Pulse_GlowSphere")
		, TEXT("Com_Pulse_GlowSphere_Particle"), reinterpret_cast<CComponent**>(&m_pPulse_GlowSphere_Particle)));

	return S_OK;
}

HRESULT CImpulseSphere_Effect::Set_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// Matrix
	FAILED_CHECK(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr()));
	FAILED_CHECK(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ)));

	// Texture
	FAILED_CHECK(m_pNoiseTexture->Bind_ShaderResource(m_pShader, "g_NoiseTexture"));

	// RawValue
	FAILED_CHECK(m_pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof(m_vColor)));
	_float3 vCamPos = pGameInstance->Get_CamPosition()->xyz();
	FAILED_CHECK(m_pShader->Bind_RawValue("g_vCamPos", &vCamPos, sizeof(_float3)));
	FAILED_CHECK(m_pShader->Bind_RawValue("g_fFresnelPower", &m_fFresnelPower, sizeof(m_fFresnelPower)));
	FAILED_CHECK(m_pShader->Bind_RawValue("g_fEmissivePower", &m_fEmissivePower, sizeof(m_fEmissivePower)));
	_float fTimeAcc = pGameInstance->Get_World_TimeAcc();
	FAILED_CHECK(m_pShader->Bind_RawValue("g_fTimeAcc", &fTimeAcc, sizeof(fTimeAcc)));

	Safe_Release(pGameInstance);
	return S_OK;
}

CImpulseSphere_Effect* CImpulseSphere_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CImpulseSphere_Effect* pInstance = New CImpulseSphere_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Create CImpulseSphere_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CImpulseSphere_Effect::Clone(void* pArg)
{
	CImpulseSphere_Effect* pInstance = New CImpulseSphere_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone CImpulseSphere_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImpulseSphere_Effect::Free(void)
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pCoolTime);
	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pPulse_Rock_Particle);
	Safe_Release(m_pPulse_SplashWater_Particle);
	Safe_Release(m_pPulse_SplashWater2_Particle);
	Safe_Release(m_pPulse_GlowSphere_Particle);
}
