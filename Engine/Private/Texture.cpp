#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)
	, m_Textures(rhs.m_Textures)
	, m_iNumTextures(rhs.m_iNumTextures)
	, m_szTextureFilePathes(rhs.m_szTextureFilePathes)
{
	for (auto& pTexture : m_Textures)
	{
		Safe_AddRef(pTexture);
	}
}

const _float2 CTexture::Get_TextureSize(_uint iTextureIndex)
{
	_float2 vSize = _float2(0.f, 0.f);

	// Index except
	if (iTextureIndex >= m_Textures.size())
		return vSize;

	ID3D11Resource* pResource = { nullptr };

	m_Textures[iTextureIndex]->GetResource(&pResource);

	// Resource except
	if (nullptr == pResource)
		return vSize;

	ID3D11Texture2D* pTexture2D = { nullptr };

	// HRESULT except
	if (FAILED(pResource->QueryInterface(&pTexture2D)))
		return vSize;

	// NULL except
	if (nullptr == pTexture2D)
		return vSize;

	D3D11_TEXTURE2D_DESC TexDesc;
	pTexture2D->GetDesc(&TexDesc);

	vSize.x = static_cast<_float>(TexDesc.Width);
	vSize.y = static_cast<_float>(TexDesc.Height);

	// Get할때 내부적으로 레퍼런스 카운트 증가하여 삭제.
	Safe_Release(pTexture2D);
	Safe_Release(pResource);

	return vSize;
}

_int CTexture::Get_TextureIndex_By_Path(const _tchar* pPath)
{
	_int iIndex = -1;
	for (auto& TexturePath : m_szTextureFilePathes)
	{
		if (!lstrcmp(TexturePath, pPath))
			return iIndex;
		++iIndex;
	}

	return -1;
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	_tchar			szTextureFilePath[MAX_PATH] = TEXT("");
	
	m_iNumTextures = iNumTextures;
	
	m_Textures.reserve(m_iNumTextures);
	m_szTextureFilePathes.reserve(m_iNumTextures);

	lstrcpy(m_szTextureFilePath, pTextureFilePath);

	// 텍스처들을 순회하면서 SRV를 생성하여 처리
	for (_uint i = 0; i < m_iNumTextures; ++i)
	{
		ID3D11ShaderResourceView* pSRV = { nullptr };

		wsprintf(szTextureFilePath, pTextureFilePath, i);

		m_szTextureFilePathes.push_back(pTextureFilePath);
		
		_tchar			szExt[MAX_PATH] = TEXT("");
		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, 256);

		HRESULT			hr = { 0 };

		if (!lstrcmp(szExt, TEXT(".dds")))
		{
			hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		}
		else if (!lstrcmp(szExt, TEXT(".tga")))
		{
			MSG_BOX("Ext .tga not supported");
			return E_FAIL;
		}
		else
		{
			hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		}
		
		if (FAILED(hr))
		{
			MSG_BOX("Failed Create Texture");
			return E_FAIL;
		}

		m_Textures.emplace_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pContantName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_iNumTextures)
		return E_FAIL;
	
	return pShader->Bind_ShaderResource(pContantName, m_Textures[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResources(CShader* pShader, const _char* pContantName)
{
	return pShader->Bind_ShaderResources(pContantName, m_Textures.data(), m_iNumTextures);
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = { nullptr };

	CTexturePool* pTexturePool = CTexturePool::GetInstance();
	Safe_AddRef(pTexturePool);

	pInstance = pTexturePool->Reuse_Texture(pDevice, pContext, pTextureFilePath, iNumTextures);

	Safe_Release(pTexturePool);
	
	if (nullptr == pInstance)
	{
		MSG_BOX("Failed to Reuse CTexture");
		return nullptr;
	}

	return pInstance;
}

CTexture* CTexture::Create_Origin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	std::lock_guard<std::mutex> lock(mtx);

	CTexture* pInstance = New CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = New CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pTexture : m_Textures)
		Safe_Release(pTexture);
	m_Textures.clear();
}
