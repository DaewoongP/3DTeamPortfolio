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
    const _float3 Get_Eye()const { return m_vEye; }
    const _float3 Get_Up()const { return m_vUp; }
    const _float3 Get_At()const { return m_vAt; }

    const _bool Is_Finish_Animation() const { return m_pAnimCameraModel->Is_Finish_Animation(); }

    void Change_Animation(const wstring& _AnimattionIndex) { m_pAnimCameraModel->Change_Animation(_AnimattionIndex); }
    

public:
    virtual HRESULT Initialize_Protofftype();
    virtual HRESULT Initialize(void* pArg);
    virtual HRESULT Initialize_Level(_uint _iLevelIndex)override;
    virtual void Tick(_float fTimeDelta);
public:
    

private:
    CModel* m_pAnimCameraModel = { nullptr };

private:
    ANIMATION_CAMERA_MODEL_DESC m_Animation_Camera_Model_Desc = ANIMATION_CAMERA_MODEL_DESC();

    _float3 m_vUp = _float3();
    _float3 m_vEye = _float3();
    _float3 m_vAt= _float3();

    //
    const CBone* m_pEyePoint = { nullptr };
    //
    const CBone* m_pUpPoint = { nullptr };
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