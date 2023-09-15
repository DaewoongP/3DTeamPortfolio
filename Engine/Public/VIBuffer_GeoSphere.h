#pragma once
#include "VIBuffer_Geometry.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_GeoSphere final : public CVIBuffer
{
private:
	typedef VTXPOSNORTEX Vertex;

	struct Triangle
	{
		_uint v1, v2, v3;
	};

protected:
	explicit CVIBuffer_GeoSphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer_GeoSphere(const CVIBuffer_GeoSphere& rhs);
	virtual ~CVIBuffer_GeoSphere() = default;

protected:
	HRESULT Initialize_Prototype();
	//void ComputeNormalTex();

	_uint getMidpointIndex(vector<Vertex>& vertices, _uint index1, _uint index2);
	void Subdivide(vector<Vertex>& vertices, vector<Triangle>& triangles);
	
private:
	_uint m_iSubdivisionNum = { 3 };
	map<pair<_uint, _uint>, _uint> m_MidPointCache;

public:
	static CVIBuffer_GeoSphere* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer* Clone(void* _pArg);
	virtual void Free(void) override;
};

END