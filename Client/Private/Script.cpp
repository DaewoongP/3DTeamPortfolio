#include "Script.h"
#include "GameInstance.h"
#include "UI_Script.h"

CMyScript::CMyScript(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CMyScript::CMyScript(const CMyScript& rhs)
	: CComposite(rhs)
{
}

HRESULT CMyScript::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CScript Add ProtoType");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CMyScript::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Add_Components();

	return S_OK;
}

HRESULT CMyScript::Add_Prototype()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Script"),
		CUI_Script::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMyScript::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Script"),
		TEXT("Com_UI_Script"), reinterpret_cast<CComponent**>(&m_pUI_Script))))
	{
		MSG_BOX("CMyScript : Failed Clone Component (Com_UI_Script)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMyScript::Add_Script(const _tchar* pTexturePath)
{
	if (m_Scripts.size() == 0)
	{
		m_Scripts.push_back(CTexture::Create(m_pDevice, m_pContext, pTexturePath));
		m_pUI_Script->Set_Texture(m_Scripts[m_iCurrentScriptIndex]);
	}
	else
		m_Scripts.push_back(CTexture::Create(m_pDevice, m_pContext, pTexturePath));


	return S_OK;
}

_bool CMyScript::Next_Script()
{
	if (m_Scripts.size() - 1 == m_iCurrentScriptIndex)
		return false;

	++m_iCurrentScriptIndex;
	m_pUI_Script->Set_Texture(m_Scripts[m_iCurrentScriptIndex]);

	return true;
}

HRESULT CMyScript::Render_Script(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	return S_OK;
}


CMyScript* CMyScript::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMyScript* pInstance = New CMyScript(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Interaction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMyScript::Clone(void* pArg)
{
	CMyScript* pInstance = New CMyScript(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Interaction");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CMyScript::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Script);
	for (auto& pTexture : m_Scripts)
	{
		Safe_Release(pTexture);
	}
}