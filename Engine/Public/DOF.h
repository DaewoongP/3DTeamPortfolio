#pragma once
#include"Engine_Defines.h"
#include "Component.h"
BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;

class CDOF :
	public CComponent
{
private:
	explicit CDOF(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDOF() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTargetTag, _float LightPower);
	virtual HRESULT Render() override;

private:
	CShader*			m_pShader = { nullptr };
	CVIBuffer_Rect*		m_pBuffer = { nullptr };
	_float4x4			m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	_tchar				m_szTargetTag[MAX_PATH] = TEXT("");
	_float				m_fGlowPower = 0.f;
private:
	HRESULT Add_Components();




private:
	_float m_fFocusDis;
	_float m_fFocusRange;
	_float m_fMaxBlur;
public:
	static CDOF* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _float LightPower = 0.f);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};
END
