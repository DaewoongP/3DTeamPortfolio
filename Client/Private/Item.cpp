#include "..\Public\Item.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"
#include "CustomModel.h"

CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CItem::CItem(const CItem& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
	, m_ItemCreateDesc(rhs.m_ItemCreateDesc)
{
}

HRESULT CItem::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iLevel = iLevel;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel,
		m_ItemCreateDesc.wstrUIPath.data()), E_FAIL);

	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Model(m_pDevice, m_pContext, m_iLevel, m_ItemCreateDesc.eModelType
		, m_ItemCreateDesc.wstrModelPath.data()), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pPlayer = ((ITEM_CLONE_DESC*)pArg)->pPlayer;
	m_pPlayerModel = static_cast<CCustomModel*>(m_pPlayer->Find_Component(TEXT("Com_Model_CustomModel_Player")));
	m_pPlayerTransform = m_pPlayer->Get_Transform();
	m_pPlayerInformation = static_cast<CPlayer_Information*>(m_pPlayer->Find_Component(TEXT("Com_Player_Information")));

	return S_OK;
}

HRESULT CItem::Add_Components()
{
	FAILED_CHECK_RETURN(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer")
		, TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer)), E_FAIL);

	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_ItemCreateDesc.wstrUIPath.data()).c_str()
		, TEXT("Com_UITexture"), reinterpret_cast<CComponent**>(&m_pUITexture)), E_FAIL);

	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, ToPrototypeTag(TEXT("Prototype_Component_Model"), m_ItemCreateDesc.wstrModelPath.data()).c_str()
		, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)), E_FAIL);

	if (CModel::TYPE_ANIM == m_ItemCreateDesc.eModelType)
	{
		FAILED_CHECK_RETURN(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh")
			, TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader)), E_FAIL);
	}
	else
	{
		FAILED_CHECK_RETURN(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh")
			, TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader)), E_FAIL);
	}

	return S_OK;
}

void CItem::Free(void)
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pRenderer);
		Safe_Release(m_pUITexture);
		Safe_Release(m_pModel);
		Safe_Release(m_pShader);
	}
}
