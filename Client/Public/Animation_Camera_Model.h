#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CModel;
class CBone;
END



BEGIN(Client)

class CAnimation_Camera_Model :
    public CGameObject
{
public:
    typedef struct tagAnimation_Camera_Model_Desc
    {
        CTransform* pTargetTransform = { nullptr };
    }ANIMATION_CAMERA_MODEL_DESC;

private:
    explicit CAnimation_Camera_Model(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CAnimation_Camera_Model(const CAnimation_Camera_Model& rhs);
    virtual ~CAnimation_Camera_Model() = default;

public:
    //Get,Set

public:
    virtual HRESULT Initialize_Protofftype();
    virtual HRESULT Initialize(void* pArg);
    virtual HRESULT Initialize_Level(_uint _iLevelIndex)override;
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
public:
  

private:
    CModel* m_pAnimCameraModel = { nullptr };

private:
    ANIMATION_CAMERA_MODEL_DESC m_Animation_Camera_Model_Desc = ANIMATION_CAMERA_MODEL_DESC();

    _float3 m_vUp = _float3();
    _float3 m_vEye = _float3();
    _float3 m_vAt= _float3();

    //뼈이름이라 소문자 씀
    const CBone* m_pskt_cam = { nullptr };
    //이름은 lookat이지만 룩을 구하는데 쓰이는 up을 구할때 사용한다.
    const CBone* m_plookat = { nullptr };
private:
    HRESULT Add_Components();
    HRESULT Ready_BonData();


private:
    void Update_Up_Eye_At();





public:
    static CAnimation_Camera_Model* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

END