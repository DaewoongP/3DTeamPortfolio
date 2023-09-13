#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CDebug_Render_Plane final : public CVIBuffer
{
public:
	typedef struct tagPlaneDesc
	{
		_uint iSizeX = { 0 };
		_uint iSizeZ = { 0 };
		_float3 vOrigin;
	}PLANEDESC;

private:
	explicit CDebug_Render_Plane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDebug_Render_Plane(const CDebug_Render_Plane& rhs);
	virtual ~CDebug_Render_Plane() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CDebug_Render_Plane* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END