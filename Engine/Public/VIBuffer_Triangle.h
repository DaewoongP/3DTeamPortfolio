#pragma once
/* =============================================== */
// 
//	Á¤ : ¹Ú´ë¿õ
//	ºÎ :
//
/* =============================================== */

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Triangle final : public CVIBuffer
{
public:
	typedef struct tagTriangleDesc
	{
		_uint iNum = { 0 };
		const _float3* pTriangles = { nullptr };
	}TRIANGLEDESC;

private:
	explicit CVIBuffer_Triangle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Triangle(const CVIBuffer_Triangle& rhs);
	virtual ~CVIBuffer_Triangle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	void Tick(TRIANGLEDESC TriangleDesc);

public:
	static CVIBuffer_Triangle* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END