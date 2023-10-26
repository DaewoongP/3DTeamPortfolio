#include "..\Public\Lamp_Wall.h"
#include "GameInstance.h"

CLamp_Wall::CLamp_Wall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CLamp_Wall::CLamp_Wall(const CLamp_Wall& rhs)
	: CMapObject(rhs)
{
}

HRESULT CLamp_Wall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLamp_Wall::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CLamp_Wall Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLamp_Wall::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CLamp_Wall Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	// 자체 발광 추가
	CLight::LIGHTDESC		LightDescHork;
	ZeroMemory(&LightDescHork, sizeof LightDescHork);

	LightDescHork.eType = CLight::TYPE_POINT;
	LightDescHork.vPos = m_pTransform->Get_Position().TransCoord() + _float3(0.f, -2.f, 0.f);
	LightDescHork.fRange = 10.f;

	LightDescHork.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDescHork.vAmbient = LightDescHork.vDiffuse;
	LightDescHork.vSpecular = LightDescHork.vDiffuse;

	BEGININSTANCE;
	pGameInstance->Add_Light(LightDescHork, &m_pLight);
	ENDINSTANCE;

	return S_OK;
}

void CLamp_Wall::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CLamp_Wall::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}
}

HRESULT CLamp_Wall::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModel->Bind_Material(m_pShader, "g_NormalTexture", iMeshCount, NORMALS);

		if (1 == iMeshCount)
		{
			m_vEmissive = _float4(0.5f, 0.5f, 0.45f, 0.5f);
			if (FAILED(m_pShader->Bind_RawValue("g_vBloom", &m_vEmissive, sizeof(_float4))))
				return E_FAIL;
		}

		else
		{
			m_vEmissive = _float4(0.f, 0.f, 0.f, 0.f);
			if (FAILED(m_pShader->Bind_RawValue("g_vBloom", &m_vEmissive, sizeof(_float4))))
				return E_FAIL;
		}

		m_pShader->Begin("Mesh_No_Cull");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

CLamp_Wall* CLamp_Wall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLamp_Wall* pInstance = New CLamp_Wall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CLamp_Wall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLamp_Wall::Clone(void* pArg)
{
	CLamp_Wall* pInstance = New CLamp_Wall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CLamp_Wall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLamp_Wall::Free()
{
	__super::Free();

	Safe_Release(m_pLight);
}
