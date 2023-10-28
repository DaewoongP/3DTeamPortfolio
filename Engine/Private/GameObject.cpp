#include "..\Public\GameObject.h"
#include "Level_Manager.h"
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CGameObject::CGameObject(const CGameObject& rhs)
	: CComposite(rhs)
{
	m_pTransform = static_cast<CTransform*>(rhs.m_pTransform->Clone(rhs.m_pTransform));
}

HRESULT CGameObject::Initialize_Prototype()
{
	m_pTransform = CTransform::Create(m_pDevice, m_pContext);

	if (nullptr == m_pTransform)
		return E_FAIL;
	
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	m_OffsetMatrix = XMMatrixIdentity();
	m_pTransform->Set_Owner(this);

	m_Components.emplace(TEXT("Com_Transform"), m_pTransform);

	Safe_AddRef(m_pTransform);

	return S_OK;
}

void CGameObject::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	for (auto iter = m_SoundChannel.begin(); iter < m_SoundChannel.end();)
	{
		if (false == pGameInstance->Is_SoundPlaying(*iter))
			iter = m_SoundChannel.erase(iter);
		else
			iter++;
	}

	Safe_Release(pGameInstance);
}

void CGameObject::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CGameObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	return S_OK;
}

void CGameObject::Stop_Sound()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	for (auto& iChannel : m_SoundChannel)
	{
		pGameInstance->Stop_Sound(iChannel);
	}

	Safe_Release(pGameInstance);
}

void CGameObject::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
}
