#pragma once
#include "Composite.h"

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
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
    
private:
    HRESULT Add_Components();

public:
    static CFlyGameManager* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END