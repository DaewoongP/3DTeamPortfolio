#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CDebug_Render_TriangleMesh final : public CVIBuffer
{
public:
	typedef struct tagTriangleMeshDesc
	{
		_uint iNumTriangles = { 0 };
		const _float3* pTriangles = { nullptr };
		//const _uint* pIndices = { nullptr };
		_float3 vOffsetPosition;
		_float4 vOffsetRotation;
	}TRIANGLEMESHDESC;

private:
	explicit CDebug_Render_TriangleMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDebug_Render_TriangleMesh(const CDebug_Render_TriangleMesh& rhs);
	virtual ~CDebug_Render_TriangleMesh() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CDebug_Render_TriangleMesh* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END