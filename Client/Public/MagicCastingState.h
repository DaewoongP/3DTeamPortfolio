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

    enum BASICSPELLRANDOM
    {
        BASICSPELL_RANDOM_FRONT,
        BASICSPELL_RANDOM_BACK,
        BASICSPELL_RANDOM_END     
    };

    enum SPELLACTION
    {
        SPELL_ACTION_01,
        SPELL_ACTION_02,
        SPELL_ACTION_03,
        SPELL_ACTION_END
    };
public:
    typedef struct tagMagicCastingStateDesc
    {
        //��Ƽ���̿� ���� �Լ�������
        function<void()> pFuncSpell = { nullptr };
    }MAGICCASTINGSTATEDESC;



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
    virtual void OnStateEnter(void* _pArg = nullptr);

    virtual void OnStateTick();
    virtual void OnStateExit();

    virtual void Bind_Notify();

private:
    _uint m_iBasicSpellCombo{ 0 };
    _bool m_isReadySpell{ true };
    _float m_fFixAngleSpeed{ 0.0f };

    _uint m_iBasicSpellRandom{ 0 };

    _bool m_isLastBasicSpellBack = { false };

    vector<wstring> m_vecSpellActionTextList;

    _uint m_iSpellActionIndex = { (_uint)SPELL_ACTION_END };

private:
    void Initialize_BasicSpell_Combo();

    void BasicSpell_Ready();
    
    //Ŭ���� �޺�(��Ÿ �� ���̵�� ���ư��� ���� ��Ƽ������ ��Ȱ)
    void BasicSpell_Tick();

    //�ֹ� ������ ���� ȸ�� ��Ų��.
    void Fix_Angle();

    //�ֹ� ������ �ʱ�ȭ �Ұ� 
    void Initialize_Spell();


    void Spell_Action_Count();

private:
    void Action_Casual_Tick();
    void Action_Cmbt_Tick();

    

    void Go_Idle();

    void Go_Roll();


public:
    static CMagicCastingState* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END