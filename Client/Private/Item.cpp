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

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 모든 레벨을 조사해서 플레이어 주소를 가져옴.
	for (_uint i = 0; i < LEVEL_END; ++i)
	{
		m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(
			i
			, TEXT("Layer_Player")
			, TEXT("GameObject_Player")));

		if (m_pPlayer != nullptr)
			break;
	}
	
	// 플레이어 주소에 대한 유효성 검사.
	if (nullptr == m_pPlayer)
	{
		__debugbreak();
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	// 필요한 플레이어의 컴포넌트들을 받아옴.
	m_pPlayerModel = static_cast<CCustomModel*>(m_pPlayer->Find_Component(TEXT("Com_Model_CustomModel_Player")));
	m_pPlayerTransform = m_pPlayer->Get_Transform();
	m_pPlayerInformation = static_cast<CPlayer_Information*>(m_pPlayer->Find_Component(TEXT("Com_Player_Information")));
	
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(_float(g_iWinSizeX), _float(g_iWinSizeY), 0.f, 100.f));

	return S_OK;
}

HRESULT CItem::Change_Position(_float fX, _float fY, _float fZ, _uint iWinSizeX, _uint iWinSizeY)
{
	m_pTransform->Set_Position(
		XMVectorSet(fX - iWinSizeX * 0.5f, -fY + iWinSizeY * 0.5f, fZ, 1.f));

	return S_OK;
}

HRESULT CItem::Change_Scale(_float fX, _float fY)
{
	m_pTransform->Set_Scale(_float3(fX, fY, 1.f));

	return S_OK;
}

void CItem::Tick(_float fTimeDelta)
{
	if (false == m_isEnable)
		return;

	__super::Tick(fTimeDelta);
}

void CItem::Late_Tick(_float fTimeDelta)
{
	if (false == m_isEnable)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CItem::Render()
{
	if (FAILED(Set_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pUIShader->Begin("Dynamic_Back")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

void CItem::ToLayer(_int iLevel, const _tchar* ComTag, const _tchar* LayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_uint iLevelIndex = iLevel;

	if (-1 == iLevelIndex)
	{
		iLevelIndex = pGameInstance->Get_CurrentLevelIndex();
	}

	if (FAILED(pGameInstance->Add_Component(this, iLevelIndex, LayerTag, ComTag)))
	{
		__debugbreak();
		Safe_Release(pGameInstance);
		return;
	}

	Safe_Release(pGameInstance);
}

HRESULT CItem::Add_Components()
{
	FAILED_CHECK(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer")
		, TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer)));

	FAILED_CHECK(CComposite::Add_Component(m_iLevel, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_ItemCreateDesc.wstrUIPath.data()).c_str()
		, TEXT("Com_UITexture"), reinterpret_cast<CComponent**>(&m_pUITexture)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex")
		, TEXT("Com_UIShader"), reinterpret_cast<CComponent**>(&m_pUIShader)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect")
		, TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer)));

	return S_OK;
}

HRESULT CItem::Set_ShaderResources()
{
	FAILED_CHECK(m_pUIShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr()));
	FAILED_CHECK(m_pUIShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pUIShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));
	FAILED_CHECK(m_pUITexture->Bind_ShaderResource(m_pUIShader, "g_Texture"));
	FAILED_CHECK(m_pUIShader->Bind_RawValue("g_fAlphaRatio", &m_fAlphaRatio, sizeof(m_fAlphaRatio)));

	return S_OK;
}

void CItem::Free(void)
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pRenderer);
		Safe_Release(m_pUITexture);
		Safe_Release(m_pUIShader);
		Safe_Release(m_pVIBuffer);
	}
}
