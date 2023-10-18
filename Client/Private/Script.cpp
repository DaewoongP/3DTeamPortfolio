#include "Script.h"
#include "GameInstance.h"
#include "UI_Script.h"

CScript::CScript(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CScript::CScript(const CScript& rhs)
	: CComposite(rhs)
{
}

HRESULT CScript::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CScript Add ProtoType");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CScript::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Add_Components();

	return S_OK;
}

void CScript::Tick(_float fTimeDelta)
{
	Next_Script();
}

void CScript::Late_Tick(_float fTimeDelta)
{
	if (m_isRender)
		__super::Late_Tick(fTimeDelta);
}

HRESULT CScript::Render()
{
	//if (nullptr != m_pUI_Script)
	//	m_pUI_Script->Render();

	return S_OK;
}

HRESULT CScript::Add_Prototype()
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

HRESULT CScript::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI::UIDESC desc;
	desc.vCombinedXY = _float2(0.f, 0.f);
	desc.fX = 640.f;
	desc.fY = 530.f;
	desc.fZ = 0.f;
	desc.fSizeX = 820.f;
	desc.fSizeY = 180.f;

	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Script"),
		TEXT("Com_UI_Script"), reinterpret_cast<CComponent**>(&m_pUI_Script), &desc)))
	{
		MSG_BOX("CScript : Failed Clone Component (Com_UI_Script)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CScript::Add_Script(const _tchar* pTexturePath)
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

void CScript::Reset_Script()
{
	m_iCurrentScriptIndex = 0;
	m_pUI_Script->Set_Texture(m_Scripts[m_iCurrentScriptIndex]);
}

_bool CScript::Next_Script()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{

		if (m_Scripts.size() - 1 == m_iCurrentScriptIndex)
		{
			Safe_Release(pGameInstance);
			m_isRender = false;
			return false;
		}

		Safe_Release(pGameInstance);
		++m_iCurrentScriptIndex;
		m_pUI_Script->Set_Texture(m_Scripts[m_iCurrentScriptIndex]);
		m_isRender = true;
		return true;
	}

	Safe_Release(pGameInstance);
	return true;
}

CScript* CScript::Find_Script(const _tchar* swzScriptTag, _umap<const _tchar*, CScript*>& pSript)
{
	auto	iter = find_if(pSript.begin(), pSript.end(), CTag_Finder(swzScriptTag));

	if (iter == pSript.end())
		return nullptr;

	return iter->second;
}


CScript* CScript::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CScript* pInstance = New CScript(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Interaction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CScript::Clone(void* pArg)
{
	CScript* pInstance = New CScript(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Interaction");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CScript::Free()
{
	__super::Free();


	if (m_isCloned)
	{
		Safe_Release(m_pUI_Script);

		for (auto& pTexture : m_Scripts)
		{
			Safe_Release(pTexture);
		}
	}

}