#include "Weapon_DarkWizard_Wand.h"
#include "GameInstance.h"

CWeapon_DarkWizard_Wand::CWeapon_DarkWizard_Wand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CParts(pDevice, pContext)
{
}

CWeapon_DarkWizard_Wand::CWeapon_DarkWizard_Wand(const CWeapon_DarkWizard_Wand& rhs)
	: CParts(rhs)
{
}

HRESULT CWeapon_DarkWizard_Wand::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CWeapon_DarkWizard_Wand::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	return S_OK;
}

HRESULT CWeapon_DarkWizard_Wand::Initialize_Level(_uint iCurrentLevelIndex)
{
	if (FAILED(Add_Components_Level(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CWeapon_DarkWizard_Wand::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (true == m_isDissolve)
	{
		m_fDissolveAmount += fTimeDelta / 1.5f; // ������ �� ����
	}
}

void CWeapon_DarkWizard_Wand::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		if (false == m_isDissolve)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}
}

HRESULT CWeapon_DarkWizard_Wand::Render()
{
	if (FAILED(Set_Shader_Resources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE)))
				throw TEXT("Failed Bind_Material : g_DiffuseTexture");

			if (false == m_isDissolve)
			{
				if (FAILED(m_pShaderCom->Begin("Mesh")))
					throw TEXT("Failed Begin : Mesh");
			}
			else
			{
				if (FAILED(m_pShaderCom->Begin("Mesh_Dissolve")))
					throw TEXT("Failed Begin : Mesh_Dissolve");
			}

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Failed Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CWeapon_DarkWizard_Wand] Failed Render : \n");
			wstrErrorMSG += pErrorTag;
			MSG_BOX(wstrErrorMSG.c_str());
			__debugbreak();

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CWeapon_DarkWizard_Wand::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	if (FAILED(SetUp_ShadowShaderResources(LightViewMatrix, LightProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try /* Failed Render */
		{
			if (FAILED(m_pShadowShaderCom->Begin("Shadow")))
				throw TEXT("Shader Begin Shadow");

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Model Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CEnemy] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CWeapon_DarkWizard_Wand::Add_Components(void* pArg)
{
	try /* Check Add_Components */
	{
		/* For.Com_Renderer */
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
			(CComponent**)&m_pRendererCom, this)))
			throw TEXT("Failed Add_Component : Com_Renderer");

		/* For.Com_Shader_Mesh */
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader_Mesh"),
			(CComponent**)&m_pShaderCom, this)))
			throw TEXT("Failed Add_Component : Com_Shader_Mesh");

		/* For.Com_ShadowShader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowMesh"),
			TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShaderCom))))
			throw TEXT("Com_ShadowShader");

		/* For.Dissolve_Texture */
		m_pDissolveTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/VFX/Textures/Noises/VFX_T_NoiseGreypack02_D.png"));
		if (nullptr == m_pDissolveTexture)
			throw TEXT("m_pDissolveTexture is nullptr");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CWeapon_DarkWizard_Wand] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWeapon_DarkWizard_Wand::Add_Components_Level(_uint iCurrentLevelIndex)
{
	if (FAILED(Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Model_Weopon_DarkWizard_Wand"), L"Com_Model",
		(CComponent**)&m_pModelCom, this)))
	{
		wstring wstrErrorMSG = TEXT("[CWeapon_DarkWizard_Wand] Failed Add_Components_Level : \n");
		wstrErrorMSG += TEXT("Failed Add_Component : Com_Model");
		MSG_BOX(wstrErrorMSG.c_str());

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWeapon_DarkWizard_Wand::Set_Shader_Resources()
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

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
			throw TEXT("Failed Bind_RawValue : m_fDissolveAmount");

		if (FAILED(m_pDissolveTexture->Bind_ShaderResources(m_pShaderCom, "g_DissolveTexture")))
			throw TEXT("Failed Bind_RawValue : g_DissolveTexture");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CWeapon_DarkWizard_Wand] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CWeapon_DarkWizard_Wand::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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
		wstring wstrErrorMSG = TEXT("[CEnemy] Failed SetUp_ShadowShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

CWeapon_DarkWizard_Wand* CWeapon_DarkWizard_Wand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_DarkWizard_Wand* pInstance = New CWeapon_DarkWizard_Wand(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CWeapon_DarkWizard_Wand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CWeapon_DarkWizard_Wand* CWeapon_DarkWizard_Wand::Clone(void* pArg)
{
	CWeapon_DarkWizard_Wand* pInstance = New CWeapon_DarkWizard_Wand(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CWeapon_DarkWizard_Wand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWeapon_DarkWizard_Wand::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pModelCom);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRendererCom);
		Safe_Release(m_pShadowShaderCom);
		Safe_Release(m_pDissolveTexture);
	}
}
