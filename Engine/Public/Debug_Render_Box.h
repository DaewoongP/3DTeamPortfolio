#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CDebug_Render_Box final : public CVIBuffer
{
public:
	typedef struct tagBoxDesc
	{
		_float3 vExtents;
		_float3 vOrigin;
	}BOXDESC;

private:
	explicit CDebug_Render_Box(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDebug_Render_Box(const CDebug_Render_Box& rhs);
	virtual ~CDebug_Render_Box() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CDebug_Render_Box* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END