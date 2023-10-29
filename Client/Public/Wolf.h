#pragma once
/* =============================================== */
//	[CWolf]
// 
//	정 : 심정환 몬스터 데뷔작
//	부 :
//
/* =============================================== */

#include "Enemy.h"
#include "Client_Defines.h"

BEGIN(Client)

class CWolf final : public CEnemy
{
public:
	enum W0LF_ANIMATION {
		WF_ATTACK_START, WF_TRUN_180, WF_ATTACK_END, WF_BARK, WF_HOWL, WF_IDLE, WF_IDLE_END, 
		WF_RUN_START, WF_ATTACK_AFTER_180_TURN, WF_JUMP_R, WF_ATTACK, WF_DIE, WF_GETUP, WF_PROTEGO,
		WF_LEVIOSO_END, WF_LEVIOSO_LOOP, WF_LEVIOSO_START,
		WF_2SPIN, WF2SPIN_2, WF_1SPIN, WF_END
	};

private:
	explicit CWolf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWolf(const CWolf& rhs);
	virtual ~CWolf() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pDissolveTexture = { nullptr };				 // 디졸브용 노이즈 텍스처

private:
	_float m_fDeathTimeAcc = { 0.f }; // 사망 애니메이션이 나오고 얼마 뒤에 죽을 건지
	_float m_fLeviosoDur = { 0.f };   // 레비오소 지속 시간
	_float m_fDecendoDur = { 0.f };   // 디센도 지속 시간
	_float m_fDissolveAmount = { 0.f }; // 디졸브 값

	_bool m_isDecendo = { false }; // 디센도에 피격했는가?
	_bool m_isDissolveStart = { false };


private:
	W0LF_ANIMATION m_eCurrentAnim = { WF_END };
	W0LF_ANIMATION m_ePreAnim = { m_eCurrentAnim }; // 이전 애니메이션이 뭔지

private:
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Bind_HitMatrices();
	HRESULT SetUp_ShaderResources();

private:
	void Wolf_Attack();						// 늑대 공격, Notify
	void Wolf_ChangeAnim();					// 중간 프레임에서 애니메이션 변경, Notify

	void Wolf_Animation();					// 늑대 애니메이션 변경
	void Wolf_Turn(_float fTimeDelta);		// 늑대가 플레이어를 향해 회전
	void Wolf_Levioso(_float fTimeDelta);	// 레비오소 맞았을 때 패턴
	void Wolf_Decendo(_float fTimeDelta);	// 디센도 맞았을 때 패턴
	void Wolf_Protego();					// 프로테고에 막혔을 때 패턴

public:
	static CWolf* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END