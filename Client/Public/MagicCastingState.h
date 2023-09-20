#pragma once
#include "StateMachine.h"

BEGIN(Client)

class CMagicCastingState :
    public CStateMachine
{
    enum BASICSPELLCOMBO
    {
        BASICSPELL_START = 0,
        BASICSPELL_SECOND = 1,
        BASICSPELL_BACKSTEP = 2,
        BASICSPELL_LAST = 3,
        BASICSPELL_END
    };
private:
    explicit CMagicCastingState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CMagicCastingState(const CMagicCastingState& rhs);
    virtual ~CMagicCastingState() = default;

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
    _uint m_iBasicSpellCombo{ 0 };
    _bool m_isReadySpell{ true };

private:
    void Initialize_BasicSpell_Combo();

    void BasicSpell_Ready();
    
    //클릭시 콤보(막타 후 아이들로 돌아가는 것은 노티파이의 역활)
    void BasicSpell_Tick();

   

private:
    void Action_Casual_Tick();
    void Action_Cmbt_Tick();

    

    void Go_Idle();


public:
    static CMagicCastingState* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END