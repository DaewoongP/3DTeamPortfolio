#include "UI_Damage.h"
#include "GameInstance.h"
#include "Enemy.h"

CUI_Damage::CUI_Damage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Damage::CUI_Damage(const CUI_Damage& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Damage::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CUI_Damage Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Damage::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		CUI_DamageFont::DAMAGEFONTDESC* pDesc = (CUI_DamageFont::DAMAGEFONTDESC*)pArg;

		lstrcpy(m_DamageFontDesc.m_pText, TEXT(""));
		m_DamageFontDesc.m_vPos = { 640.f, 360.f };
		m_DamageFontDesc.m_vColor = pDesc->m_vColor;
		m_DamageFontDesc.m_fRotation = pDesc->m_fRotation;
		m_DamageFontDesc.m_vOrigin = pDesc->m_vOrigin;
		m_DamageFontDesc.m_vMinScale = pDesc->m_vMinScale;
		m_DamageFontDesc.m_vMaxScale = pDesc->m_vMaxScale;

		m_DamageFontDesc.m_fLifeTime = pDesc->m_fLifeTime;
		m_DamageFontDesc.m_fMoveSpeed = pDesc->m_fMoveSpeed;
		m_DamageFontDesc.m_fAlphaSpeed = pDesc->m_fAlphaSpeed;
	}

	return S_OK;
}

void CUI_Damage::Tick(_float fTimeDelta)
{
	for (auto& pDmageFont : m_DamageFonts)
	{
		pDmageFont->Tick(fTimeDelta);
	}
	__super::Tick(fTimeDelta);
}

void CUI_Damage::Late_Tick(_float fTimeDelta)
{
	for (auto iter = m_DamageFonts.begin(); iter != m_DamageFonts.end();)
	{
		(*iter)->Late_Tick(fTimeDelta);

		if (!(*iter)->Get_isLife())
		{
			Safe_Release((*iter));
			iter = m_DamageFonts.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Damage::Add_Prototype()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_DamageFont"),
		CUI_DamageFont::Create(m_pDevice, m_pContext, TEXT("../../Resources/Fonts/NexonGothic.spritefont")), true)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}


HRESULT CUI_Damage::Add_Font(_uint iDamage, _float3 vTargetPos)
{
	CEnemy* pEnemy = static_cast<CEnemy*>(m_pOwner);
	if (nullptr == pEnemy)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	wstring wstrDamage = to_wstring(iDamage);

	CUI_DamageFont::DAMAGEFONTDESC DamageFontDesc;

	lstrcpy(DamageFontDesc.m_pText, wstrDamage.c_str());
	DamageFontDesc.m_vWorldPos = vTargetPos;
	DamageFontDesc.m_vColor = m_DamageFontDesc.m_vColor;
	DamageFontDesc.m_fRotation = m_DamageFontDesc.m_fRotation;
	DamageFontDesc.m_vOrigin = m_DamageFontDesc.m_vOrigin;
	DamageFontDesc.m_vMinScale = m_DamageFontDesc.m_vMinScale;
	DamageFontDesc.m_vMaxScale = m_DamageFontDesc.m_vMaxScale;

	DamageFontDesc.m_fLifeTime = m_DamageFontDesc.m_fLifeTime;
	DamageFontDesc.m_fMoveSpeed = m_DamageFontDesc.m_fMoveSpeed;
	DamageFontDesc.m_fAlphaSpeed = m_DamageFontDesc.m_fAlphaSpeed;

	CUI_DamageFont* pDamageFont = static_cast<CUI_DamageFont*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_DamageFont"), &DamageFontDesc));
	if (nullptr != pDamageFont)
		m_DamageFonts.push_back(pDamageFont);

	Safe_Release(pGameInstance);

	return S_OK;
}

CUI_Damage* CUI_Damage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Damage* pInstance = New CUI_Damage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Damage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Damage::Clone(void* pArg)
{
	CUI_Damage* pInstance = New CUI_Damage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Damage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Damage::Free()
{
	__super::Free();

	for (auto& pDamageFonts : m_DamageFonts)
	{
		Safe_Release(pDamageFonts);
	}
	m_DamageFonts.clear();
}