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
        //�ִϸ��̼� �������� Ȯ��
        _bool* pisFinishAnimation = { nullptr };//��//
        CModel* pOwnerModel = { nullptr };//��//
        _float* pOwnerLookAngle = { nullptr };//��//
        _bool* pisDirectionPressed = { nullptr };//��//
        CTransform* pPlayerTransform = { nullptr };//��//
        _float* pfTargetAngle = { nullptr };//��//
        //�޸���� �������� ���п�
        _uint* piMoveType = { nullptr };//��//
        //�׼� ���п�
        _uint* piActionType= { nullptr };//��//
        //ȸ�� ����
        _float* pfRotaionSpeed = { nullptr };//��
        function<void()> pfuncFinishAnimation = { nullptr };//��
        //��� ���������� �ƴ��� 
        _bool* pLumosOn = { nullptr };
        CGameObject** ppTarget = { nullptr };

        //���� ��� �ִ°� �ִٸ� false ���� ä���� �ִٸ� true
        _bool IsValid()
        {
            //��� �ִٸ� ä�����
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
    //�÷��̾ ����� ����ü�� ������ ���⸸ �Ѵ�.
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