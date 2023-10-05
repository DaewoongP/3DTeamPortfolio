#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CFlowMap final : public CGameObject
{
private:
	explicit CFlowMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFlowMap(const CFlowMap& rhs);
	virtual ~CFlowMap() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*			m_pShader = { nullptr };
	CTexture*			m_pTexture1 = { nullptr };
	CTexture*			m_pTexture2 = { nullptr };
	CRenderer*			m_pRenderer = { nullptr };
	CVIBuffer_Rect*		m_pBuffer = { nullptr };

private:
	_float				m_fFrame = { 0.f };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CFlowMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END