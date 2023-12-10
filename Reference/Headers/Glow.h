#pragma once
#include"Engine_Defines.h"
#include "Component.h"
BEGIN(Engine)
class CBlur;
class CShader;
class CGameObject;
class CVIBuffer_Rect;

class CGlow : public CComponent
{
private:
	explicit CGlow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CGlow() = default;

public:
	HRESULT Initialize(CVIBuffer_Rect* pRectBuffer);
	HRESULT Render(list<CGameObject*>& GlowRenderObjects, _float fGlowPower = 3.f);

private:
	CBlur*				m_pBlur	= { nullptr };
	CShader*			m_pShader = { nullptr };
	CVIBuffer_Rect*		m_pBuffer = { nullptr };
	_float4x4			m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

public:
	static CGlow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CVIBuffer_Rect* pRectBuffer);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};
END
