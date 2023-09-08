#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Geometry abstract : public CVIBuffer
{
protected:
	explicit CVIBuffer_Geometry(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer_Geometry(const CVIBuffer_Geometry& rhs);
	virtual ~CVIBuffer_Geometry() = default;
	


public:
	virtual void Free(void) override;
};

END