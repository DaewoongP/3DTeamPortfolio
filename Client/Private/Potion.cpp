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
	m_ItemCreateDesc.eItemType = ITEMTYPE::POTION;
	m_ItemCreateDesc.eModelType = CModel::TYPE_NONANIM; // ¸ðµ¨ Å¸ÀÔ
	m_ItemCreateDesc.PivotMatrix = _float4x4(); // ÇÇ¹þ ¸ÅÆ®¸¯½º

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPotion::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPotion::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// »À¿¡ ºÎÂø.
	m_WorldMatrix = m_pTransform->Get_WorldMatrix() *
		m_pPlayerModel->Get_Attaching_Bone_Matrix(0) * m_pPlayerTransform->Get_WorldMatrix();
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
		wstring wstrErrorMSG = TEXT("[CWeapon_Player_Wand] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;
}

void CPotion::Free(void)
{
	__super::Free();
}
