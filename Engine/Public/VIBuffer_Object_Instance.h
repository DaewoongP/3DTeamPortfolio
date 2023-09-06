//#pragma once
//
//// NonAnim ¸ðµ¨ ±âÁØ
//
//#include "Mesh.h"
//
//BEGIN(Engine)
//
//
//class ENGINE_DLL CVIBuffer_Object_Instance final : public CMesh
//{
//protected:
//	CVIBuffer_Object_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	CVIBuffer_Object_Instance(const CVIBuffer_Object_Instance& rhs);
//	virtual ~CVIBuffer_Object_Instance() = default;
//
//public:
//	HRESULT Initialize_Prototype(const Engine::MESH Mesh, _float4x4 PivotMatrix, _float4x4* pInstanceMatrix, _uint iNumInstance);
//	virtual HRESULT Initialize(void* pArg) override;
//	virtual HRESULT Render() override;
//
//private:
//	ID3D11Buffer*				m_pVBInstance = { nullptr };
//	_uint						m_iInstanceStride = { 0 };
//	_uint						m_iIndexCountPerInstance = { 0 };
//
//	_float4x4*					m_pInstanceMatrix = { nullptr };
//	_uint						m_iNumInstance = { 0 };
//
//private:
//	virtual HRESULT Ready_VertexBuffer_NonAnim(const Engine::MESH Mesh, _float4x4 PivotMatrix) override;
//
//public:
//	static CVIBuffer_Object_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
//		const Engine::MESH Mesh, _float4x4 PivotMatrix,
//		_float4x4* pInstanceMatrix, _uint iNumInstance);
//	virtual CComponent* Clone(void* pArg);
//	virtual void Free() override;
//};
//
//END