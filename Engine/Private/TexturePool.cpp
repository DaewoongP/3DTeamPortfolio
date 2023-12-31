#include "..\Public\TexturePool.h"
#include "Texture.h"
#include "String_Manager.h"

IMPLEMENT_SINGLETON(CTexturePool)

CTexture* CTexturePool::Reuse_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	auto TexturePair = find_if(m_Textures.begin(), m_Textures.end(), CTag_Finder(pTextureFilePath));

	if (m_Textures.end() == TexturePair)
	{
		// 텍스처 새로 생성
		CTexture* pTexture = CTexture::Create_Origin(pDevice, pContext, pTextureFilePath, iNumTextures);

		m_Textures.emplace(CString_Manager::GetInstance()->Make_WChar(pTextureFilePath), pTexture);

		Safe_AddRef(pTexture);
		return pTexture;
	}

	Safe_AddRef((*TexturePair).second);

	return (*TexturePair).second;
}

HRESULT CTexturePool::Clear_Textures()
{
	for (auto& TexturePair : m_Textures)
	{
		Safe_Release(TexturePair.second);
	}

	m_Textures.clear();

	return S_OK;
}

void CTexturePool::Free()
{
	Clear_Textures();
}