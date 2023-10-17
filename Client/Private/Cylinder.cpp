#include "Cylinder.h"

#include "GameInstance.h"

CCylinder::CCylinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCylinder::CCylinder(const CCylinder& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCylinder::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCylinder::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CCylinder::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCylinder::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	return;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}

	Safe_Release(pGameInstance);
}

HRESULT CCylinder::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModel->Bind_Material(m_pShader, "g_NormalTexture", iMeshCount, NORMALS);

		m_pShader->Begin("Mesh");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

void CCylinder::Reset(const CYLINDERINITDESC& tagResetDesc)
{
	m_pTransform->Set_Position(tagResetDesc.vPosition);
	m_pTransform->Rotation(tagResetDesc.vRight, tagResetDesc.fRadian);
	m_pTransform->Set_Scale(tagResetDesc.vSacle);
}

HRESULT CCylinder::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CCylinder Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CCylinder Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Cylinder"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CCylinder Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCylinder::SetUp_ShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

CCylinder* CCylinder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCylinder* pInstance = New CCylinder(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCylinder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCylinder::Clone(void* pArg)
{
	CCylinder* pInstance = New CCylinder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCylinder");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCylinder::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pShader);
		Safe_Release(m_pModel);
		Safe_Release(m_pRenderer);
	}
}
