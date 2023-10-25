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

class CBeastBalloon_D final : public CGameObject
{
public:
	typedef struct tagBalloonInitDesc
	{
		_int iScore = { 0 };
		_float3 vPosition;
	}BALLOONINITDESC;

	typedef struct tagScoreDesc
	{
		_int Score = { 0 };
		wstring wstrObjectTag = { TEXT("") };
	}SCOREDESC;

private:
	explicit CBeastBalloon_D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBeastBalloon_D(const CBeastBalloon_D& rhs);
	virtual ~CBeastBalloon_D() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShader = { nullptr };
	CShader* m_pShadowShader = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CTexture* m_pEmissiveTexture = { nullptr };

private:
	_int m_iScore = { 0 };

private:
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

public:
	static CBeastBalloon_D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END