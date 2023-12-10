#include "VIBuffer_Geometry.h"

CVIBuffer_Geometry::CVIBuffer_Geometry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Geometry::CVIBuffer_Geometry(const CVIBuffer_Geometry& rhs)
	: CVIBuffer(rhs)
{
}

void CVIBuffer_Geometry::Free(void)
{
	__super::Free();
}
