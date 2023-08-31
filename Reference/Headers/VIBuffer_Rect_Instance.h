#pragma once
/* =============================================== */
// 
//	�� : �ڴ��
//	�� : 
//
/* =============================================== */

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagInstanceDesc
	{
		// �������� �ִ� ������
		_float		fHeight;
		// ��ƼŬ ��ü�� �ٿ���ڽ�
		_float3		vExtents;
		// �ּ� ���ǵ� X ~ �ִ� ���ǵ� Y
		_uint2		vSpeed;
		// ���� �ð�
		_float		fLifeTime;
	}INSTANCEDESC;

private:
	explicit CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& rhs);
	virtual ~CVIBuffer_Rect_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCEDESC* pInstanceDesc, _uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

private:
	INSTANCEDESC	m_InstanceDesc;
	_float*			m_pInstanceSpeed = { nullptr };
	_float4*		m_pInstancePos = { nullptr };

private:
	HRESULT Make_InstanceLogic(const INSTANCEDESC* pInstanceDesc);

public:
	static CVIBuffer_Rect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCEDESC* pInstanceDesc, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END