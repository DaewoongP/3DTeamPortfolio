#pragma once

/* =============================================== */
// 
//	�� : �ڴ��
//	�� :
//
/* =============================================== */

#include "Composite.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CComposite
{
protected:
	explicit CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	CTransform* Get_Transform() const { return m_pTransform; }
	void* Get_CollisionData() { return m_pCollisionArg; }
	_bool isDead() const { return m_isDead; }
	void Set_Dead() { m_isDead = true; }
	void Set_CollisionData(void* pArg) { m_pCollisionArg = pArg; }
	
	//���� ������ ���̱� ������ Offset�� �ξ� �������� �ø��� �մϴ�.
	void Set_Offset_Matrix(_float4x4 matrix) { m_OffsetMatrix = matrix; }
	_float4x4 Get_Offset_Matrix() const { return m_OffsetMatrix; }
	const _float4x4* Get_Offset_MatrixPtr() const { return &m_OffsetMatrix; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionDesc) {}
	virtual void OnCollisionStay(COLLEVENTDESC CollisionDesc) {}
	virtual void OnCollisionExit(COLLEVENTDESC CollisionDesc) {}

	//������ ���� ������ �������� �Լ�. �� ������Ʈ���� �ļ� �����������.
	virtual void On_Maigc_Throw_Data(void* data) const {}
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth();

protected:
	CTransform*		m_pTransform = { nullptr };

protected:
	_float4x4	m_OffsetMatrix = {};
	_bool m_isDead = { false };

private:
	void*		m_pCollisionArg = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END