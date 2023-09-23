#pragma once
#include "StateMachine.h"

BEGIN(Client)

class CHitState :
    public CStateMachine
{
    //임시로 사용(사용할지도 모르고...)
    enum HITDIRSTATE
    {
        HITDIR_FRONT,
        HITDIR_BACK,
        HITDIR_RIGHT,
        HITDIR_LEFT,
        HITDIR_END
    };
    enum POWERFULHITDIRSTATE
    {
        POWERFULHITDIR_FRONT,
        POWERFULHITDIR_BACK,
        POWERFULHITDIR_END
    };
private:
    explicit CHitState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CHitState(const CHitState& rhs);
    virtual ~CHitState() = default;

public:
    //Get,Set

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
public:
    virtual void OnStateEnter();
    virtual void OnStateTick();
    virtual void OnStateExit();

    virtual void Bind_Notify();

private:
    _uint m_iHitDir = { (_uint)HITDIR_END };
    _uint m_iPowerfulHitDir = { (_uint)POWERFULHITDIR_END };

    //임시로 만듬
    _bool m_isPowerfulHit = { true };

private:
    void Go_Idle();

    //임시로 만듬

    //방향을 구한다.
    void Set_Dir();

    //아이들 또는 여러가지 상태 제어 상태를 여기서 처리한다.

    //아마
    //공중 부유(아씨오, 레비오소)및 이 상태의 피격 상태
    
    //작은 충격, 큰 충격
    void Hit_Tick();
    //날아가기?(이건 적의 공격이 뭔지에 따라?)
    void PowerfulHit_Tick();

public:
    static CHitState* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END