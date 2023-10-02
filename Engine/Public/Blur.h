#pragma once
#include "Engine_Defines.h"
#include "Component.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;

class CBlur final : public CComponent
{
public:
	enum BLUR { BLUR_X, BLUR_Y, BLUR_END };
private:
	explicit CBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBlur() = default;

public:
	HRESULT Initialize(CVIBuffer_Rect* pRectBuffer);
	HRESULT Render(const _tchar* pMRTTag, const _tchar* pTargetTag, BLUR eBlurType);

private:
	CShader*				m_pShader = { nullptr };
	CVIBuffer_Rect*			m_pBuffer = { nullptr };
	_float2					m_vWinSize;
	_float4x4				m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

public:
	static CBlur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CVIBuffer_Rect* pRectBuffer);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END