#pragma once
#include "Composite.h"

BEGIN(Engine)

class CModel;

END

BEGIN(Client)

class CStateMachine;

class CStateContext :
    public CComposite
{
public:
    enum MOVETYPE
    {
        MOVETYPE_NONE,
        MOVETYPE_WALK,
        MOVETYPE_JOGING,
        MOVETYPE_SPRINT,
        MOVETYPE_END
    };

    enum ACTIONTYPE
    {
        ACTION_NONE,
        ACTION_CASUAL,
        ACTION_CMBT,
        ACTION_END
    };

public:
    typedef struct tagStateContextDesc
    {
        CModel* pOwnerModel = { nullptr };
        _float* pOwnerLookAngle = { nullptr };
        _bool* pIsDirectionPressed = { nullptr };
        CTransform* pPlayerTransform = { nullptr };
        _float* pTargetAngle = { nullptr };
        _float* pClothPowerPlus = { nullptr };
    }STATECONTEXTDESC;

private:
    explicit CStateContext(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CStateContext(const CStateContext& rhs);
    virtual ~CStateContext() = default;

public:
    //Get,Set

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);

public:
    HRESULT Set_StateMachine(const _tchar* _pTag, void * _pArg = nullptr);

private:
    CModel* m_pOwnerModel = { nullptr };
    _float* m_pOwnerLookAngle = { nullptr };
    _bool*  m_pIsDirectionPressed = { nullptr };
    CTransform* m_pPlayerTransform = { nullptr };
    _float* m_pTargetAngle = { nullptr };

    //
    CStateMachine* m_pCurrentStateMachine = { nullptr };

    //달리기와 전력질주 구분용
    _uint m_iMoveSwitch = { (_uint)MOVETYPE_JOGING };

    //액션 구분용
    _uint m_iActionSwitch = { 0 };

    //애니메이션 끝난것을 확인
    _bool m_isFinishAnimation = { false };

    function<void()> m_pfuncFinishAnimation = { nullptr };  //  FinishAnimation();

    //회전 배율
    _float m_fRotaionSpeed = { 0.0f };

    unordered_map<const _tchar*, CStateMachine*> m_pStateMachines;

private:
    void FinishAnimation();

private:
    CStateMachine* Find_StateMachine(const _tchar* _pTag);

    HRESULT Add_StateMachine(const _tchar* _pTag, CStateMachine* _pState);

    HRESULT Ready_StateMachine();

public:
    static CStateContext* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END