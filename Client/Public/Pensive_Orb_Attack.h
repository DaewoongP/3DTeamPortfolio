#pragma once
#include "StateMachine_Enemy.h"

BEGIN(Client)
class CEnergyBall;
END

BEGIN(Client)
class CPensive_Orb_Attack :
    public CStateMachine_Enemy
{
    
private:
    explicit CPensive_Orb_Attack(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CPensive_Orb_Attack(const CPensive_Orb_Attack& rhs);
    virtual ~CPensive_Orb_Attack() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
public:
    virtual void OnStateEnter(void* _pArg = nullptr);
    virtual void OnStateTick();
    virtual void OnStateExit();

private:
    _float  m_fAction_Change_Duration{ 0.0f };
    _uint   m_iMotionIndex = { 0 };
    _tchar  m_wszAnimationTag[3][MAX_PATH];

private:
    // 공을 생성해주겠습니다.(매직볼)
    void Bind_Notify();

public:
    static CPensive_Orb_Attack* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END