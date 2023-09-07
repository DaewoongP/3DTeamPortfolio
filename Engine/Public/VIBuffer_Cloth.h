#pragma once
/* =============================================== */
// 
//	Á¤ : ¹Ú´ë¿õ
//	ºÎ :
//
/* =============================================== */

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cloth final : public CVIBuffer
{
private:
	explicit CVIBuffer_Cloth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Cloth(const CVIBuffer_Cloth& rhs);
	virtual ~CVIBuffer_Cloth() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	void Tick(_float fTimeDelta);

private:
	_uint			m_iNumVerticesX = { 0 };
	_uint			m_iNumVerticesZ = { 0 };

private:
	vector<VTXPOSTEX>	m_Vertices;
	vector<_ulong>		m_Indices;
	vector<_float>		m_InvMasses;

private:
	cloth::Fabric*		m_pFabric = { nullptr };
	cloth::Cloth*		m_pCloth = { nullptr };
	cloth::Solver*		m_pSolver = { nullptr };

private:
	HRESULT Initialize_ClothMesh();

public:
	static CVIBuffer_Cloth* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END