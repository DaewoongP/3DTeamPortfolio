#include "UI_Group_Finisher.h"
#include "GameInstance.h"
#include "UI_Back.h"
#include "UI_Finisher.h"
#include "Health.h"

CUI_Group_Finisher::CUI_Group_Finisher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Group_Finisher::CUI_Group_Finisher(const CUI_Group_Finisher& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Group_Finisher::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CUI_Group_Finisher Add ProtoType");
		return E_FAIL;
	}

	m_pBacks.clear();
	m_pFinishers.clear();

	return S_OK;
}

HRESULT CUI_Group_Finisher::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		FINISHERDESC* pDesc = (FINISHERDESC*)pArg;
		
		Create_Front(pDesc->szFilePath);
		Create_Back(TEXT("../../Resources/GameData/UIData/UI_Group_Finisher_Back.uidata"));

		m_pHealth = pDesc->pHealth;
	}


	return S_OK;
}

void CUI_Group_Finisher::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Group_Finisher::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

_bool CUI_Group_Finisher::Set_Gauge()
{
	if (nullptr == m_pHealth || nullptr == m_pFinishers[BACK] || nullptr == m_pFinishers[FRONT]
		|| m_pBacks.size() < 2 || m_pFinishers.size() < 2)
		return false;

	_float fPercent = m_pHealth->Get_Current_HP_Percent();
	if (fPercent <= 0.5f)
	{
		m_pFinishers[FRONT]->Set_Gauge(0.f);
		m_pFinishers[BACK]->Set_Gauge(m_pHealth->Get_Current_HP() * 2.f);
	}
	else if (fPercent > 0.5f)
	{
		m_pFinishers[FRONT]->Set_Gauge((m_pHealth->Get_Current_HP() - 50.f) * 2.f);
		m_pFinishers[BACK]->Set_Gauge(m_pHealth->Get_MaxHP());
	}

	return true;
}


HRESULT CUI_Group_Finisher::Add_Prototype()
{
	BEGININSTANCE;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		CUI_Back::Create(m_pDevice, m_pContext), true)))
	{
		ENDINSTANCE;
		__debugbreak;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Finisher"),
		CUI_Finisher::Create(m_pDevice, m_pContext), true)))
	{
		ENDINSTANCE;
		__debugbreak;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CUI_Group_Finisher::Add_Components(wstring wszTag)
{
	BEGININSTANCE;
	CUI_Back* pBack = nullptr;
	CUI_Finisher* pFinisher = nullptr;

	wstring wszFrameTag = TEXT("Com_UI_Back_Frame_");
	wszFrameTag += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		wszFrameTag.c_str(), reinterpret_cast<CComponent**>(&pBack))))
	{
		MSG_BOX("Com_UI_Finisher : Failed Clone Component (Com_UI_Finisher_Frame)");
		ENDINSTANCE;
		__debugbreak;
		return E_FAIL;
	}
	m_pBacks.push_back(pBack);

	wstring wszFinisherTag = TEXT("Com_UI_Finisher_");
	wszFinisherTag += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Finisher"),
		wszFinisherTag.c_str(), reinterpret_cast<CComponent**>(&pFinisher))))
	{
		MSG_BOX("Com_UI_Finisher : Failed Clone Component (Com_UI_Finisher)");
		ENDINSTANCE;
		__debugbreak;
		return E_FAIL;
	}
	m_pFinishers.push_back(pFinisher);

	ENDINSTANCE;

	return S_OK;
}


HRESULT CUI_Group_Finisher::Read_File(const _tchar* pFilePath, _uint iIndex)
{
	if (nullptr == m_pBacks[iIndex] || iIndex >= FINISHER_END)
	{
		MSG_BOX("Failed Load");
		return E_FAIL;
	}

	_ulong dwByte = 0;
	DWORD dwStrByte = 0;

	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed Load");
		CloseHandle(hFile);
		return E_FAIL;
	}

	_tchar szGroupName[MAX_PATH] = TEXT("");

	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, szGroupName, dwStrByte, &dwByte, nullptr);

	m_pBacks[iIndex]->Load(Load_File(hFile));

	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	m_pFinishers[iIndex]->Load(Load_File(hFile));
	m_pFinishers[iIndex]->Set_Parent(m_pBacks[iIndex]);

	CloseHandle(hFile);

	return S_OK;
}

CUI::UIDESC CUI_Group_Finisher::Load_File(const HANDLE hFile)
{
	CUI::UIDESC UIDesc;
	ZEROMEM(&UIDesc);

	_ulong dwByte = 0;
	DWORD dwStrByte = 0;
	_tchar szTextureName[MAX_PATH] = TEXT("");
	_tchar szAlphaPrototypeTag[MAX_PATH] = TEXT("");
	_bool isParent, isAlpha, isSave;
	_int eID;

	ReadFile(hFile, &UIDesc.vCombinedXY, sizeof(_float2), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.fX, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.fY, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.fZ, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.fSizeX, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.fSizeY, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, szTextureName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, UIDesc.szTexturePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &isParent, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &isAlpha, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.vColor, sizeof(_float4), &dwByte, nullptr);
	ReadFile(hFile, szAlphaPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, UIDesc.szAlphaTexturePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &eID, sizeof(_int), &dwByte, nullptr);
	ReadFile(hFile, &isSave, sizeof(_bool), &dwByte, nullptr);

	fs::path fsPath = UIDesc.szTexturePath;

	std::wstring wszExtension = fsPath.extension();
	if (wszExtension == TEXT(".png"))
	{
		fsPath.replace_extension(L".dds");
		lstrcpy(UIDesc.szTexturePath, fsPath.c_str());
	}

	return UIDesc;
}

HRESULT CUI_Group_Finisher::Create_Front(void* pArg)
{
	wstring wstrTag = TEXT("Front");
	if (FAILED(Add_Components(wstrTag)))
		return E_FAIL;

	if (FAILED(Read_File(reinterpret_cast<const _tchar*>(pArg), FRONT)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Group_Finisher::Create_Back(const _tchar* pFIlePath)
{
	wstring wstrTag = TEXT("Back");
	if (FAILED(Add_Components(wstrTag)))
		return E_FAIL;

	if (FAILED(Read_File(pFIlePath, BACK)))
		return E_FAIL;

	return S_OK;
}

CUI_Group_Finisher* CUI_Group_Finisher::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_Finisher* pInstance = New CUI_Group_Finisher(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_Finisher");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Group_Finisher::Clone(void* pArg)
{
	CUI_Group_Finisher* pInstance = New CUI_Group_Finisher(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Group_Finisher");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CUI_Group_Finisher::Free()
{
	__super::Free();


	for (auto& iter : m_pBacks)
	{
		Safe_Release(iter);
	}
	m_pBacks.clear();

	for (auto& iter : m_pFinishers)
	{
		Safe_Release(iter);
	}
	m_pFinishers.clear();

	Safe_Release(m_pHealth);

}

