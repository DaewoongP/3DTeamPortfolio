#pragma once

/* =============================================== */
// 
//	Á¤ : ¹Ú´ë¿õ
//	ºÎ :
//
/* =============================================== */

#include "Component.h"
#include "TexturePool.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	explicit CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	const _float2 Get_TextureSize(_uint iTextureIndex);
	_uint Get_NumTextures() { return m_iNumTextures; }
	const _tchar* Get_TextureFilePath(_uint iTextureIndex = 0) { return m_szTextureFilePathes[iTextureIndex]; }
	_int Get_TextureIndex_By_Path(const _tchar* pPath);

public:
	virtual HRESULT Initialize_Prototype(const _tchar * pTextureFilePath, _uint iNumTextures);

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char * pContantName, _uint iTextureIndex = 0);
	HRESULT Bind_ShaderResources(class CShader* pShader, const _char * pContantName);

private:
	_tchar								m_szTextureFilePath[MAX_PATH] = TEXT("");
	_uint								m_iNumTextures = { 0 };
	vector<ID3D11ShaderResourceView*>	m_Textures;
	vector<const _tchar*>				m_szTextureFilePathes;

public:
	static CTexture* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTextureFilePath, _uint iNumTextures = 1);
	static CTexture* Create_Origin(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
