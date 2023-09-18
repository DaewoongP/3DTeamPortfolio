#pragma once
#include "VIBuffer_Geometry.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_UVSphere final : public CVIBuffer
{
protected:
	explicit CVIBuffer_UVSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_UVSphere(const CVIBuffer_UVSphere& rhs);
	virtual ~CVIBuffer_UVSphere() = default;

protected:
	HRESULT Initialize_Prototype();

public:
	static CVIBuffer_UVSphere* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer* Clone(void* _pArg);
	virtual void Free(void) override;
};

END