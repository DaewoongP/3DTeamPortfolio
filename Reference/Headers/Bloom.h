#pragma once
#include "Engine_Defines.h"
#include "Component.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CBloom : public CComponent
{
private:
	explicit CBloom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBloom() = default;

public:
	HRESULT Initialize(CVIBuffer_Rect* pRectBuffer);
	HRESULT Render(const _tchar* pRenderTargetTag);

private:
	CShader*			m_pShader = { nullptr };
	CVIBuffer_Rect*		m_pBuffer = { nullptr };
	_float4x4			m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

public:
	static CBloom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CVIBuffer_Rect* pRectBuffer);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END