#include "..\Public\Potion.h"
#include "GameInstance.h"
#include "CoolTime.h"
#include "Player.h"

CPotion::CPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CPotion::CPotion(const CPotion& rhs)
	: CItem(rhs)
	, m_PotionCreateDesc(rhs.m_PotionCreateDesc)
{
}

HRESULT CPotion::Initialize_Prototype(_uint iLevel)
{
	// ¾ÆÀÌÅÛ Á¤º¸
	m_ItemCreateDesc.eItemType = ITEMTYPE::POTION;
	m_PotionCreateDesc.eModelType = CModel::TYPE_NONANIM; // ¸ðµ¨ Å¸ÀÔ
	m_PotionCreateDesc.PivotMatrix = _float4x4(); // ÇÇ¹þ ¸ÅÆ®¸¯½º

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Model(m_pDevice, m_pContext, m_iLevel, m_PotionCreateDesc.eModelType
		, m_PotionCreateDesc.wstrModelPath.data()), E_FAIL);

	Safe_Release(pGameInstance);

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPotion::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	const CBone* pBone = m_pPlayerModel->Get_Bone(TEXT("SKT_RightHand"));
	if (nullptr == pBone)
		return E_FAIL;

	m_iBoneIndex = pBone->Get_Index();

	return S_OK;
}

void CPotion::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// »À¿¡ ºÎÂø.
	m_WorldMatrix = m_pTransform->Get_WorldMatrix() *
		m_pPlayerModel->Get_Attaching_Bone_Matrix(m_iBoneIndex) * m_pPlayerTransform->Get_WorldMatrix();
}

void CPotion::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPotion::Render()
{
	if (FAILED(Set_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", i, DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModel->Bind_Material(m_pShader, "g_NormalTexture", i, NORMALS)))
			return E_FAIL;

		if (FAILED(m_pShader->Begin("Mesh")))
			return E_FAIL;

		if (FAILED(m_pModel->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPotion::Set_ShaderResources()
{
	BEGININSTANCE;

	try /* Check SetUp_ShaderResources */
	{
		if (nullptr == m_pShader)
			throw TEXT("m_pShaderCom is nullptr");

		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			throw TEXT("Failed Bind_Matrix : g_WorldMatrix");

		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
			throw TEXT("Failed Bind_Matrix : g_ViewMatrix");

		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
			throw TEXT("Failed Bind_Matrix : g_ProjMatrix");

		if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
			throw TEXT("Failed Bind_RawValue : g_fCamFar");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CPotion] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CPotion::Add_Components()
{
	FAILED_CHECK_RETURN(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer")
		, TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer)), E_FAIL);

	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, ToPrototypeTag(TEXT("Prototype_Component_Model"), m_PotionCreateDesc.wstrModelPath.data()).c_str()
		, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)), E_FAIL);

	FAILED_CHECK_RETURN(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh")
		, TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader)), E_FAIL);

	return S_OK;
}

void CPotion::Free(void)
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pRenderer);
		Safe_Release(m_pModel);
		Safe_Release(m_pShader);
	}
}
