#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Tool)

class CRectDebug final : public CGameObject
{
private:
	explicit CRectDebug(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRectDebug(const CRectDebug& rhs);
	virtual ~CRectDebug() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Scale(_int2 value);

private:
	_bool				m_isRendering = { true };

private:
	CShader* m_pShader = { nullptr };
	CTexture* m_pTexture = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CVIBuffer_Rect* m_pBuffer = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CRectDebug* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END