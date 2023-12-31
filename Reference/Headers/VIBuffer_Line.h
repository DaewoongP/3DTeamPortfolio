#pragma once
/* =============================================== */
// 
//	�� : �ڴ��
//	�� :
//
/* =============================================== */

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Line final : public CVIBuffer
{
public:
	typedef struct tagLineDesc
	{
		_uint iNum = { 0 };
		const _float3* pLines = { nullptr };
	}LINEDESC;

private:
	explicit CVIBuffer_Line(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer_Line(const CVIBuffer_Line& rhs);
	virtual ~CVIBuffer_Line() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	void Tick(LINEDESC LineDesc);

public:
	static CVIBuffer_Line* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END