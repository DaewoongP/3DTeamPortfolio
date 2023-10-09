#include "..\Public\RadialBlur.h"
#include "GameInstance.h"

CRadialBlur::CRadialBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CRadialBlur::CRadialBlur(const CRadialBlur& rhs)
	: CGameObject(rhs)
{
}

HRESULT CRadialBlur::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRadialBlur::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Position(_float3(25.f, 3.f, 22.5f));

	return S_OK;
}

void CRadialBlur::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CRadialBlur::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DISTORTION, this);
	}
}

HRESULT CRadialBlur::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Radial")))
		return E_FAIL;

	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRadialBlur::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	m_pRenderer = static_cast<CRenderer*>(pGameInstance->Clone_Component(0, TEXT("Prototype_Component_Renderer")));
	Safe_Release(pGameInstance);

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_RadialBlur.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);

	m_pBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);

	if (nullptr == m_pShader ||
		nullptr == m_pRenderer ||
		nullptr == m_pBuffer)
		return E_FAIL;

	return S_OK;
}

HRESULT CRadialBlur::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CRadialBlur* CRadialBlur::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRadialBlur* pInstance = New CRadialBlur(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRadialBlur");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRadialBlur::Clone(void* pArg)
{
	CRadialBlur* pInstance = New CRadialBlur(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CRadialBlur");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRadialBlur::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pBuffer);
}
