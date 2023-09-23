#include "Dummy_UI.h"
#include "GameInstance.h"
#include "UI_Window.h"

CDummy_UI::CDummy_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

CDummy_UI::CDummy_UI(const CDummy_UI& rhs)
	: CGameObject(rhs)
{
	//lstrcpy(m_wszTextureName, rhs.m_wszTextureName);
}

HRESULT CDummy_UI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummy_UI::Initialize(void* pArg)
{
	DWORD dwByte = 0;
	Load((HANDLE*)(pArg), dwByte);
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg && m_isSave == false)
	{
		CUI_Window::UI_DATA* UIDesc = (CUI_Window::UI_DATA*)pArg;

		_float2 fSize = _float2(UIDesc->m_fSizeX, UIDesc->m_fSizeY);
		Set_Size(fSize.x, fSize.y);

		lstrcpy(m_wszTextureName, UIDesc->m_wszTextureName);
		lstrcpy(m_wszTexturePath, UIDesc->m_wszTexturePath);
	}

	
	if (FAILED(Ready_Texture()))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	if (m_isAlpha)
	{
		Add_AlphaTexture();
	}

	m_pTransform->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransform->Set_Position(_float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_fZ));
	
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CDummy_UI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pParent == nullptr)
	{
		m_vCombinedXY.x = m_fX;
		m_vCombinedXY.y = m_fY;
	}
	else
	{
		_float2 vParentPos = m_pParent->Get_fXY();

		m_vCombinedXY.x = vParentPos.x + m_fX;
		m_vCombinedXY.y = vParentPos.y + m_fY;
	}
}

void CDummy_UI::Late_Tick(_float fTimeDelta)
{
	Change_Scale(m_fSizeX, m_fSizeY);
	Change_Position(m_fX, m_fY);
	
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
#ifdef _DEBUG
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UITEXTURE, this);
#endif // _DEBUG
	}
}

HRESULT CDummy_UI::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_isAlpha && nullptr != m_pAlphaTextureCom)
	{
		m_pShaderCom->Begin("UIAlpha");

	}
	else
	{
		m_pShaderCom->Begin("UI");
	}


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CDummy_UI::Tick_Imgui()
{

}

