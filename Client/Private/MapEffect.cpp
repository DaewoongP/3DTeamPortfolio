#include "MapEffect.h"
#include"GameInstance.h"
#include"Texture.h"
#include"Shader.h"

CMapEffect::CMapEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext) :CGameObject(_pDevice, _pContext)
{
}

CMapEffect::CMapEffect(const CMapEffect& _rhs) : CGameObject(_rhs)
{
}

HRESULT CMapEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapEffect::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CMapObject Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MAPOBJECTDESC* pMapObjectDesc = reinterpret_cast<MAPOBJECTDESC*>(pArg);
	m_pTransform->Set_WorldMatrix((*pMapObjectDesc).WorldMatrix);

	if (nullptr == pMapObjectDesc)
	{
		MSG_BOX("Object Desc is NULL");
		return E_FAIL;
	}

	if (FAILED(Add_Components(pMapObjectDesc)))
		return E_FAIL;

	return S_OK;
}

void CMapEffect::Tick(_float _fTimeDelta)
{
	m_vOffset.x += 0.3f * _fTimeDelta;
}

void CMapEffect::Late_Tick(_float _fTimeDelta)
{
	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DISTORTION, this);
	}
}

HRESULT CMapEffect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; iMeshCount++)
	{
		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModel->Bind_Material(m_pShader, "g_NormalTexture", iMeshCount, NORMALS);

		m_pShader->Begin("Effect_Mesh");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapEffect::Add_Components(MAPOBJECTDESC* pMapObjectDesc)
{
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, pMapObjectDesc->wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Model)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapEffect::Setup_ShaderResources()
{
	BEGININSTANCE
		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
			return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vOffset", &m_vOffset, sizeof m_vOffset)))
		return E_FAIL;

	/*if (FAILED(m_pShader->Bind_RawValue("g_vTililing", &m_vTililing, sizeof m_vTililing)))
		return E_FAIL;*/

	ENDINSTANCE

		return S_OK;
}

CMapEffect* CMapEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapEffect* pInstance = New CMapEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapEffect::Clone(void* pArg)
{
	CMapEffect* pInstance = New CMapEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMapEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMapEffect::Free()
{
	__super::Free();

	Safe_Release(m_pRigidBody);
	Safe_Release(m_pTransform);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}