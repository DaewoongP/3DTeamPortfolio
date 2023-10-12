#pragma once
#include "Engine_Defines.h"
#include "Component.h"

BEGIN(Engine)
class CBlur;
class CShader;
class CVIBuffer_Rect;

class CDOF : public CComponent
{
private:
	explicit CDOF(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDOF() = default;

public:
	HRESULT Initialize(CVIBuffer_Rect* pRectBuffer);
	HRESULT Render(const _tchar* pTargetTag);

private:
	CBlur*				m_pBlur	= { nullptr };
	CShader*			m_pShader = { nullptr };
	CVIBuffer_Rect*		m_pBuffer = { nullptr };
	_float4x4			m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	_float				m_fFocusDis;
	_float				m_fFocusRange;

public:
	static CDOF* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CVIBuffer_Rect* pRectBuffer);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};
END
