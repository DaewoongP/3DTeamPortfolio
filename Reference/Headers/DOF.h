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
	_float Get_FocusDistance() { return m_fFocusDistance; }
	_float Get_FocusRange() { return m_fFocusRange; }
	void Set_FocusDistance(_float fDistance) { m_fFocusDistance = fDistance; }
	void Set_FocusRange(_float fRange) { m_fFocusRange = fRange; }
	void Default();
	void Off() {
		m_isDof = false;
	}

public:
	HRESULT Initialize(CVIBuffer_Rect* pRectBuffer);
	HRESULT Render(const _tchar* pTargetTag);

private:
	CBlur*				m_pBlur	= { nullptr };
	CShader*			m_pShader = { nullptr };
	CVIBuffer_Rect*		m_pBuffer = { nullptr };
	_float4x4			m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	_bool				m_isDof = { false };
	_float				m_fFocusDistance = { 0.f };
	_float				m_fFocusRange = { 0.f };

public:
	static CDOF* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CVIBuffer_Rect* pRectBuffer);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};
END
