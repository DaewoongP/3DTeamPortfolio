#pragma once
/* =============================================== */
// 
//	정 : 전대인
//	부 : 박대웅
//
/* =============================================== */

#include "VIBuffer_Velocity_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Velocity_Instance final : public CVIBuffer_Velocity_Instance
{
private:
	explicit CVIBuffer_Rect_Velocity_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer_Rect_Velocity_Instance(const CVIBuffer_Rect_Velocity_Instance& rhs);
	virtual ~CVIBuffer_Rect_Velocity_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr) override;

public:
	static CVIBuffer_Rect_Velocity_Instance* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END