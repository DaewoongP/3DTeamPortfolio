#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)
private:
	explicit CFrustum();
	virtual ~CFrustum() = default;

public:
	vector<_float4> Get_CulledPositions() const { return m_CulledPositions; }

public:
	HRESULT Initialize();
	void Tick();

public:
	void Transform_ToLocalSpace(_float4x4 WorldMatrix);
	_bool isIn_WorldFrustum(_float4 vWorldPos, _float fRange);
	_bool isIn_LocalSpace(_float4 vLocalPos, _float fRange);

private:
	_float3				m_vOriginal_Points[8];
	_Plane				m_vWorldPlanes[6];
	_Plane				m_vLocalPlanes[6];

private:
	vector<_float4>		m_CulledPositions;

public:
	virtual void Free() override;
};

END