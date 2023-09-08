#pragma once
#include "VIBuffer_Geometry.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_GeoSphere final : public CVIBuffer
{
protected:
	explicit CVIBuffer_GeoSphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer_GeoSphere(const CVIBuffer_GeoSphere& rhs);
	virtual ~CVIBuffer_GeoSphere() = default;

protected:
	HRESULT Initialize_Prototype();
	//void ComputeNormalTex();
public:
	static CVIBuffer_GeoSphere* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer* Clone(void* _pArg);
	virtual void Free(void) override;
};

END