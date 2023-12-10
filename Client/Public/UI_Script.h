#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Script : public CUI
{
protected:
	explicit CUI_Script(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Script(const CUI_Script& rhs);
	virtual ~CUI_Script() = default;

public:
	HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_Texture(CTexture* pTexture) {
		if (nullptr != m_Textures[m_iTextureIndex])
			Safe_Release(m_Textures[m_iTextureIndex]);

		m_Textures[m_iTextureIndex] = pTexture;
		Safe_AddRef(m_Textures[m_iTextureIndex]);
	}

protected:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

protected:
	HRESULT Add_Components();
	virtual HRESULT SetUp_ShaderResources();

public:
	static CUI_Script* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
