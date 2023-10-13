#pragma once
#include "StateMachine.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CRollState :
    public CStateMachine
{
    enum BLINK
    {
        BLINK_START,
        BLINK_END
    };
public:
    struct tagRollStateDesc
    {
        _bool IsBlink;
    };
private:
    explicit CRollState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CRollState(const CRollState& rhs);
    virtual ~CRollState() = default;

public:
    //Get,Set

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
public:
    virtual void OnStateEnter(void* _pArg = nullptr);

    virtual void OnStateTick();
    virtual void OnStateExit();

    virtual void Bind_Notify();

private:
    CRenderer*      m_pRenderer = { nullptr };

private:
    _bool m_isBlink = { true };
    BLINK m_eBlink = { BLINK_END };
private:
    void Go_Idle();


public:
    static CRollState* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END