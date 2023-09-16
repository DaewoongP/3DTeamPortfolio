#pragma once

#include "StateMachine.h"

BEGIN(Client)

class CMoveStartState :
    public CStateMachine
{

private:
    explicit CMoveStartState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CMoveStartState(const CMoveStartState& rhs);
    virtual ~CMoveStartState() = default;

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

    void LookFront();

    void Go_Idle();

    void Go_Loop();

    void Over_135();


    void None_Angle();

    void Right_Angle();

    void Left_Angle();

public:
    static CMoveStartState* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END
