#pragma once
#include "Engine_Defines.h"
#include "Component.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CRenderTarget_Manager;

class CBlur final : public CComponent
{
public:
	enum BLUR { BLUR_X, BLUR_Y, BLUR_XY, BLUR_END };
private:
	explicit CBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBlur() = default;

public:
	HRESULT Initialize();
	HRESULT Render(const _tchar* pTargetTag, BLUR eBlurType = BLUR_XY);

private:
	CShader*				m_pShader = { nullptr };
	CVIBuffer_Rect*			m_pBuffer = { nullptr };
	CRenderTarget_Manager*	m_pRenderTarget_Manager = { nullptr };
	_float4x4				m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	HRESULT Add_Components();

public:
	static CBlur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END