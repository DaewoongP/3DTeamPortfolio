#pragma once
#include "Engine_Defines.h"
#include "Component.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;

class CBlur final : public CComponent
{
public:
	enum BLUR { BLUR_X, BLUR_Y, BLUR_XY, BLUR_END };
private:
	explicit CBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBlur() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTargetTag, BLUR eBlurOption);
	virtual HRESULT Render() override;

private:
	CShader*			m_pShader = { nullptr };
	CVIBuffer_Rect*		m_pBuffer = { nullptr };
	_float4x4			m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	_tchar				m_szTargetTag[MAX_PATH] = TEXT("");
	BLUR				m_eBlurOption = { BLUR_END };

private:
	HRESULT Add_Components();

public:
	static CBlur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		const _tchar* pTargetTag, BLUR eBlurOption);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END