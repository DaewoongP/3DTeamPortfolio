#pragma once
#include "Base.h"

BEGIN(Engine)
class CTexture;

class CTexturePool final : public CBase
{
	DECLARE_SINGLETON(CTexturePool)
private:
	explicit CTexturePool() = default;
	virtual ~CTexturePool() = default;

public:
	class CTexture* Reuse_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	HRESULT Clear_Textures();

private:
	unordered_map<const _tchar*, class CTexture*>	m_Textures;

public:
	virtual void Free() override;
};

END