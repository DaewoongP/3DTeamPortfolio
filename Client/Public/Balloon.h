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

class CBalloon abstract : public CGameObject
{
public:
	enum BALLOONTYPE { TYPE_NONE, TYPE_UP, TYPE_DOWN, TYPE_RIGHT, TYPE_LEFT, TYPE_FORWARD, TYPE_BACKWARD, TYPE_RANDOM, TYPE_END };

	typedef struct tagBalloonInitDesc
	{
		_int iScore = { 0 };
		_float4x4 WorldMatrix;
		_float3 vScale;
		BALLOONTYPE eType;
	}BALLOONINITDESC;

	typedef struct tagScoreDesc
	{
		/* 여기는 알잘딱으로 추가하세요. */
		_int Score = { 0 };
		wstring wstrObjectTag = { TEXT("") };
	}SCOREDESC;

public:
	_float3 Get_Position() {
		return m_pTransform->Get_Position();
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

protected:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShader = { nullptr };
	CShader* m_pShadowShader = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

protected:
	_int m_iScore = { 0 };

protected:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

public:
	virtual void Free() override;
};

END