HRESULT CDummy_UI::Set_AlphaTexture(_tchar* pFilePath)
{
	_char szChar[MAX_PATH] = "";
	WCharToChar(pFilePath, szChar);

	_char szFileName[MAX_PATH] = "";
	_splitpath_s(szChar, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	string strFileName = szFileName;
	string strCom = "Prototype_Component_" + strFileName + "_Texture";

	_tchar wszComName[MAX_PATH] = TEXT("");
	CharToWChar(strCom.c_str(), wszComName);

	BEGININSTANCE

	// 텍스쳐 추가.
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, wszComName,
		CTexture::Create(m_pDevice, m_pContext, pFilePath))))
	{
		MSG_BOX("Failed Create Texture Component");
	}
	ENDINSTANCE

	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, wszComName,
		TEXT("Com_AlphaTexture"), reinterpret_cast<CComponent**>(&m_pAlphaTextureCom))))
	{
		MSG_BOX("Failed CDummy_UI Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	Set_AlphaTextureTag(wszComName);
	Set_AlphaTexturePath(pFilePath);

	return S_OK;
}

_bool CDummy_UI::Is_In_Rect(HWND hWnd)
{
	_bool		isIn = false;

	POINT		ptMouse;
	GetCursorPos(&ptMouse);

	ScreenToClient(hWnd, &ptMouse);

	RECT		rcUI;

	SetRect(&rcUI, _int(m_vCombinedXY.x - m_fSizeX * 0.5f), _int(m_vCombinedXY.y - m_fSizeY * 0.5f),
		_int(m_vCombinedXY.x + m_fSizeX * 0.5f), _int(m_vCombinedXY.y + m_fSizeY * 0.5f));

	isIn = PtInRect(&rcUI, ptMouse);

	return isIn;
}

_float2 CDummy_UI::UIPos_To_WorldPos(_float fX, _float fY)
{
	_float2 fXY = _float2(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f);

	return fXY;
}

_float2 CDummy_UI::WorldPos_To_UIPos(_float fX, _float fY)
{
	_float2 fXY = _float2(fX + g_iWinSizeX * 0.5f, -(fY - g_iWinSizeY * 0.5f));

	return fXY;
}

HRESULT CDummy_UI::Change_Position(_float fX, _float fY)
{
	m_pTransform->Set_Position(
		XMVectorSet(m_vCombinedXY.x - g_iWinSizeX * 0.5f, -m_vCombinedXY.y + g_iWinSizeY * 0.5f, m_fZ, 1.f));

	return S_OK;
}

HRESULT CDummy_UI::Change_Scale(_float fX, _float fY)
{
	m_pTransform->Set_Scale(_float3(fX, fY, 1.f));

	return S_OK;
}

HRESULT CDummy_UI::Save(HANDLE hFile, _ulong& dwByte)
{
	wstring strPath = L"3DTeamPortfolio";
	wstring strFullPath = m_wszTexturePath;

	if (strFullPath.find(strPath) != std::wstring::npos)
	{
		std::wstring result = TEXT("..\\..") + strFullPath.substr(strFullPath.find(strPath) + strPath.length());
		lstrcpy(m_wszTexturePath, result.c_str());
	}

	wstring AstrPath = L"3DTeamPortfolio";
	wstring AstrFullPath = m_wszAlphaTextureFilePath;

	if (AstrFullPath.find(AstrPath) != std::wstring::npos)
	{
		std::wstring result = TEXT("..\\..") + AstrFullPath.substr(AstrFullPath.find(AstrPath) + AstrPath.length());
		lstrcpy(m_wszAlphaTextureFilePath, result.c_str());
	}

	WriteFile(hFile, &m_vCombinedXY, sizeof m_vCombinedXY, &dwByte, nullptr);
	WriteFile(hFile, &m_fX, sizeof m_fX, &dwByte, nullptr);
	WriteFile(hFile, &m_fY, sizeof m_fY, &dwByte, nullptr);
	WriteFile(hFile, &m_fZ, sizeof m_fZ, &dwByte, nullptr);
	WriteFile(hFile, &m_fSizeX, sizeof m_fSizeX, &dwByte, nullptr);
	WriteFile(hFile, &m_fSizeY, sizeof m_fSizeY, &dwByte, nullptr);
	WriteFile(hFile, m_wszTextureName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, m_wszTexturePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &m_isParent, sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &m_isAlpha, sizeof m_isAlpha, &dwByte, nullptr);
	WriteFile(hFile, &m_vColor, sizeof m_vColor, &dwByte, nullptr);
	WriteFile(hFile, m_wszAlphaTexturePrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, m_wszAlphaTextureFilePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &m_eUIType, sizeof(UI_ID), &dwByte, nullptr);

	m_isSave = true;
	WriteFile(hFile, &m_isSave, sizeof(_bool), &dwByte, nullptr);

	return S_OK;
}

HRESULT CDummy_UI::Load(HANDLE hFile, _ulong& dwByte)
{
	ReadFile(hFile, &m_vCombinedXY, sizeof(_float2), &dwByte, nullptr);
	ReadFile(hFile, &m_fX, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_fY, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_fZ, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_fSizeX, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_fSizeY, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, m_wszTextureName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, m_wszTexturePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &m_isParent, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &m_isAlpha, sizeof m_isAlpha, &dwByte, nullptr);
	ReadFile(hFile, &m_vColor, sizeof m_vColor, &dwByte, nullptr);
	ReadFile(hFile, m_wszAlphaTexturePrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, m_wszAlphaTextureFilePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &m_eUIType, sizeof(UI_ID), &dwByte, nullptr);
	ReadFile(hFile, &m_isSave, sizeof(_bool), &dwByte, nullptr);
	return S_OK;
}


HRESULT CDummy_UI::Add_Components()
{
	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CDummy_UI Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* Com_Texture */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, m_wszTextureName,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed CDummy_UI Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	if (m_isAlpha)
	{
		if (FAILED(CComposite::Add_Component(LEVEL_TOOL, m_wszAlphaTexturePrototypeTag,
			TEXT("Com_AlphaTexture"), reinterpret_cast<CComponent**>(&m_pAlphaTextureCom))))
		{
			MSG_BOX("Failed CDummy_UI Add_Component : (Com_Texture)");
			return E_FAIL;
		}
	}

	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CDummy_UI Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* Com_Buffer */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		MSG_BOX("Failed CDummy_UI Add_Component : (Com_Buffer)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDummy_UI::Add_AlphaTexture()
{
	BEGININSTANCE

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, m_wszAlphaTexturePrototypeTag,
			CTexture::Create(m_pDevice, m_pContext, m_wszAlphaTextureFilePath))))
		{
			MSG_BOX("Failed Create Texture Component");
		}

	ENDINSTANCE

		if (FAILED(CComposite::Add_Component(LEVEL_TOOL, m_wszAlphaTexturePrototypeTag,
			TEXT("Com_AlphaTexture"), reinterpret_cast<CComponent**>(&m_pAlphaTextureCom))))
		{
			MSG_BOX("Failed CDummy_UI Add_Component : (Com_Texture)");
			return E_FAIL;
		}

	return S_OK;
}

HRESULT CDummy_UI::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (m_isAlpha && nullptr !=m_pAlphaTextureCom)
	{
		if (FAILED(m_pAlphaTextureCom->Bind_ShaderResources(m_pShaderCom, "g_AlphaTexture")))
			return E_FAIL;

		//if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		//	return E_FAIL;

	}

	return S_OK;
}

HRESULT CDummy_UI::Ready_Texture()
{
	BEGININSTANCE

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, m_wszTextureName,
		CTexture::Create(m_pDevice, m_pContext, m_wszTexturePath))))
	{
		MSG_BOX("Failed Create Texture Component");
	}


	if (m_isAlpha)
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, m_wszAlphaTexturePrototypeTag,
			CTexture::Create(m_pDevice, m_pContext, m_wszAlphaTextureFilePath))))
		{
			MSG_BOX("Failed Create Texture Component");
		}
	}

	ENDINSTANCE

	return S_OK;
}


CDummy_UI* CDummy_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_UI* pInstance = New CDummy_UI(pDevice, pContext);


	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_UI::Clone(void* pArg)
{
	CDummy_UI* pInstance = New CDummy_UI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy_UI::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pAlphaTextureCom);

	Safe_Release(m_pParent);
}