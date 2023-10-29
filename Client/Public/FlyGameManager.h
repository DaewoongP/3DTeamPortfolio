#pragma once
#include "Composite.h"

BEGIN(Client)
class CRacer;
class CBalloon;
class CMarioCount;
class CUI_Group_Timer;
class CUI_Group_Score;
END

BEGIN(Client)

// ���� ������ �����ϴ� �ϳ��� ��������.
// ���� �޴����� �����̴� ���� ������Ʈ�� trans�� ��������
// racer��� ��ü�� ����� racer�� trans�� ���󰡰� �������.
// racer�� ���� �浹�� �����ϰ�
// �׷����� ���� ������ ������

class CFlyGameManager :
    public CComposite
{
private:
    explicit CFlyGameManager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CFlyGameManager(const CFlyGameManager& rhs);
    virtual ~CFlyGameManager() = default;

public:
    const vector<CBalloon*>* Get_Balloons() const {
        return &m_pAllBalloonGroup;
    }

public:
    void Add_Score(_uint iNumber,_uint iScore);
    void ReplaceBallon();
    void Racer_AddForce(_uint iNumber, _uint eType, _float fForce);

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
    
private:
    HRESULT Add_Components();

private:
    void Find_Racer_Parent(_uint iLevel, const _tchar* wszLayerTag);

private:
    vector<CRacer*>         m_pRacerGroup = {};
    vector<CGameObject*>    m_pRacerOwnerGroup = {};
    vector<_uint>           m_pScoreGroup = {};

    //Ǯ���� �����ε�, ���� Ŭ���� �� ������ ������� ���ο� ������.
    vector<CBalloon*>        m_pAllBalloonGroup = {};
    vector<wstring>          m_wszBalloonComponentTag = {};

    CUI_Group_Timer*        m_pUiTimer = { nullptr };
    CUI_Group_Score*        m_pUiScore = { nullptr };

    CMarioCount*            m_pCount = { nullptr };
    _float                  m_fInitWaitTime = { 0.f };
    _float                  m_fInitWaitTimeAcc = { 0.f };
    class CEvent_Sky_Enter* m_pEvent = { nullptr };

private:
    //���� ����ð���.
    _float                  m_fGameTimer = { 180.f };
    //���� ���� ������
    _bool                   m_isGameContinue = { false };
    _bool                   m_isGameFinished = { false };

public:
    static CFlyGameManager* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END