#include "UI_Group_MiniMap.h"
#include "GameInstance.h"
#include "UI_Effect_Back.h"
#include "UI_Back.h"

CUI_Group_MiniMap::CUI_Group_MiniMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Group_MiniMap::CUI_Group_MiniMap(const CUI_Group_MiniMap& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Group_MiniMap::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CUI_Group_MiniMap Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Group_MiniMap::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Read_File(reinterpret_cast<const _tchar*>(pArg))))
		return E_FAIL;

	if (FAILED(Add_MapTexture()))
		return E_FAIL;

	return S_OK;
}

void CUI_Group_MiniMap::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_fMiniMapRotation > 360.f)
	{
		m_fMiniMapRotation = 0.f;
	}

	if (m_fPlayerRotation > 360.f)
	{
		m_fPlayerRotation = 0.f;
	}

	m_fMiniMapRotation += fTimeDelta * 10.f;
	m_fPlayerRotation += fTimeDelta * 10.f;

	m_pUI_Effect_Back->Set_Rotation(_float3(0.f, 0.f, 1.f), XMConvertToRadians(m_fMiniMapRotation));
	m_pUI_Back->Set_Rotation(_float3(0.f, 0.f, 1.f), XMConvertToRadians(m_fPlayerRotation));
	Set_MapTexture(0);
}

void CUI_Group_MiniMap::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Group_MiniMap::Set_MapTexture(_uint iIndex)
{
	m_pUI_Effect_Back->Set_Texture(m_MapTextures[iIndex]);

	return S_OK;
}

HRESULT CUI_Group_MiniMap::Add_Prototype()
{
	BEGININSTANCE;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		CUI_Effect_Back::Create(m_pDevice, m_pContext), true)))
	{
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		CUI_Back::Create(m_pDevice, m_pContext), true)))
	{
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CUI_Group_MiniMap::Add_Components()
{
	BEGININSTANCE;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		TEXT("Com_UI_MiniMap_Frame"), reinterpret_cast<CComponent**>(&m_pUI_Effect_Back))))
	{
		MSG_BOX("CUI_Group_MiniMap : Failed Clone Component (Com_UI_MiniMap_Frame)");
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_MiniMap_Player"), reinterpret_cast<CComponent**>(&m_pUI_Back))))
	{
		MSG_BOX("CUI_Group_MiniMap : Failed Clone Component (Com_UI_MiniMap_Player)");
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CUI_Group_MiniMap::Read_File(const _tchar* pFilePath)
{
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

	m_pUI_Effect_Back->Load(Load_File(hFile));

	CUI_Image::IMAGEDESC ImageDesc;
	ImageDesc.vCombinedXY = m_pUI_Effect_Back->Get_CombinedXY();
	ImageDesc.fX = m_pUI_Effect_Back->Get_XY().x;
	ImageDesc.fY = m_pUI_Effect_Back->Get_XY().y;
	ImageDesc.fZ = 0.5f;
	ImageDesc.fSizeX = m_pUI_Effect_Back->Get_SizeXY().x;
	ImageDesc.fSizeY = m_pUI_Effect_Back->Get_SizeXY().y;
	ImageDesc.fSizeY = m_pUI_Effect_Back->Get_SizeXY().y;
	ImageDesc.eShadertype = CUI_Image::SHADERTYPE::MINIMAP;

	m_pUI_Effect_Back->Set_ImageCom(ImageDesc);

	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	m_pUI_Back->Load(Load_File(hFile));
	m_pUI_Back->Set_Parent(m_pUI_Effect_Back);

	CloseHandle(hFile);

	return S_OK;
}

CUI::UIDESC CUI_Group_MiniMap::Load_File(const HANDLE hFile)
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

	return UIDesc;
}

HRESULT CUI_Group_MiniMap::Add_MapTexture()
{
	m_MapTextures.push_back(CTexture::Create(m_pDevice, m_pContext, 
		TEXT("../../Resources/UI/Game/UI/HUD/MiniMap/MiniMapTiles/Tents/UI_T_MapMini_BanditCampA_TentInterior_01_Base_D.png")));

	m_MapTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/UI/HUD/MiniMap/MiniMapTiles/Tents/UI_T_MapMini_BanditCampA_TentInterior_02_Base_D.png")));

	m_MapTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/UI/HUD/MiniMap/MiniMapTiles/Tents/UI_T_MapMini_BanditCampA_TentInterior_03_Base_D.png")));

	m_MapTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/UI/HUD/MiniMap/MiniMapTiles/Tents/UI_T_MapMini_BanditCampA_TentInterior_04_Base_D.png")));

	m_MapTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/UI/HUD/MiniMap/MiniMapTiles/Tents/UI_T_MapMini_BanditCampA_TentInterior_05_Base_D.png")));

	return S_OK;
}

CUI_Group_MiniMap* CUI_Group_MiniMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_MiniMap* pInstance = New CUI_Group_MiniMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_MiniMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Group_MiniMap::Clone(void* pArg)
{
	CUI_Group_MiniMap* pInstance = New CUI_Group_MiniMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Group_MiniMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CUI_Group_MiniMap::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Effect_Back);
	Safe_Release(m_pUI_Back);

	for (auto& pTexture : m_MapTextures)
	{
		Safe_Release(pTexture);
	}
	m_MapTextures.clear();
}

