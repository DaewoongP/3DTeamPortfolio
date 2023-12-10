#pragma once
#include "Composite.h"

BEGIN(Engine)
class CDebug_Render_Sphere;
class CVIBuffer_Line;

class ENGINE_DLL CDebug_Render_Capsule final : public CComposite
{
public:
	typedef struct tagCapsuleDesc
	{
		_float fRadius = { 0.f };
		// ≥Ù¿Ã extents
		_float fHalfHeight = { 0.f };
		_float3 vOffsetPosition;
		_float4 vOffsetRotation;
	}CAPSULEDESC;

private:
	explicit CDebug_Render_Capsule(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDebug_Render_Capsule(const CDebug_Render_Capsule& rhs);
	virtual ~CDebug_Render_Capsule() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

private:
	CDebug_Render_Sphere*			m_pHighSphere = { nullptr };
	CDebug_Render_Sphere*			m_pLowSphere = { nullptr };
	CVIBuffer_Line*					m_pLine = { nullptr };

public:
	static CDebug_Render_Capsule* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END