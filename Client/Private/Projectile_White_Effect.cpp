#include "..\Public\Projectile_White_Effect.h"
#include "GameInstance.h"

CProjectile_White_Effect::CProjectile_White_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CProjectile_White_Effect::CProjectile_White_Effect(const CProjectile_White_Effect& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
{
}

HRESULT CProjectile_White_Effect::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iLevel = iLevel;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_Component_Shader_Projectile_White")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Shader_Projectile_White"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Projectile_White.hlsl")
			, VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
		{
			__debugbreak();
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_Component_Model_WindingSphere")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Model_WindingSphere"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM
				, TEXT("../../Resources/Models/NonAnims/WindingSphere/WindingSphere.dat")))))
		{
			__debugbreak();
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CProjectile_White_Effect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CProjectile_White_Effect::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	ADD_IMGUI([&] {Tick_Imgui(fTimeDelta); });
#endif // _DEBUG

	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	m_vCamPos =  pGameInstance->Get_CamPosition()->xyz();
	Safe_Release(pGameInstance);

	_float3 vAxis = m_vCamPos - m_pTransform->Get_Position();
	vAxis.Normalize();
	if (_float3::Zero != vAxis)
	{
		m_pTransform->Turn(vAxis, XMConvertToRadians(720.f), fTimeDelta);
	}
	
}

void CProjectile_White_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_isGlow)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_GLOW, this);
	}
	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CProjectile_White_Effect::Render()
{
	if (FAILED(Set_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Default")))
		return E_FAIL;

	_uint iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", i, TextureType::DIFFUSE);
		m_pModel->Bind_Material(m_pShader, "g_NormalTexture", i, TextureType::NORMALS);

		if (FAILED(m_pModel->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

#ifdef _DEBUG
void CProjectile_White_Effect::Tick_Imgui(_float fTimeDelta)
{
	CImGui_Manager::NextWindow_LeftBottom();
	ImGui::Begin("CProjectile_White_Effect");
	ImGui::Checkbox("Glow", &m_isGlow);
	ImGui::Checkbox("Blur", &m_isBlur);
	ImGui::DragFloat("EmissiveLength", (_float*)&m_fEmissiveStrength, 0.1f);
	ImGui::DragFloat2("Tiling", (_float*)&m_vTiling, 0.01f);
	ImGui::DragFloat2("Offset", (_float*)&m_vOffset, 0.01f);
	ImGui::ColorEdit4("Color", (_float*)&m_vColor);
	ImGui::End();
}
#endif // _DEBUG



HRESULT CProjectile_White_Effect::Add_Components()
{
	FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer")
		, TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer)));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_Model_WindingSphere")
		, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_Shader_Projectile_White")
		, TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader)));

	m_pEmissiveTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/VFX_T_FirePlasma_D.png"));

	return S_OK;
}

HRESULT CProjectile_White_Effect::Set_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// Matrix
	FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr()));
	FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)));
	FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ)));

	// Texture
	FAILED(m_pEmissiveTexture->Bind_ShaderResource(m_pShader, "g_EmissiveTexture"));

	// RawValue
	_float fTimeAcc = pGameInstance->Get_World_TimeAcc();
	FAILED(m_pShader->Bind_RawValue("g_fTimeAcc", &fTimeAcc, sizeof(fTimeAcc)));
	FAILED(m_pShader->Bind_RawValue("g_fEmissiveStrength", &m_fEmissiveStrength, sizeof(m_fEmissiveStrength)));
	FAILED(m_pShader->Bind_RawValue("g_isBlur", &m_isBlur, sizeof(m_isBlur)));
	FAILED(m_pShader->Bind_RawValue("g_vTiling", &m_vTiling, sizeof(m_vTiling)));
	FAILED(m_pShader->Bind_RawValue("g_vOffset", &m_vOffset, sizeof(m_vOffset)));
	FAILED(m_pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof(m_vColor)));

	Safe_Release(pGameInstance);
	return S_OK;
}

CProjectile_White_Effect* CProjectile_White_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CProjectile_White_Effect* pInstance = New CProjectile_White_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Create CProjectile_White_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CProjectile_White_Effect::Clone(void* pArg)
{
	CProjectile_White_Effect* pInstance = New CProjectile_White_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone CProjectile_White_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CProjectile_White_Effect::Free(void)
{
	__super::Free();


	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pEmissiveTexture);
}
