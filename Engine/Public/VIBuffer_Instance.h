#pragma once

/* =============================================== */
// 
//	정 : 전대인
//	부 :
//
/* =============================================== */

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
protected:
	explicit CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(VTXINSTANCE* pInstances, _int iRenderedParticleNum = -1, _bool isAlphaBlend = false, _float4x4 AlphaBlendObjectWorldMatrixInverse = XMMatrixIdentity());

public:
	virtual void Sort_AlphaBlend(VTXINSTANCE* pInstances, _int iRenderedParticleNum, _float4x4 AlphaBlendObjectWorldMatrixInverse);

public:
	void Set_DrawNum(_uint iDrawNum);
	_uint Get_DrawNum();

public:
	virtual HRESULT Render();
	
protected:
	ID3D11Buffer*	m_pVBInstance = { nullptr };
	_uint			m_iInstanceStride = { 0 };
	_uint			m_iIndexCountPerInstance = { 0 };
	_uint			m_iDrawNum = { 0 };
	_uint			m_iNumInstance = { 0 };

public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END