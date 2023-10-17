#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Engine)

class CFlowmap final : public CComponent
{
private:
	explicit CFlowmap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFlowmap(const CFlowmap& rhs);
	virtual ~CFlowmap() = default;

public:
	HRESULT Initialize(CVIBuffer_Rect* pRectBuffer);
	HRESULT Render();
	HRESULT Render_Flowmap();
	HRESULT Render_Reflection();
	HRESULT Render_Refraction();

private:
	CShader*		m_pShader = { nullptr };
	CTexture*		m_pTexture = { nullptr };
	CVIBuffer_Rect* m_pBuffer = { nullptr };
	_float4x4		m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	_float4x4		m_ReflectionViewMatrix;
	_float			m_fHeight = { 0.f };

private:
	HRESULT Add_Components();
	HRESULT Calculate_ReflectionViewMatrix();

public:
	static CFlowmap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CVIBuffer_Rect* pRectBuffer);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END