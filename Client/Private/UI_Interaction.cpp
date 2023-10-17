#include "UI_Interaction.h"
#include "GameInstance.h"
#include "UI_Dynamic_Back.h"
#include "UI_Font.h"

CUI_Interaction::CUI_Interaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Interaction::CUI_Interaction(const CUI_Interaction& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Interaction::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CUI_Interaction Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Interaction::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		INTERACTIONDESC* pDesc = (INTERACTIONDESC*)pArg;
		lstrcpy(m_wszName, pDesc->m_wszName);
		lstrcpy(m_wszFunc, pDesc->m_wszFunc);
		m_WorldMatrix = pDesc->m_WorldMatrix;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_Interaction::Tick(_float fTimeDelta)
{
	_float3 vOwnerPos = m_WorldMatrix->Translation();

	_float2 vCaculatePos = Caculate_Projection(vOwnerPos);

	m_pUI_Dynamic_Back->Set_XY(vCaculatePos);
	m_pFont[NAME]->Set_vPos(_float2(vCaculatePos.x - 35.f, vCaculatePos.y - 20.f));
	m_pFont[FUNC]->Set_vPos(_float2(vCaculatePos.x - 35.f, vCaculatePos.y + 10.f));

	__super::Tick(fTimeDelta);
}

void CUI_Interaction::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Interaction::Add_Prototype()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		CUI_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"),
		CUI_Font::Create(m_pDevice, m_pContext, TEXT("../../Resources/Fonts/NexonGothic.spritefont")), true)))
	{
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CUI_Interaction::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_Interaction_Default"), reinterpret_cast<CComponent**>(&m_pUI_Dynamic_Back))))
	{
		MSG_BOX("CUI_Interaction : Failed Clone Component (Com_UI_Interaction_Default)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	CUI::UIDESC UIDesc;
	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 640.f };
	UIDesc.fY = { 360.f };
	UIDesc.fZ = { 0.1f };
	UIDesc.fSizeX = { 128.f };
	UIDesc.fSizeY = { 64.f };
	_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/UI_Edit/Interaction_Default.png");
	lstrcpy(UIDesc.szTexturePath, szTexturePath);

	m_pUI_Dynamic_Back->Load(UIDesc);

	CUI_Font::FONTDESC Desc;
	Desc.m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	Desc.m_fRotation = { 0.f };
	Desc.m_vOrigin = { 0.f, 0.f };
	Desc.m_vScale = { 0.4f, 0.4f };
	Desc.m_vPos = { 640.f, 360.f };
	lstrcpy(Desc.m_pText, m_wszName);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"),
		TEXT("Com_UI_Font_Name"), reinterpret_cast<CComponent**>(&m_pFont[NAME]), &Desc)))
	{
		MSG_BOX("CUI_Interaction : Failed Clone Component (Com_UI_Font_Name)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Desc.m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	Desc.m_fRotation = { 0.f };
	Desc.m_vOrigin = { 0.f, 0.f };
	Desc.m_vScale = { 0.4f, 0.4f };
	Desc.m_vPos = { 640.f, 360.f };
	lstrcpy(Desc.m_pText, m_wszFunc);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"),
		TEXT("Com_UI_Font_Func"), reinterpret_cast<CComponent**>(&m_pFont[FUNC]), &Desc)))
	{
		MSG_BOX("CUI_Interaction : Failed Clone Component (Com_UI_Font_Func)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}


CUI::UIDESC CUI_Interaction::Load_File(const HANDLE hFile)
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

_float2 CUI_Interaction::Caculate_Projection(_float3 vPos)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float3 vOwnerPos = vPos;

	vOwnerPos.y = vOwnerPos.y + 0.5f;
	const _float4x4* viewMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	const _float4x4* projMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

	_float4x4 worldToProjMatrix = XMMatrixIdentity();
	worldToProjMatrix = (*viewMatrix) * (*projMatrix);

	_float3 ScreenPosition = XMVector3TransformCoord(vOwnerPos, worldToProjMatrix);

	if (ScreenPosition.x > 1.f ||
		ScreenPosition.x < -1.f ||
		ScreenPosition.y > 1.f ||
		ScreenPosition.y < -1.f ||
		ScreenPosition.z > 1.f ||
		ScreenPosition.z < 0.f)
	{
		Safe_Release(pGameInstance);
		return _float2(999.f, 999.f);
	}

	_float2 vViewPortSize = pGameInstance->Get_ViewPortSize(m_pContext);
	Safe_Release(pGameInstance);

	return _float2((ScreenPosition.x + 1.f) * (vViewPortSize.x / 2.f),
		(-ScreenPosition.y + 1.f) * (vViewPortSize.y / 2.f));
}

CUI_Interaction* CUI_Interaction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Interaction* pInstance = New CUI_Interaction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Interaction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Interaction::Clone(void* pArg)
{
	CUI_Interaction* pInstance = New CUI_Interaction(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Interaction");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CUI_Interaction::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Dynamic_Back);

	if (m_isCloned)
	{
		for (size_t i = 0; i < INTERACTIONFONT_END; i++)
		{
			Safe_Release(m_pFont[i]);
		}
	}
}