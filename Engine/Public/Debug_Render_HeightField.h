#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CDebug_Render_HeightField final : public CVIBuffer
{
public:
	typedef struct tagHeightFieldDesc
	{
		_uint iNumRows = { 0 };
		_uint iNumColumns = { 0 };
		const _float3* pPositions = { nullptr };
		_float3 vOrigin;
	}HEIGHTFIELDDESC;

private:
	explicit CDebug_Render_HeightField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDebug_Render_HeightField(const CDebug_Render_HeightField& rhs);
	virtual ~CDebug_Render_HeightField() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CDebug_Render_HeightField* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END