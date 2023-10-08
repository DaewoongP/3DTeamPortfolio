#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CPhase : public CComponent
{
public:
	enum STATE { ENTER, STAY, EXIT, STATE_END };
	enum TICK_STATE { TICK, LATE_TICK, TICK_STATE_END };

private:
	explicit CPhase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPhase(const CPhase& rhs);
	virtual ~CPhase() = default;

public:
	void Set_Tick(function<void()> function, TICK_STATE eTickState, STATE eState);
	void Set_ExitCondition(function<bool()> function, STATE eState);
	void Set_TransitionCallBack(function<void()> function, STATE eState);

public:
	void Play();

public:
	HRESULT			Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;

private:
	STATE m_eCurState = { STATE_END };
	array<function<void()>, STATE_END>	m_TickState[TICK_STATE_END];
	array<function<bool()>, STATE_END>	m_TransitionCondition;
	array<function<void()>, STATE_END>	m_TransitionCallBack;

private:
	void State_Tick(TICK_STATE eTickState);
	void Exit_Condition(STATE eNextState);
	void Next_State(STATE eNextState);

private:
	_bool IsStateValid(STATE eState);
	_bool IsTickStateValid(TICK_STATE eState);

public:
	static CPhase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END