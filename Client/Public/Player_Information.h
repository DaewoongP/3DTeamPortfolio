#pragma once
#include "Composite.h"

BEGIN(Engine)

class CHealth;

END

BEGIN(Client)
class CPotionTap;
class CUI_Group_HP;
class CUI_Group_Finisher;
class CInventory;
class CUI_Group_Finisher_Icon;
class CUI_Group_Potion;
class CTool;

class CPlayer_Information :
    public CComposite
{
public:
    enum PLAYER_ITEM
    {
        HEALING_POTION,         //힐 포션
        ENDURUS_POTION,     //방어력 포션
        MAXIMA_POTION,        //공격력 포션
        FOCUS_POTION,              //쿨타임 포션
        THUNDERBREW_POTION,          //번개구름 생성 포션
        INVISIBILITY_POTION,      //투명해지는 포션
        MANDRAKE,               //맨드레이크
        CHINESES_CHOPPING_CABBAGE,           //깨무는 양배추
        TENTACULAR,             //텐타큘라
        PLAYER_ITEM_END
    };

public:

private:
    explicit CPlayer_Information(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CPlayer_Information(const CPlayer_Information& rhs);
    virtual ~CPlayer_Information() = default;

public:
    //Get,Set

    //아이템 갯수 (enum 참고)
    _uint const * Get_ItemCountPtr(PLAYER_ITEM _eItem) const { return &m_vecItemCount[_eItem]; }
    //현제 채력 비율
    _float Get_Health_Percent() const { return m_pHealth->Get_Current_HP_Percent(); }
    //현제 피니셔 비율
    _float Get_Finisher_Percent() const { return m_pFinisher->Get_Current_HP_Percent(); }
    CHealth* Get_Health() { return m_pHealth; }
    CInventory* Get_Inventory() { return m_pInventory; }
    CPotionTap* Get_PotionTap() { return m_pPotionTap; }

public:
    //test 용
    void Add_Potion();

    CTool* Get_Healpotion();
public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);

public:
     //피니셔 사용 가능 여부
    _bool Is_Use_Fnisher();

    //피니셔 사용시 호출(게이지 감소)
    void Using_Fnisher();

    //피니셔 회복
    void Stack_Finisher() { m_pFinisher->Set_HP(m_pFinisher->Get_HP() + 1); }

    //체력 변경
    void fix_HP(_int _iNum);


private:
    //체력
    CHealth* m_pHealth = { nullptr };
    //피니셔 게이지
    CHealth* m_pFinisher = { nullptr };

    CUI_Group_HP* m_pUI_Health;
    CUI_Group_Finisher* m_pUI_Finisher;
    
    //아이템 갯수를 저장
    vector<_uint> m_vecItemCount = {};

    // 인벤토리
    CInventory* m_pInventory = { nullptr };
    CPotionTap* m_pPotionTap = { nullptr };

    CUI_Group_Finisher_Icon* m_pFinisherIcon = { nullptr };
    CUI_Group_Potion* m_pPotion = { nullptr };

private:
    vector<CTool*> m_vecTool;
    
private:
    HRESULT Add_Components();

   

public:
    static CPlayer_Information* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END