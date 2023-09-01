#pragma once
/* =============================================== */
// 
//	�� : ������
//	�� : �ڴ��
//
/* =============================================== */

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instance
{
private:
	explicit CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& rhs);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	// ī�޶��� ��ġ�� ��ü�� �������������� �Űܼ� �ν��Ͻ� ������İ� ��ü ���� ������ ī�޶� ��ǥ�� �������Ѵ�.
	virtual void Tick(_float4x4* pInstanceMatrix, _bool isAlphaBlend = false, _float4x4 AlphaBlendObjectWorldMatrixInverse = XMMatrixIdentity());

public:
	HRESULT Make_Buffers();
	void Sort_AlphaBlend(_float4x4 * pInstanceMatrix, _float4x4 AlphaBlendObjectWorldMatrixInverse);

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END