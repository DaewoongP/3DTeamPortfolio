#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRigidBody;
class CRenderer;
class CTexture;
END

BEGIN(Client)
class CBalloon_Timer;
class CBalloon_Coin;
END

BEGIN(Client)

class CBalloon abstract : public CGameObject
{
public:
	enum BALLOONTYPE { TYPE_NONE, TYPE_UP, TYPE_DOWN, TYPE_RIGHT, TYPE_LEFT, TYPE_FORWARD, TYPE_BACKWARD, TYPE_RANDOM, TYPE_END };

	typedef struct tagBalloonInitDesc
	{
		_int iScore = { 0 };
		_float fForce = { 20.f };
		_float3 vPosition;
		_float3 vScale;
	}BALLOONINITDESC;

	typedef struct tagScoreDesc
	{
		/* 여기는 알잘딱으로 추가하세요. */
		_int Score = { 0 };
		wstring wstrObjectTag = { TEXT("") };
	}SCOREDESC;

public:
	_float3 Get_Position() { return m_pTransform->Get_Position(); }
	void	ResetBallon(BALLOONINITDESC InitDesc)
	{
		m_iScore = InitDesc.iScore;
		m_pTransform->Set_Position(InitDesc.vPosition);
		m_pTransform->Set_Scale(InitDesc.vScale);
		m_fForce = InitDesc.fForce;
		m_isDead = false;
		m_isColliderOn = true;
		m_pRigidBody->Enable_Collision("Body", this, nullptr);
	}

protected:
	explicit CBalloon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBalloon(const CBalloon& rhs);
	virtual ~CBalloon() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;

protected:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShader = { nullptr };
	CShader* m_pShadowShader = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CBalloon_Timer* m_pTimer = { nullptr };
	CBalloon_Coin* m_pCoin = { nullptr };

protected:
	_int			m_iScore = { 0 };
	BALLOONTYPE		m_eBallonActionType = { TYPE_NONE };
	_float			m_fForce = { 20.f };
	_bool			m_isColliderOn = { false };

protected:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

private:
	HRESULT Make_Timer();

public:
	virtual void Free() override;
};

END