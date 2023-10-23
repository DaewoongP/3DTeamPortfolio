#include "../Public/Sky.h"
#include "GameInstance.h"

CSky::CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSky::CSky(const CSky& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSky::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSky::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Scale(_float3(5.f, 5.f, 5.f));
	m_vMoonPos = _float2(0.8f, 0.2f);

	return S_OK;
}

HRESULT CSky::Initialize_Level(_uint iCurrentLevelIndex)
{
	m_iCurrentLevelIndex = (LEVELID)iCurrentLevelIndex;

	return S_OK;
}

void CSky::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	ADD_IMGUI([&] { this->Debug(); });
#endif // _DEBUG

	__super::Tick(fTimeDelta);
}

void CSky::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pTransform->Set_Position(pGameInstance->Get_CamPosition()->xyz());

	Safe_Release(pGameInstance);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CSky::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", i, DIFFUSE)))
			return E_FAIL;

		if (LEVEL_VAULT == m_iCurrentLevelIndex)
		{
			if (FAILED(m_pShader->Begin("VaultSky")))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShader->Begin("Sky")))
				return E_FAIL;
		}

		if (FAILED(m_pModel->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSky::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CSky Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CSky Add_Component : (Com_Shader)");
		return E_FAIL;
	}
	
	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sky"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CSky Add_Component : (Com_Model)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSky::SetUp_ShaderResources()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	_float fTimeAcc = pGameInstance->Get_World_TimeAcc();
	Safe_Release(pGameInstance);

	if (FAILED(m_pShader->Bind_RawValue("g_vMoonPos", &m_vMoonPos, sizeof(_float2))))
		return E_FAIL;
	if (g_iTest > 1)
	{
		m_isNight = true;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_isNight", &m_isNight, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_iFrame", &m_iFrame, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fTime", &fTimeAcc, sizeof(_float))))
		return E_FAIL;
	m_iFrame++;

	return S_OK;
}

#ifdef _DEBUG
void CSky::Debug()
{
	ImGui::SetNextWindowPos(ImVec2(0.f, 800.f));
	ImGui::SetNextWindowSize(ImVec2(300.f, 400.f));
	ImGui::Begin("Sky");

	ImGui::SliderFloat2("sky", reinterpret_cast<_float*>(&m_vMoonPos), -2.f, 2.f);

	ImGui::End();
}
#endif // _DEBUG

CSky* CSky::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSky* pInstance = New CSky(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSky::Clone(void* pArg)
{
	CSky* pInstance = New CSky(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pModel);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
}