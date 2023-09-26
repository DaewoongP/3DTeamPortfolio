#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Logo final : public CUI
{
private:
	explicit CUI_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Logo(const CUI_Logo& rhs);
	virtual ~CUI_Logo() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pDissolveTexture = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

#ifdef _DEBUG
private:
	HRESULT		Debug_UI(_float fTimeDelta);

#endif // _DEBUG

private:
	_float		m_fTimeDelta = { 0.f };
	_float		m_fDissolveAmount = { 0.f };

public:
	static CUI_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
