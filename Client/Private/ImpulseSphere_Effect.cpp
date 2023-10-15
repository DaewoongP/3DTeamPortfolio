#include "..\Public\ImpulseSphere_Effect.h"
#include "GameInstance.h"

CImpulseSphere_Effect::CImpulseSphere_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CImpulseSphere_Effect::CImpulseSphere_Effect(const CImpulseSphere_Effect& rhs)
	: CGameObject(rhs)
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

	return S_OK;
}

HRESULT CImpulseSphere_Effect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		__debugbreak();
		return E_FAIL;
	}

	Add_Components();

	m_pTransform->Set_Position(_float3());
	m_pCoolTime = CCoolTime::Create(m_pDevice, m_pContext);
	m_pCoolTime->Set_MaxCoolTime(3.f);
	m_pCoolTime->Set_Enable(false);

	return S_OK;
}

void CImpulseSphere_Effect::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	ADD_IMGUI([&] {Tick_Imgui(fTimeDelta); })
#endif // _DEBUG

	if (false == m_isEnable)
		return;

	__super::Tick(fTimeDelta);
	m_pCoolTime->Tick(fTimeDelta);
	_float fRatio = 6.f;
	m_fScale += fTimeDelta * fRatio;
	m_pTransform->Set_Scale(_float3(m_fScale, m_fScale, m_fScale));

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
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_GLOW, this);
	}
}

HRESULT CImpulseSphere_Effect::Render()
{
	Set_ShaderResources();
	FAILED_CHECK(m_pShader->Begin("ImpulseSphere"));
	FAILED_CHECK(m_pVIBuffer->Render());

	return S_OK;
}

void CImpulseSphere_Effect::Play(_float3 vPos)
{
	m_pTransform->Set_Position(vPos); 
	m_fScale = 0.f;
	m_isEnable = true;
	m_pCoolTime->Play_CoolTime();
}

void CImpulseSphere_Effect::Stop()
{
	m_isEnable = false;
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
	ImGui::DragFloat("RimPower", &m_fRimPower, 0.01f);

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

	return S_OK;
}

HRESULT CImpulseSphere_Effect::Set_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr()));
	FAILED_CHECK(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ)));
	FAILED_CHECK(m_pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof(m_vColor)));
	FAILED_CHECK(m_pShader->Bind_RawValue("g_vCamPos", pGameInstance->Get_CamPosition(), sizeof(_float4)));
	FAILED_CHECK(m_pShader->Bind_RawValue("g_fRimPower", &m_fRimPower, sizeof(m_fRimPower)));

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
}
