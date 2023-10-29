#pragma once

/* =============================================== */
// 
//	정 : 박대웅
//	부 :
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
	_bool isDead() const { return m_isDead; }
	// 주의) Obj_Dead처리가 아닙니다.
	void Set_Dead() { m_isDead = true; }
	
	//모델의 영점이 발이기 때문에 Offset을 두어 중점으로 올리려 합니다.
	void Set_Offset_Matrix(_float4x4 matrix) { m_OffsetMatrix = matrix; }
	_float4x4 Get_Offset_Matrix() const { return m_OffsetMatrix; }
	const _float4x4* Get_Offset_MatrixPtr() const { return &m_OffsetMatrix; }
	const _float4x4* Get_HitMatrixPtr() const { return m_pHitMatrix; }
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionDesc) {}
	virtual void OnCollisionStay(COLLEVENTDESC CollisionDesc) {}
	virtual void OnCollisionExit(COLLEVENTDESC CollisionDesc) {}

	//마법이 값을 떤지고 도망가는 함수. 각 오브젝트마다 파서 대응해줘야함.
	virtual void On_Maigc_Throw_Data(void* data) const {}
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

public:
	// 객체의 모든 사운드를 해제
	void Stop_Sound();

protected:
	CTransform*			m_pTransform = { nullptr };

protected:
	_float4x4			m_OffsetMatrix = {};
	const _float4x4*	m_pHitMatrix = { nullptr };
	mutable _bool		m_isDead = { false };
	vector<pair<_int, _float>>		m_SoundChannel = {};
	_float				m_fSoundPower = { 30.f };

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END