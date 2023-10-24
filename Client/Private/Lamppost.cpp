#include "..\Public\Lamppost.h"
#include "GameInstance.h"

CLamppost::CLamppost(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CLamppost::CLamppost(const CLamppost& rhs)
	: CMapObject(rhs)
{
}

HRESULT CLamppost::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLamppost::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CLamppost Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLamppost::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	// 자체 발광 추가
	CLight::LIGHTDESC		LightDescHork;
	ZeroMemory(&LightDescHork, sizeof LightDescHork);

	LightDescHork.eType = CLight::TYPE_POINT;
	LightDescHork.vPos = m_pTransform->Get_Position().TransCoord() + _float3(0.f, 2.f, 0.f);
	LightDescHork.fRange = 5.f;

	LightDescHork.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDescHork.vAmbient = LightDescHork.vDiffuse;
	LightDescHork.vSpecular = LightDescHork.vDiffuse;

	BEGININSTANCE;
	pGameInstance->Add_Light(LightDescHork, &m_pLight);
	ENDINSTANCE;

	return S_OK;
}

void CLamppost::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CLamppost::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}
}

HRESULT CLamppost::Render()
{
	if(FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModel->Bind_Material(m_pShader, "g_NormalTexture", iMeshCount, NORMALS);

		/*if (2 == iMeshCount)
		{
			m_vEmissive = _float4(0.5f, 0.5f, 0.45f, 0.5f);
			if (FAILED(m_pShader->Bind_RawValue("g_vBloom", &m_vEmissive, sizeof(_float4))))
				return E_FAIL;
		}*/

		m_pShader->Begin("Mesh_No_Cull");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

CLamppost* CLamppost::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLamppost* pInstance = New CLamppost(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CLamppost");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLamppost::Clone(void* pArg)
{
	CLamppost* pInstance = New CLamppost(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CLamppost");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLamppost::Free()
{
	__super::Free();

	Safe_Release(m_pLight);
}
