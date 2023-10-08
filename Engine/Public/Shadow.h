#pragma once
#include "Engine_Defines.h"
#include "Component.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CBlur;

class CShadow final : public CComponent
{
private:
	explicit CShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CShadow() = default;

public:
	HRESULT Initialize(CVIBuffer_Rect* pRectBuffer);
	virtual HRESULT Render() override;

private:
	CBlur*				m_pBlur = { nullptr };
	CShader*			m_pShader = { nullptr };
	CVIBuffer_Rect*		m_pBuffer = { nullptr };
	_float4x4			m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	_float2				m_vWinSize;

public:
	static CShadow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CVIBuffer_Rect* pRectBuffer);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END