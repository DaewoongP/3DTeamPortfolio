#pragma once
#include "Composite.h"
#include "Client_Defines.h"
BEGIN(Engine)

class CModel;
class CTransform;

END

BEGIN(Client)

class CStateMachine :
    public CComposite
{
public:
    typedef struct tagStateMachineDesc
    {
        //애니메이션 끝난것을 확인
        _bool* pisFinishAnimation = { nullptr };//공//
        CModel* pOwnerModel = { nullptr };//플//
        _float* pOwnerLookAngle = { nullptr };//플//
        _bool* pisDirectionPressed = { nullptr };//플//
        CTransform* pPlayerTransform = { nullptr };//플//
        _float* pfTargetAngle = { nullptr };//플//
        //달리기와 전력질주 구분용
        _uint* piMoveType = { nullptr };//공//
        //액션 구분용
        _uint* piActionType= { nullptr };//공//
        //회전 배율
        _float* pfRotaionSpeed = { nullptr };//공
        function<void()> pfuncFinishAnimation = { nullptr };//플
        //루모스 진행중인지 아닌지 
        _bool* pLumosOn = { nullptr };
        CGameObject** ppTarget = { nullptr };

        //값이 비어 있는게 있다면 false 전부 채워져 있다면 true
        _bool IsValid()
        {
            //비어 있다면 채워줘라
            if (nullptr == pisFinishAnimation) { return false; }
            if (nullptr == pOwnerModel) { return false; }
            if (nullptr == pOwnerLookAngle) { return false; }
            if (nullptr == pisDirectionPressed) { return false; }
            if (nullptr == pPlayerTransform) { return false; }
            if (nullptr == pfTargetAngle) { return false; }
            if (nullptr == piMoveType) { return false; }
            if (nullptr == piActionType) { return false; }
            if (nullptr == pfRotaionSpeed) { return false; }
            if (nullptr == pfuncFinishAnimation) { return false; }
            if (nullptr == pLumosOn) { return false; }
            if (nullptr == ppTarget) { return false; }
            
            return true;
        };
    }STATEMACHINEDESC;

protected:
    explicit CStateMachine(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CStateMachine(const CStateMachine& rhs);
    virtual ~CStateMachine() = default;

public:
    HRESULT Set_StateMachine(const _tchar* _pTag);
public:
    HRESULT Set_StateMachineDesc(STATEMACHINEDESC* _pStateMachineDesc);
public:
    virtual HRESULT Initialize(void* pArg);
    virtual void OnStateEnter(void * _pArg = nullptr);
    virtual void OnStateTick()  {};
    virtual void OnStateExit()  {};

    virtual void Bind_Notify()  {};

protected:
    //플레이어에 선언된 구조체를 가지고 오기만 한다.
    STATEMACHINEDESC m_StateMachineDesc = { STATEMACHINEDESC() };

    _float m_f45Angle = { XMConvertToRadians(45.0f) };
    _float m_f135Angle = { XMConvertToRadians(135.0f) };
    

protected:
    void Change_Animation(const wstring& _AnimationTag, _bool _isLumos = true);

public:
    static CStateMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END