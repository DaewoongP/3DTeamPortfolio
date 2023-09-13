#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CDebug_Render_Ring final : public CVIBuffer
{
public:
	typedef struct tagRingDesc
	{
		_float fRadius = { 0.f };
		_float3 vMajorAxis;
		_float3 vMinorAxis;
		_float3 vOrigin;
	}RINGDESC;

private:
	explicit CDebug_Render_Ring(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDebug_Render_Ring(const CDebug_Render_Ring& rhs);
	virtual ~CDebug_Render_Ring() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CDebug_Render_Ring* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END