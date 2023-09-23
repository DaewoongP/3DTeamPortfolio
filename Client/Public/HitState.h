#pragma once
#include "StateMachine.h"

BEGIN(Client)

class CHitState :
    public CStateMachine
{
    //�ӽ÷� ���(��������� �𸣰�...)
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

    //�ӽ÷� ����
    _bool m_isPowerfulHit = { true };

private:
    void Go_Idle();

    //�ӽ÷� ����

    //������ ���Ѵ�.
    void Set_Dir();

    //���̵� �Ǵ� �������� ���� ���� ���¸� ���⼭ ó���Ѵ�.

    //�Ƹ�
    //���� ����(�ƾ���, �������)�� �� ������ �ǰ� ����
    
    //���� ���, ū ���
    void Hit_Tick();
    //���ư���?(�̰� ���� ������ ������ ����?)
    void PowerfulHit_Tick();

public:
    static CHitState* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END