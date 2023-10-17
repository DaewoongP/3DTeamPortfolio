#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Magic.h"

BEGIN(Engine)
class CRigidBody;
class CRenderer;
class CParticleSystem;
class CTrail;
class CLight;
class CMeshEffect;
END

BEGIN(Client)
class CMagicBall : public CGameObject
{
public:
	//매직볼의 상태임.		시작->트레일->발사->사망->종료
	enum MAGICBALL_STATE { MAGICBALL_STATE_BEGIN, MAGICBALL_STATE_DRAWTRAIL, MAGICBALL_STATE_CASTMAGIC, MAGICBALL_STATE_DYING, MAGICBALL_STATE_END};
	//이펙트 상태임.		완드->발사->사망->종료
	enum EFFECT_STATE{EFFECT_STATE_WAND, EFFECT_STATE_MAIN, EFFECT_STATE_HIT, EFFECT_STATE_END};
public:
	typedef struct MagicBallInitDesc
	{
		const CGameObject*			pTarget = { nullptr };
		const CGameObject*			pWeapon = { nullptr };

		CMagic::MAGIC_GROUP		eMagicGroup = { CMagic::MG_END };
		CMagic::MAGIC_TYPE		eMagicType = { CMagic::MT_END };
		BUFF_TYPE				eBuffType = { BUFF_NONE };
		SPELL					eMagicTag = { SPELL_END };
		COLLISIONFLAG			eCollisionFlag;
		_bool					isChase = { false };
		_int					iDamage = { 0 };
		_float					fLifeTime = { 1.0f };
		_bool					isMouseTarget = { true };
		_float3					vTarget_Dir = {};
	}MAGICBALLINITDESC;
	typedef struct CollsionRequestDesc
	{
		CMagic::MAGIC_GROUP		eMagicGroup = { CMagic::MG_END };
		CMagic::MAGIC_TYPE		eMagicType = { CMagic::MT_END };
		BUFF_TYPE				eBuffType = { BUFF_NONE };
		SPELL					eMagicTag = { SPELL_END };
		_int					iDamage = { 0 };
		CTransform*				pTransform = { nullptr };
		function<void(void*)>		Action = { nullptr };
	}COLLSIONREQUESTDESC;

protected:
	explicit CMagicBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMagicBall(const CMagicBall& rhs);
	virtual ~CMagicBall() = default;

public:
	// 매직볼의 상태를 강제로 세팅합니다.
	void Set_MagicBallState(MAGICBALL_STATE eState) { m_eMagicBallState = eState; m_isFirstFrameInState = true;}
	void Set_MagicBallState_quiet(MAGICBALL_STATE eState) { m_eMagicBallState = eState; m_isFirstFrameInState = false;}
	_float3 Get_MoveDir() { return m_vDir; }
	
	// 매직볼의 상태를 자연스레 다음으로 세팅합니다.
	void Do_MagicBallState_To_Next() { 
		if (m_eMagicBallState != MAGICBALL_STATE_END)
		{
			m_eMagicBallState = static_cast<MAGICBALL_STATE>(m_eMagicBallState + 1);
			m_isFirstFrameInState = true;
			m_isHeavyChange = true;
		}
	}
	void Re_Set_StartEndLerpAcc(_float3 vStart, _float3 vDir);


public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Reset(MAGICBALLINITDESC& InitDesc);

protected:
	virtual void Ready_SpinMove(CTrail* pTrail,_float2 vSpinWeight, _float fSpinSpeed);
	virtual void Ready_SplineMove(CTrail* pTrail, _float3 Aixs = _float3(1,1,1));
	virtual void Ready_SplineMove_Accio(CTrail* pTrail, _float3 Aixs = _float3(1, 1, 1));
	virtual void Ready_StraightMove(CTrail* pTrail);
	virtual void Ready_SplineSpinMove(CTrail* pTrail , _float2 vSpinWeight, _float fSpinSpeed);

// 컴포넌트
protected:
	CRigidBody*				m_pRigidBody = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };
	CLight* m_pLight = { nullptr };
	
//생산성을 높이기위해 도입한 파티클 벡터 특수한 친구들은 여기에 안쓸겁니다.
protected:
	vector<CTrail*>						m_TrailVec[EFFECT_STATE_END];
	vector<CParticleSystem*>			m_ParticleVec[EFFECT_STATE_END];

//발사에 관한 로직
protected:
	//총알에서는 발사 기간이요. 쉴드에서는 지속시간이다.
	_float					m_fLifeTime = { 1.0f };

	const CGameObject*		m_pTarget = { nullptr };

	const _float4x4*		m_pWeaponWorldMatrix = { nullptr };
	const _float4x4*		m_pWeaponOffsetMatrix = { nullptr };
	const _float4x4*		m_pTargetWorldMatrix = { nullptr };
	const _float4x4*		m_pTargetOffsetMatrix = { nullptr };

	// 실시간으로 갱신될 완드와 타겟 위치
	_float4x4				m_CurrentTargetMatrix = {};
	_float4x4				m_CurrentWeaponMatrix = {};
	// 처음에만 기록될 완드와 타겟 위치
	_float3					m_vStartPosition = {};
	_float3					m_vEndPosition = {};

	_float					m_fLerpAcc = { 0.f };
	_bool					m_isChase = { false };

	//For. Spin
	_float2					m_vSpinWeight = {};
	_float					m_fSpinSpeed = { 1.f };

	//For. Spline
	_float3					m_vSplineLerp[2] = {};
	_float					m_fTimeScalePerDitance = { 0.f };
	_uint					m_iLevel = { 0 };

	_bool					m_isMouseTarget = { false };
	_float3					m_vTarget_Dir = {};

	_float3					m_vPostPosition = {};
	_float3					m_vDir = {};

//전달 및 흐름
protected:
	COLLSIONREQUESTDESC		m_CollisionDesc = {};
	COLLISIONFLAG			m_eCollisionFlag = {};

	//행동 계산용임.
	MAGICBALL_STATE			m_eMagicBallState = { MAGICBALL_STATE_BEGIN };
	_bool					m_isFirstFrameInState = { true };

	_float					m_fWandParticleDelayTimer = 0.1f;
	_float					m_fDyingTimer = 5.f;
	_bool					m_isHeavyChange = { false };

protected:
	_float4					m_vLightColor = {};

protected:
	//모든 이펙트 비활성화
	virtual void Ready_Begin();
	//완드 이펙트 활성화 
	virtual void Ready_DrawMagic();
	//메인 이펙트 활성화
	virtual void Ready_CastMagic();
	//히트 이펙트 활성화
	virtual void Ready_Dying();

	//다음 상태로
	virtual void Tick_Begin(_float fTimeDelta);
	//완드 이펙트 재생(다음상태로는 다른곳에서 재생)
	virtual void Tick_DrawMagic(_float fTimeDelta);
	//메인 이펙트 재생(도달/충돌시 다음상태로) / 마법 발사 상태는 다 다르므로 알아서 처리하셈
	virtual void Tick_CastMagic(_float fTimeDelta);
	//히트 이펙트 재생(재생 종료시 사망)
	virtual void Tick_Dying(_float fTimeDelta);

	void Tick_MagicBall_State(_float fTimeDelta);
	void Set_StartPosition();
	
protected:
	HRESULT Add_Components();
	//기본적인 리지드바디 할당임. 크기 바꾸고싶으면 오버라이드 하면됨.
	virtual HRESULT Add_RigidBody();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END		