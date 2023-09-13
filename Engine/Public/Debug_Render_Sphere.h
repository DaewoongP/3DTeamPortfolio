#pragma once
#include "Composite.h"

BEGIN(Engine)
class CDebug_Render_Ring;

class ENGINE_DLL CDebug_Render_Sphere final : public CComposite
{
public:
	typedef struct tagSphereDesc
	{
		_float fRadius = { 0.f };
		_float3 vOrigin;
	}SPHEREDESC;
private:
	explicit CDebug_Render_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDebug_Render_Sphere(const CDebug_Render_Sphere& rhs);
	virtual ~CDebug_Render_Sphere() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

private:
	CDebug_Render_Ring*		m_pXZRing = { nullptr };
	CDebug_Render_Ring*		m_pXYRing = { nullptr };
	CDebug_Render_Ring*		m_pYZRing = { nullptr };

public:
	static CDebug_Render_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END