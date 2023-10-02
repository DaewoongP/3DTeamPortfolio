#pragma once
#include "Engine_Defines.h"
#include "Component.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;

class CHDR final : public CComponent
{
private:
	explicit CHDR(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CHDR() = default;

public:
	HRESULT Initialize(CVIBuffer_Rect* pRectBuffer);
	virtual HRESULT Render() override;

private:
	CShader*			m_pShader = { nullptr };
	CVIBuffer_Rect*		m_pBuffer = { nullptr };
	_float4x4			m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

public:
	static CHDR* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CVIBuffer_Rect* pRectBuffer);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END