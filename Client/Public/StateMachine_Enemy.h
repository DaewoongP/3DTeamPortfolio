#pragma once
#include "Composite.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CModel;
class CTransform;
class CGameObject;
END

BEGIN(Client)

class CStateMachine_Enemy :
    public CComposite
{
public:
    typedef struct tagStateMachineDesc
    {
        //애니메이션이 끝난것을 확인
        CModel* pOwnerModel = { nullptr };
        CTransform* pTarget = { nullptr };
        _uint* pPhase = { nullptr };
        _uint* pAttackType = { nullptr };
        _bool* pisAttackable = { nullptr };
        _bool* pisTurnable = { nullptr };
        
        _bool IsValid()
        {
            if (nullptr == pOwnerModel) { return false; }
            if (nullptr == pTarget) { return false; }
            if (nullptr == pPhase) { return false; }
            if (nullptr == pAttackType) { return false; }
            if (nullptr == pisAttackable) { return false; }
            if (nullptr == pisTurnable) { return false; }
            return true;
        };
    }STATEMACHINEDESC;

protected:
    explicit CStateMachine_Enemy(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CStateMachine_Enemy(const CStateMachine_Enemy& rhs);
    virtual ~CStateMachine_Enemy() = default;

public:
    HRESULT Set_StateMachine(const _tchar* _pTag);
public:
    HRESULT Set_StateMachineDesc(STATEMACHINEDESC* _pStateMachineDesc);
public:
    virtual HRESULT Initialize(void* pArg);
    virtual void OnStateEnter(void* _pArg = nullptr) {};
    virtual void OnStateTick()  {};
    virtual void OnStateExit()  {};

    virtual void Bind_Notify()  {};

protected:
    STATEMACHINEDESC m_StateMachineDesc = { STATEMACHINEDESC() };

protected:
    void Change_Animation(const wstring& _AnimationTag, _bool _isLumos = true);
    void Change_Animation_Part(const wstring& _AnimationTag, _uint iPartIndex);

public:
    static CStateMachine_Enemy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END