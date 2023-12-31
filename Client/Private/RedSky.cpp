#include "..\Public\RedSky.h"
#include "GameInstance.h"

CRedSky::CRedSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CRedSky::CRedSky(const CRedSky& rhs)
	: CGameObject(rhs)
{
}

HRESULT CRedSky::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Scale(_float3(5.f, 5.f, 5.f));

	return S_OK;
}

void CRedSky::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CRedSky::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pTransform->Set_Position(pGameInstance->Get_CamPosition()->xyz());

	Safe_Release(pGameInstance);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CRedSky::Render()
{
	FAILED_CHECK(SetUp_ShaderResources());

	FAILED_CHECK(m_pShader->Begin("Sky"));

	FAILED_CHECK(m_pBuffer->Render());

	return S_OK;
}

HRESULT CRedSky::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CRedSky Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CRedSky Add_Component : (Com_Shader)");
		return E_FAIL;
	}
	
	/* For.Com_Buffer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer))))
	{
		MSG_BOX("Failed CRedSky Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	m_pTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Sky/RedSpace.dds"));

	return S_OK;
}

HRESULT CRedSky::SetUp_ShaderResources()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CRedSky* CRedSky::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRedSky* pInstance = New CRedSky(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRedSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRedSky::Clone(void* pArg)
{
	CRedSky* pInstance = New CRedSky(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CRedSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRedSky::Free()
{
	__super::Free();

	Safe_Release(m_pBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pRenderer);
}