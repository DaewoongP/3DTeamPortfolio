#include "..\Public\MeshEffect.h"
#include "Model.h"
#include "Texture.h"
#include "Shader.h"
#include "GameInstance.h"

CMeshEffect::CMeshEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
	m_Path[TEXTURE_PATH] = TEXT("../../Resources/NonAnims/SM_SpherePrimitiveRegularNormals_01/T_Default_Material_Grid_M.png");
	m_Path[MODEL_PATH] = TEXT("../../Resources/NonAnims/SM_SpherePrimitiveRegularNormals_01/SM_SpherePrimitiveRegularNormals_01.dat");
	m_Path[SHADER_PATH] = TEXT("../Bin/ShaderFiles/Shader_DefaultEffect.hlsl");
}

CMeshEffect::CMeshEffect(const CMeshEffect& _rhs)
	: CGameObject(_rhs)
{
	for (_uint i = 0; i < PATH_END; ++i)
		m_Path[i] = _rhs.m_Path[i];
}

HRESULT CMeshEffect::Initialize_Prototype(const _tchar* _pDirectoryPath)
{
	__super::Initialize_Prototype();
	// ���⼭ m_Path�ε����ֱ�.

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));

	return S_OK;
}

HRESULT CMeshEffect::Initialize(void* _pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;


	return S_OK;
}

void CMeshEffect::Tick(_float _fTimeDelta)
{
	//m_pModel->Play_Animation(_fTimeDelta);
	//m_eMeshEffectDesc.vOffset.x += 1.f * _fTimeDelta;
	m_eMeshEffectDesc.vOffset.y += 1.f * _fTimeDelta;
}

void CMeshEffect::Late_Tick(_float _fTimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CMeshEffect::Render()
{
	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(m_strPassName.c_str())))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", i, TextureType::DIFFUSE);
		if (FAILED(m_pModel->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

MESHEFFECT* CMeshEffect::Get_MeshEffect_Desc()
{
	return &m_eMeshEffectDesc;
}

void CMeshEffect::Set_PassName(string strPassName)
{
	m_strPassName = strPassName;
}

void CMeshEffect::Set_Path(wstring wstrPath, PATH ePath)
{
	Clamp(ePath, TEXTURE_PATH, static_cast<PATH>(PATH_END - 1));
	m_Path[ePath] = wstrPath;
}

HRESULT CMeshEffect::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
	{
		MSG_BOX("Failed CSky Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(0, ToPrototypeTag(TEXT("Prototype_Component_Model"), m_Path[MODEL_PATH].c_str()).c_str()
		, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed to Add Component in CMeshEffect");
		return E_FAIL;
	}

	/* For.Com_MainTexture */
	if (FAILED(CComposite::Add_Component(0, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_Path[TEXTURE_PATH].c_str()).c_str()
		, TEXT("Com_MainTexture"), reinterpret_cast<CComponent**>(&m_pTexture))))
	{
		MSG_BOX("Failed to Add Component in CMeshEffect");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(0, ToPrototypeTag(TEXT("Prototype_Component"), m_Path[SHADER_PATH].c_str()).c_str()
		, TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed to Add Component in CMeshEffect");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMeshEffect::Setup_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CGameObject* pOwnerObject = dynamic_cast<CGameObject*>(m_pOwner);
	_float4x4 WorldMatirx = m_pTransform->Get_WorldMatrix();

	if (nullptr != pOwnerObject)
	{
		_float4x4 OwnerWorldMatirx = pOwnerObject->Get_Transform()->Get_WorldMatrix();
		WorldMatirx *= OwnerWorldMatirx;
	}

	if (FAILED(m_pShader->Bind_RawValue("g_vOffset", &m_eMeshEffectDesc.vOffset, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatirx)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	return S_OK;
}
CMeshEffect* CMeshEffect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pDirectoryPath)
{
	CMeshEffect* pInstance = new CMeshEffect(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pDirectoryPath)))
	{
		MSG_BOX("Failed to Created CMeshEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CMeshEffect::Clone(void* _pArg)
{
	CMeshEffect* pInstance = new CMeshEffect(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned CMeshEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CMeshEffect::Free()
{
	__super::Free();
	Safe_Release(m_pTexture);
	Safe_Release(m_pModel);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
}