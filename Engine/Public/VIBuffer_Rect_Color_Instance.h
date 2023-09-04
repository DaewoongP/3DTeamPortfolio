#pragma once
/* =============================================== */
// 
//	�� : ������
//	�� : �ڴ��
//
/* =============================================== */

#include "VIBuffer_Color_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Color_Instance final : public CVIBuffer_Color_Instance
{
private:
	explicit CVIBuffer_Rect_Color_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer_Rect_Color_Instance(const CVIBuffer_Rect_Color_Instance& rhs);
	virtual ~CVIBuffer_Rect_Color_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	// ī�޶��� ��ġ�� ��ü�� �������������� �Űܼ� �ν��Ͻ� ������İ� ��ü ���� ������ ī�޶� ��ǥ�� �������Ѵ�.
	virtual void Tick(COLORINSTANCE * pInstances, _int iRenderedParticleNum = -1, _bool isAlphaBlend = false, _float4x4 AlphaBlendObjectWorldMatrixInverse = XMMatrixIdentity());

public:
	void Sort_AlphaBlend(COLORINSTANCE * pInstances, _float4x4 AlphaBlendObjectWorldMatrixInverse);
	HRESULT Make_Buffers();

public:
	static CVIBuffer_Rect_Color_Instance* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END